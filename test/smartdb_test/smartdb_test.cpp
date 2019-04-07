/**
 * desc: smartdb 接口测试
 * file: smartdb_test.cpp
 *
 * author:  myw31415926
 * date:    20190323
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cassert>

#include "smartdb_sqlite.h"
#include "util.h"

#include "rapidjson/filereadstream.h"

//////////////////////////////////////////////////////////////
// 创建表
const std::string g_dbname = "test.db";

const std::string sql_create = "\
    CREATE TABLE if not exists Person (\
        ID INTEGER NOT NULL, \
        Name Text, \
        Address BLOB \
    );";

// 创建数据库
void SmartDBCreateTest()
{
    std::shared_ptr<smartdb::SmartDBSqlite> db = std::make_shared<smartdb::SmartDBSqlite>();
    assert(db != nullptr);

    if (!db->Open(g_dbname, nullptr)) {
        std::cerr << "create db[" << g_dbname << "] failed, error code: "
                  << db ->GetLastErrCode() << std::endl;
        return;
    }

    if (!db->Excecute(sql_create)) {
        std::cerr << "create table failed, error code: " << db ->GetLastErrCode()
                  << "\nsql: " << sql_create << std::endl;
        return;
    }
}

//////////////////////////////////////////////////////////////
// 插入数据库
void SmartDBInsertTest(int count)
{
    const std::string sql_insert1 = "INSERT INTO Person(ID, Name, Address) VALUES(0, 'Peter', 'address');";
    const std::string sql_insert2 = "INSERT INTO Person(ID, Name, Address) VALUES(?, ?, ?);";

    std::shared_ptr<smartdb::SmartDBSqlite> db = std::make_shared<smartdb::SmartDBSqlite>();
    assert(db != nullptr);

    if (!db->Open(g_dbname, nullptr)) {
        std::cerr << "create db[" << g_dbname << "] failed, error code: "
                  << db ->GetLastErrCode() << std::endl;
        return;
    }

    if (!db->Excecute(sql_insert1)) {
        std::cerr << "insert table failed, error code: " << db ->GetLastErrCode()
                  << "\nsql: " << sql_insert1 << std::endl;
        return;
    }

    for (int i = 1; i < count; i++) {
        std::string name = "Peter" + std::to_string(i);
        std::string addr = "address - " + std::to_string(i);
        if (!db->Excecute(sql_insert2, i, name, addr)) {
            std::cerr << "insert table failed, error code: " << db ->GetLastErrCode()
                      << "\nsql: " << sql_insert2 << std::endl;
            return;
        }
    }
}

//////////////////////////////////////////////////////////////
// 查询数据库
void SmartDBScalarTest()
{
    const std::string sql_scalar1 = "select count(1) from Person;";
    const std::string sql_scalar2 = "select count(1) from Person where ID=?;";
    const std::string sql_scalar3 = "select max(ID) from Person;";
    const std::string sql_scalar4 = "select max(IDX) from Person;";

    std::shared_ptr<smartdb::SmartDBSqlite> db = std::make_shared<smartdb::SmartDBSqlite>();
    assert(db != nullptr);

    if (!db->Open(g_dbname, nullptr)) {
        std::cerr << "create db[" << g_dbname << "] failed, error code: "
                  << db ->GetLastErrCode() << std::endl;
        return;
    }

    try {
        auto count1 = db->ExcecuteScalar<sqlite3_int64>(sql_scalar1);
        std::cout << "the table[Person] count=" << count1 << std::endl;

        int id = 6;
        auto count2 = db->ExcecuteScalar<sqlite3_int64>(sql_scalar2, id);
        std::cout << "the table[Person] count=" << count2 << " where id=" << id << std::endl;

        auto max1 = db->ExcecuteScalar<sqlite3_int64>(sql_scalar3);
        std::cout << "the table[Person][ID] max=" << max1 << std::endl;

        auto max2 = db->ExcecuteScalar<sqlite3_int64>(sql_scalar4);
        std::cout << "the table[Person][Id] max=" << max2 << std::endl;
    } catch (const std::logic_error& e) {
        std::cerr << "logic_error: " << e.what() << std::endl;
    }
}

//////////////////////////////////////////////////////////////
// insert json接口
void SmartDBInsertJsonTest()
{
    const std::string sql_insert = "INSERT INTO Person(ID, Name, Address) VALUES(?, ?, ?);";

    std::shared_ptr<smartdb::SmartDBSqlite> db = std::make_shared<smartdb::SmartDBSqlite>();
    assert(db != nullptr);

    if (!db->Open(g_dbname, nullptr)) {
        std::cerr << "create db[" << g_dbname << "] failed, error code: "
                  << db ->GetLastErrCode() << std::endl;
        return;
    }

    const char* json_str = "[\
        {\"ID\" : 5000, \"Name\" : \"Peter5000\", \"Address\" : \"address - 5000\"},\
        {\"ID\" : 5001, \"Name\" : \"Peter5001\", \"Address\" : \"address - 5001\"},\
        {\"ID\" : 5002, \"Name\" : \"Peter5002\", \"Address\" : \"address - 5002\"} \
    ]";

    // 从文件中读取json串
    char json_buf[65536];
    FILE* fp = fopen("../smartdb_test/person.json", "r");
    assert(fp != NULL);
    fread(json_buf, 1, sizeof(json_buf), fp);
    //std::cout << "person.json : \n" << json_buf << std::endl;

    try {
        db->ExcecuteJson(sql_insert, json_str);
        db->ExcecuteJson(sql_insert, json_buf);
    } catch (const std::logic_error& e) {
        std::cerr << "logic_error: " << e.what() << std::endl;
    }

}

//////////////////////////////////////////////////////////////
// 数据库事务
void SmartDBTransactionTest(int count)
{
    const std::string sql_insert = "INSERT INTO Person(ID, Name, Address) VALUES(?, ?, ?);";

    std::shared_ptr<smartdb::SmartDBSqlite> db = std::make_shared<smartdb::SmartDBSqlite>();
    assert(db != nullptr);

    if (!db->Open(g_dbname, nullptr)) {
        std::cerr << "create db[" << g_dbname << "] failed, error code: "
                  << db ->GetLastErrCode() << std::endl;
        return;
    }

    try {
        db->Begin();

        for (int i = 1; i < count; i++) {
            std::string name = "Peter" + std::to_string(i);
            std::string addr = "address - " + std::to_string(i);
            if (!db->Excecute(sql_insert, i, name, addr)) {
                std::cerr << "insert table failed, error code: " << db ->GetLastErrCode()
                          << "\nsql: " << sql_insert << std::endl;
                db->RollBack();     // 失败要回滚
                return;
            }
        }

        db->Commit();
    } catch (const std::logic_error& e) {
        std::cerr << "logic_error: " << e.what() << std::endl;
    }
}

//////////////////////////////////////////////////////////////
// 查询数据库


void SmartDBQueryTest()
{
    const std::string sql_query1 = "SELECT * FROM Person;";
    const std::string sql_query2 = "SELECT * FROM Person WHERE ID=?;";

    std::shared_ptr<smartdb::SmartDBSqlite> db = std::make_shared<smartdb::SmartDBSqlite>();
    assert(db != nullptr);

    if (!db->Open(g_dbname, nullptr)) {
        std::cerr << "create db[" << g_dbname << "] failed, error code: "
                  << db ->GetLastErrCode() << std::endl;
        return;
    }

    try {
        std::shared_ptr<rapidjson::Document> doc1 = db->Query(sql_query1);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc1->Accept(writer);
        std::cout << "query json:\n" << buffer.GetString() << std::endl;

        // 复用前需要清空buffer，重置writer
        buffer.Clear();
        writer.Reset(buffer);

        std::shared_ptr<rapidjson::Document> doc2 = db->Query(sql_query2, 5000);
        doc2->Accept(writer);
        std::cout << "query json:\n" << buffer.GetString() << std::endl;
    } catch (const std::logic_error& e) {
        std::cerr << "logic_error: " << e.what() << std::endl;
    }
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "\n*** SmartDBCreateTest ***" << std::endl;
    util::TimeSpan ts;
    SmartDBCreateTest();
    std::cout << "run time: " << ts.Span() << " ms" << std::endl;

    std::cout << "\n*** SmartDBInsertTest ***" << std::endl;
    ts.Reset();
    int count = 10;
    if (argc > 1) {
        count = std::atoi(argv[1]);
    }
    SmartDBInsertTest(count);
    std::cout << "run time: " << ts.Span() << " ms, count = " << count << std::endl;

    std::cout << "\n*** SmartDBScalarTest ***" << std::endl;
    ts.Reset();
    SmartDBScalarTest();
    std::cout << "run time: " << ts.Span() << " ms" << std::endl;

    std::cout << "\n*** SmartDBInsertJsonTest ***" << std::endl;
    ts.Reset();
    SmartDBInsertJsonTest();
    std::cout << "run time: " << ts.Span() << " ms" << std::endl;

    std::cout << "\n*** SmartDBTransactionTest ***" << std::endl;
    ts.Reset();
    SmartDBTransactionTest(count * 10000);
    std::cout << "run time: " << ts.Span() << " ms" << std::endl;

    std::cout << "\n*** SmartDBQueryTest ***" << std::endl;
    ts.Reset();
    SmartDBQueryTest();
    std::cout << "run time: " << ts.Span() << " ms" << std::endl;

    return 0;
}
