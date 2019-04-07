/**
 * desc: smartdb sqlite 接口
 * file: smartdb_sqlite.h
 *
 * author:  myw31415926
 * date:    20190317
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef SMART_DB_SQLITE_H_
#define SMART_DB_SQLITE_H_

#include "variant.h"

#include "sqlite3.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <string>
#include <memory>
#include <cstring>
#include <type_traits>
#include <unordered_map>

namespace smartdb {

// sqlite3事务
#define SMARTDB_BEGIN    "BEGIN"
#define SMARTDB_COMMIT   "COMMIT"
#define SMARTDB_ROLLBACK "ROLLBACK"

//二进制类型，statement支持绑定二进制
struct SqliteBlob
{
    char *buf;
    int  size;
};

class SmartDBSqlite
{
    using SqliteValue = util::Variant<int, uint32_t, sqlite3_int64, sqlite3_uint64,
        double, char*, const char*, std::string, SqliteBlob, std::nullptr_t>;
    using JsonBuilder = rapidjson::Writer<rapidjson::StringBuffer>;
public:
    SmartDBSqlite() : db_handle_(nullptr), sql_stmt_(nullptr), json_builder_(json_sbuf_) {}

    virtual ~SmartDBSqlite()
    {
        Close();
    }

    /**
     * @brief: 打开数据库。如果数据库不存在，则将创建并打开数据库
     * @param[in] db_name: 数据库名称
     * @param[in] userdata: 用户数据
     * @return: 成功返回true; 失败返回false
     */
    bool Open(const std::string& db_name, void* userdata)
    {
        (void)userdata;     // 一般传入用户名和密码，sqlite3不使用，避免unused警告
        err_code_ = sqlite3_open(db_name.c_str(), &db_handle_);
        return (err_code_ == SQLITE_OK);
    }

    /**
     * @brief: 关闭数据库
     * @return: 成功返回true; 失败返回false
     */
    bool Close()
    {
        if (db_handle_ == nullptr) {
            return true;
        }

        sqlite3_finalize(sql_stmt_);
        err_code_ = CloseDBHandle();
        db_handle_ = nullptr;
        sql_stmt_  = nullptr;
        return (err_code_ == SQLITE_OK);
    }

    /**
     * @brief: 执行不带占位符的SQL语句，不返回结果，如insert, update, delete等
     * @param[in] sql: SQL语句
     * @return: 成功返回true; 失败返回false
     */
    bool Excecute(const std::string& sql)
    {
        err_code_ = sqlite3_exec(db_handle_, sql.c_str(), nullptr, nullptr, nullptr);
        return (SQLITE_OK == err_code_);
    }

    /**
     * @brief: 执行带占位符的SQL语句，不返回结果，如insert, update, delete等
     * @param[in] sql: SQL语句
     * @param[in] args: SQL语句参数列表，填充占位符
     * @return: 成功返回true; 失败返回false
     */
    template<typename... Args>
    bool Excecute(const std::string& sql, Args&&... args)
    {
        if (!Prepare(sql)) {
            return false;
        }
        return ExcecuteArgs(std::forward<Args>(args)...);
    }

    /**
     * @brief: 执行简单的汇聚SQL语句，返回函数执行结果
     *         返回结果可能是多种类型，使用variant保存和获取
     * @param[in] sql: SQL语句
     * @param[in] args: SQL语句参数
     * @return: 返回查询结果
     */
    template<typename R, typename... Args>
    R ExcecuteScalar(const std::string& sql, Args... args)
    {
        if (!Prepare(sql)) {
            return GetErrorVal<R>();
        }

        // 绑定SQL脚本中的参数
        if (BindParams(sql_stmt_, 1, std::forward<Args>(args)...) != SQLITE_OK) {
            return GetErrorVal<R>();
        }

        err_code_ = sqlite3_step(sql_stmt_);
        if (err_code_ != SQLITE_ROW) {
            return GetErrorVal<R>();
        }

        SqliteValue val = GetValue(sql_stmt_, 0);
        R ret = val.Get<R>();
        sqlite3_reset(sql_stmt_);

        return ret;
    }

    /**
     * @brief: 执行SQL语句
     * @param[in] sql: SQL语句
     * @param[in] json: SQL语句参数，位于json数组中，格式如下
     *      [{"ID" : 1, "Name" : "name1"}, {"ID" : 1, "Name" : "name1"}]
     * @return: 成功返回true; 失败返回false
     */
    bool ExcecuteJson(const std::string& sql, const char* json)
    {
        // 解析json串
        rapidjson::Document doc;
        //doc.Parse<0>(json);
        doc.Parse(json);
        if (doc.HasParseError()) {
            throw std::logic_error("json parse error: " + std::to_string(doc.GetParseError()));
        }

        if (!doc.IsArray()) {
            throw std::logic_error("json error: json is not the array");
        }

        // 解析SQL语句
        if (!Prepare(sql)) {
            return false;
        }

        // 启动事务写数据
        return JosnTransaction(doc);
    }

    /**
     * @brief: 执行SQL查询语句
     * @param[in] sql: SQL语句
     * @param[in] args: SQL语句参数
     * @return: 返回查询结果，封装到json数组中，格式如下
     *      [{"ID" : 1, "Name" : "name1"}, {"ID" : 1, "Name" : "name1"}]
     */
    template<typename... Args>
    std::shared_ptr<rapidjson::Document> Query(const std::string& sql, Args... args)
    {
        if (!Prepare(sql)) {
            return nullptr;
        }

        if (BindParams(sql_stmt_, 1, std::forward<Args>(args)...) != SQLITE_OK) {
            return nullptr;
        }

        json_sbuf_.Clear();
        json_builder_.Reset(json_sbuf_);
        auto doc = std::make_shared<rapidjson::Document>();

        // 查询DB，构造json数组
        BuildJsonArray();
        doc->Parse<0>(json_sbuf_.GetString());

        return doc;
    }

    /**
     * @brief: 开始事务
     */
    void Begin()
    {
        if (!Excecute(SMARTDB_BEGIN)) {
            throw std::logic_error("transaction begin error: " + std::to_string(err_code_));
        }
    }

    /**
     * @brief: 回滚事务
     */
    void RollBack()
    {
        if (!Excecute(SMARTDB_ROLLBACK)) {
            throw std::logic_error("transaction rollback error: " + std::to_string(err_code_));
        }
    }

    /**
     * @brief: 提交事务
     */
    void Commit()
    {
        if (!Excecute(SMARTDB_COMMIT)) {
            throw std::logic_error("transaction commit error: " + std::to_string(err_code_));
        }
    }

    /**
     * @brief: 获取最近一次错误代码
     * @return: 返回最近一次错误代码
     */
    int GetLastErrCode()
    {
        return err_code_;
    }

