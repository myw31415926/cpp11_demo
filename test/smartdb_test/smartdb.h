/**
 * desc: smartdb 接口
 * file: smartdb.h
 *
 * author:  myw31415926
 * date:    20190317
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef SMART_DB_H_
#define SMART_DB_H_

#include "rapidjson/document.h"

#include <string>
#include <memory>

namespace smartdb {

class SmartDB
{
public:
    SmartDB();
    virtual ~SmartDB();

    /**
     * @brief: 打开数据库。如果数据库不存在，则将创建并打开数据库
     * @param[in] db_name: 数据库名称
     * @param[in] userdata: 用户数据
     * @return: 成功返回true; 失败返回false
     */
    virtual bool Open(const std::string& db_name, void* userdata);

    /**
     * @brief: 关闭数据库
     * @return: 成功返回true; 失败返回false
     */
    virtual bool Close();

    /**
     * @brief: 执行不带占位符的SQL语句，不返回结果，如insert, update, delete等
     * @param[in] sql: SQL语句
     * @return: 成功返回true; 失败返回false
     */
    virtual bool Excecute(const std::string& sql);

    /**
     * @brief: 执行SQL语句
     * @param[in] sql: SQL语句
     * @param[in] args: SQL语句参数
     * @return: 成功返回true; 失败返回false
     */
    template<typename... Args>
    virtual bool Excecute(const std::string& sql, Args&&... args);

    /**
     * @brief: 执行SQL语句
     * @param[in] sql: SQL语句
     * @param[in] t: SQL语句参数，封装在tuple容器中
     * @return: 成功返回true; 失败返回false
     */
    template<typename Tuple>
    virtual bool ExcecuteTuple(const std::string& sql, Tuple&& t);

    /**
     * @brief: 执行SQL语句
     * @param[in] sql: SQL语句
     * @param[in] json: SQL语句参数，封装在json字符串中
     * @return: 成功返回true; 失败返回false
     */
     virtual bool ExcecuteJson(const std::string& sql, const char* json);

    /**
     * @brief: 执行SQL语句（汇聚函数，如max, count），返回结果
     * @param[in] sql: SQL语句
     * @param[in] args: SQL语句参数
     * @return: 返回查询结果
     */
    template<typename R, typename... Args>
    virtual R ExcecuteScalar(const std::string& sql, Args... args);

    /**
     * @brief: 执行SQL查询语句
     * @param[in] sql: SQL语句
     * @param[in] args: SQL语句参数
     * @return: 返回查询结果，封装到json对象中
     */
    template<typename... Args>
    virtual std::shared_ptr<rapidjson::Document> Query(const std::string& sql, Args... args);

    /**
     * @brief: 开始事务
     * @return: 成功返回true; 失败返回false
     */
    virtual bool Begin();

    /**
     * @brief: 回滚事务
     * @return: 成功返回true; 失败返回false
     */
    virtual bool RollBack();

    /**
     * @brief: 提交事务
     * @return: 成功返回true; 失败返回false
     */
    virtual bool Commit();

    /**
     * @brief: 获取最近一次错误代码
     * @return: 返回最近一次错误代码
     */
    int GetLastErrCode()
    {
        return err_code_;
    }

protected:
    /**
     * 禁止复制和赋值，继承此基类的对象都不能复制和赋值
     */
    SmartDB(const SmartDB&) = delete;
    SmartDB& operator=(const SmartDB&) = delete;

protected:
    // 错误代码
    int err_code_;
};

}
#endif // SMART_DB_H_