private:
    /**
     * @brief: 关闭数据库句柄
     * @return: 返回sqlite3的error code
     */
    int CloseDBHandle()
    {
        int ret_code = sqlite3_close(db_handle_);
        while (ret_code == SQLITE_BUSY) {
            ret_code = SQLITE_OK;
            // 任何与此DB连接相关的SQL语句必须在调用sqlite3_close之前被释放
            sqlite3_stmt* stmt = sqlite3_next_stmt(db_handle_, nullptr);
            if (stmt == nullptr) {
                break;
            }
            ret_code = sqlite3_finalize(stmt);
            if (ret_code == SQLITE_OK) {
                ret_code = sqlite3_close(db_handle_);
            }
        }

        return ret_code;
    }

    /**
     * @brief: 解析和保存SQL语句
     * @param[in] sql: SQL语句，可能带有占位符
     * @return: 成功返回true; 失败返回false
     */
    int Prepare(const std::string& sql)
    {
        // 编译SQL语句
        err_code_ = sqlite3_prepare_v2(db_handle_, sql.c_str(), -1, &sql_stmt_, nullptr);
        if (err_code_ != SQLITE_OK) {
            return false;
        }
        return true;
    }

    /**
     * @brief: 绑定参数，执行SQL语句，必须先调用Prepare
     * @param[in] args: SQL语句参数列表，填充占位符
     * @return: 成功返回true; 失败返回false
     */
    template<typename... Args>
    bool ExcecuteArgs(Args... args)
    {
        if (BindParams(sql_stmt_, 1, std::forward<Args>(args)...) != SQLITE_OK) {
            return false;
        }

        err_code_ = sqlite3_step(sql_stmt_);
        sqlite3_reset(sql_stmt_);   // 重新初始化sqlite3_stmt，方便下次调用
        return (err_code_ == SQLITE_DONE);
    }

    // 统一绑定参数，解析带占位符的SQL语句。终止函数
    int BindParams(sqlite3_stmt* stmt, int current)
    {
        return SQLITE_OK;
    }

    // 统一绑定参数，解析带占位符的SQL语句。模板函数，递归展开
    template<typename T, typename... Args>
    int BindParams(sqlite3_stmt* stmt, int current, T&& first, Args&&... args)
    {
        BindValue(stmt, current, first);    // 绑定基本数据类型
        if (err_code_ != SQLITE_OK) {
            return err_code_;
        }

        BindParams(stmt, current + 1, std::forward<Args>(args)...);
        return err_code_;
    }

    // bind double
    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value>::type
    BindValue(sqlite3_stmt* stmt, int current, T t)
    {
        err_code_ = sqlite3_bind_double(stmt, current, std::forward<T>(t));
    }

    // bind integer(int64 and other)
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value>::type
    BindValue(sqlite3_stmt* stmt, int current, T t)
    {
        BindIntValue(stmt, current, t);
    }

    // bind int64
    template<typename T>
    typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value>::type
    BindIntValue(sqlite3_stmt* stmt, int current, T t)
    {
        err_code_ = sqlite3_bind_int64(stmt, current, std::forward<T>(t));
    }

    // bind other int value
    template<typename T>
    typename std::enable_if<!std::is_same<T, int64_t>::value && !std::is_same<T, uint64_t>::value>::type
    BindIntValue(sqlite3_stmt* stmt, int current, T t)
    {
        err_code_ = sqlite3_bind_int(stmt, current, std::forward<T>(t));
    }

    // bind string
    template<typename T>
    typename std::enable_if<std::is_same<T, std::string>::value>::type
    BindValue(sqlite3_stmt* stmt, int current, const T& t)
    {
        err_code_ = sqlite3_bind_text(stmt, current, t.c_str(), t.length(), SQLITE_TRANSIENT);
    }

    // bind char*
    template<typename T>
    typename std::enable_if<std::is_same<T, char*>::value || std::is_same<T, const char*>::value>::type
    BindValue(sqlite3_stmt* stmt, int current, T t)
    {
        err_code_ = sqlite3_bind_text(stmt, current, t, std::strlen(t) + 1, SQLITE_TRANSIENT);
    }

    // bind blob
    template<typename T>
    typename std::enable_if<std::is_same<T, SqliteBlob>::value>::type
    BindValue(sqlite3_stmt* stmt, int current, const T& t)
    {
        err_code_ = sqlite3_bind_blob(stmt, current, t.buf, t.size, SQLITE_TRANSIENT);
    }

    // bind null
    template<typename T>
    typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type
    BindValue(sqlite3_stmt* stmt, int current, const T t)
    {
        err_code_ = sqlite3_bind_null(stmt, current);
    }

    // 返回无效值
    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    GetErrorVal()
    {
        return T(-1);
    }

    template<typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_same<T, SqliteBlob>::value, T>::type
    GetErrorVal()
    {
        return "";
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, SqliteBlob>::value, T>::type
    GetErrorVal()
    {
        return {nullptr, 0};
    }

    // 取列的值
    SqliteValue GetValue(sqlite3_stmt *stmt, const int& index)
    {
        int type = sqlite3_column_type(stmt, index);

        // 根据列的类型调用对应的函数
        auto it = valmap_.find(type);
        if (it == valmap_.end()) {
            throw std::logic_error("can not find this type: " + std::to_string(type));
        }
        return it->second(stmt, index);
    }

    // 启动事务写数据
    bool JosnTransaction(const rapidjson::Document& doc) {
        Begin();    // 启动事务

        // 解析json对象
        for (auto& v : doc.GetArray()) {
            if (!ExcecuteJson(v)) {   // 对每个doc[index]=value执行SQL语句
                RollBack();
                return false;
            }
        }

        if (err_code_ != SQLITE_DONE) {
            RollBack();
            return false;
        }

        Commit();
        return true;
    }

    // 绑定json对象，并执行SQL语句
    bool ExcecuteJson(const rapidjson::Value& val)
    {
        size_t idx = 1;
        for (auto& v : val.GetObject()) {
            // std::cout << "key=" << v.name.GetString() << std::endl;
            // 绑定json值
            BindJsonValue(v.value, idx++);
        }

        err_code_ = sqlite3_step(sql_stmt_);
        sqlite3_reset(sql_stmt_);
        return (err_code_ == SQLITE_DONE);
    }

    // 绑定json值
    void BindJsonValue(const rapidjson::Value& v, int index)
    {
        auto type = v.GetType();
        if (type == rapidjson::kNullType) {
            err_code_ = sqlite3_bind_null(sql_stmt_, index);
        } else if (type == rapidjson::kStringType) {
            err_code_ = sqlite3_bind_text(sql_stmt_, index, v.GetString(), -1, SQLITE_STATIC);
        } else if (type == rapidjson::kNumberType) {    // int + int64 + double
            if (v.IsInt() || v.IsUint()) {
                err_code_ = sqlite3_bind_int(sql_stmt_, index, v.GetInt());
            } else if (v.IsInt64() || v.IsUint64()) {
                err_code_ = sqlite3_bind_int64(sql_stmt_, index, v.GetInt64());
            } else {
                err_code_ = sqlite3_bind_double(sql_stmt_, index, v.GetDouble());
            }
        } else {
            throw std::invalid_argument("rapidjson can not find this type.");
        }
    }

    // 根据查询结果构造json数组
    void BuildJsonArray()
    {
        // 获取查询结果列数
        int col_count = sqlite3_column_count(sql_stmt_);

        json_builder_.StartArray();
        while (true) {
            err_code_ = sqlite3_step(sql_stmt_);
            if (err_code_ == SQLITE_DONE) {
                break;
            }

            // 构造一行查询结果的json对象
            json_builder_.StartObject();
            for (int i = 0; i < col_count; i++) {
                // 构造key值
                json_builder_.Key(sqlite3_column_name(sql_stmt_, i));
                BuildJsonValue(sql_stmt_, i);
            }
            json_builder_.EndObject();
        }

        json_builder_.EndArray();
        sqlite3_reset(sql_stmt_);
    }

    // 构造jsonvalue值
    void BuildJsonValue(sqlite3_stmt* stmt, int index)
    {
        int type = sqlite3_column_type(stmt, index);
        auto it = buildmap_.find(type);
        if (it == buildmap_.end()) {
            throw std::invalid_argument("sqlite3 query can not find this type.");
        }
        it->second(stmt, index, json_builder_);
    }

private:
    // 错误代码
    int err_code_;

    // sqlite3句柄
    sqlite3*      db_handle_;
    sqlite3_stmt* sql_stmt_;

    rapidjson::StringBuffer json_sbuf_;     // json字符串的buf
    JsonBuilder json_builder_;

    static std::unordered_map<int, std::function<SqliteValue(sqlite3_stmt*, int)>> valmap_;
    static std::unordered_map<int, std::function<void(sqlite3_stmt*, int, JsonBuilder&)>> buildmap_;
};

std::unordered_map<int, std::function<SmartDBSqlite::SqliteValue(sqlite3_stmt*, int)>> 
SmartDBSqlite::valmap_ = {
    { std::make_pair(SQLITE_INTEGER, [](sqlite3_stmt* stmt, int index) {
        return sqlite3_column_int64(stmt, index); }) },
    { std::make_pair(SQLITE_FLOAT, [](sqlite3_stmt* stmt, int index) {
        return sqlite3_column_double(stmt, index); }) },
    { std::make_pair(SQLITE_TEXT, [](sqlite3_stmt* stmt, int index) {
        return std::string((const char*)sqlite3_column_text(stmt, index)); }) },
    { std::make_pair(SQLITE_BLOB, [](sqlite3_stmt* stmt, int index) {
        return std::string((const char*)sqlite3_column_blob(stmt, index)); }) },
    { std::make_pair(SQLITE_NULL, [](sqlite3_stmt* stmt, int index) {
        return nullptr; }) }
};

std::unordered_map<int, std::function<void(sqlite3_stmt*, int, SmartDBSqlite::JsonBuilder&)>>
SmartDBSqlite::buildmap_ = {
    { std::make_pair(SQLITE_INTEGER, 
        [](sqlite3_stmt* stmt, int index, SmartDBSqlite::JsonBuilder& builder) {
            builder.Int64(sqlite3_column_int64(stmt, index)); }) },
    { std::make_pair(SQLITE_FLOAT, 
        [](sqlite3_stmt* stmt, int index, SmartDBSqlite::JsonBuilder& builder) {
            builder.Double(sqlite3_column_double(stmt, index)); }) },
    { std::make_pair(SQLITE_TEXT, 
        [](sqlite3_stmt* stmt, int index, SmartDBSqlite::JsonBuilder& builder) {
            builder.String((const char*)sqlite3_column_text(stmt, index)); }) },
    { std::make_pair(SQLITE_BLOB, 
        [](sqlite3_stmt* stmt, int index, SmartDBSqlite::JsonBuilder& builder) {
            // blob字段要注意获取实际的流长度
            builder.String((const char*)sqlite3_column_blob(stmt, index), sqlite3_column_bytes(stmt, index)); }) },
    { std::make_pair(SQLITE_NULL, 
        [](sqlite3_stmt* stmt, int index, SmartDBSqlite::JsonBuilder& builder) {
            builder.Null(); }) }
};

}
#endif // SMART_DB_SQLITE_H_
