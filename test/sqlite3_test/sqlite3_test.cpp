/**
 * desc: sqlite3 接口测试
 * file: sqlite3_test.cpp
 *
 * author:  myw31415926
 * date:    20190317
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "sqlite3.h"

#include <iostream>
#include <string>
#include <cstring>

//////////////////////////////////////////////////////////////
// 创建表
const char* sql_create = "\
    CREATE TABLE if not exists Person (\
        ID INTEGER NOT NULL, \
        Name Text, \
        Address BLOB \
    );";

// 创建数据库
void Sqlite3CreateTest()
{
    sqlite3* db_handle = NULL;

    // 打开数据库
    int ret = sqlite3_open("test.db", &db_handle);
    if (ret != SQLITE_OK) {
        sqlite3_close(db_handle);
        return;
    }

    // 创建表
    ret = sqlite3_exec(db_handle, sql_create, NULL, NULL, NULL);
    if (ret != SQLITE_OK) {
        std::cout << "create table[Person] faled" << std::endl;
    }

    // 关闭数据库
    sqlite3_close(db_handle);
}

//////////////////////////////////////////////////////////////
// 插入数据库
const char* sql_insert = "INSERT INTO Person(ID, Name, Address) VALUES(?, ?, ?);";

void Sqlite3InsertTest()
{
    sqlite3* db_handle = NULL;

    // 打开数据库
    int ret = sqlite3_open("test.db", &db_handle);
    if (ret != SQLITE_OK) {
        sqlite3_close(db_handle);
        return;
    }

    // 解析插入数据的SQL脚本
    sqlite3_stmt* stmt = NULL;
    ret = sqlite3_prepare_v2(db_handle, sql_insert, std::strlen(sql_insert), &stmt, NULL);
    if (ret != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db_handle);
        return;
    }

    int id = 2;
    const char* name = "Peter";
    for (int i = 0; i < 10; i++) {
        // 绑定参数
        sqlite3_bind_int(stmt, 1, id);
        // SQLITE_STATIC 告诉sqlite3_bind_text函数字符串为常量，可以放心使用
        // SQLITE_TRANSIENT 会对字符串做一份拷贝
        // 一般使用这两个常量参数来调用sqlite3_bind_text。
        sqlite3_bind_text(stmt, 2, name, std::strlen(name), SQLITE_TRANSIENT);
        sqlite3_bind_null(stmt, 3);

        // 执行SQL语句
        ret = sqlite3_step(stmt);
        if (ret != SQLITE_DONE) {
            std::cout << "sqlite3_step faled. return " << ret << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db_handle);
            return;
        }

        // 重新初始化sqlite3_stmt，方便下次调用
        sqlite3_reset(stmt);
    }
    // stmt用完后要释放，否则会内存泄露
    sqlite3_finalize(stmt);

    // 关闭数据库
    sqlite3_close(db_handle);
}

//////////////////////////////////////////////////////////////
// 查询数据库
const char* sql_query = "SELECT * FROM Person;";

void Sqlite3QueryTest()
{
    sqlite3* db_handle = NULL;

    // 打开数据库
    int ret = sqlite3_open("test.db", &db_handle);
    if (ret != SQLITE_OK) {
        sqlite3_close(db_handle);
        return;
    }

    // 解析插入数据的SQL脚本
    sqlite3_stmt* stmt = NULL;
    ret = sqlite3_prepare_v2(db_handle, sql_query, std::strlen(sql_query), &stmt, NULL);
    if (ret != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db_handle);
        return;
    }

    // 获取列数
    int col_count = sqlite3_column_count(stmt);
    while (true) {
        // 执行SQL语句，获取每行的数据
        ret = sqlite3_step(stmt);
        if (ret == SQLITE_DONE) {
            std::cout << "sqlite3_step return SQLITE_DONE" << std::endl;
            break;  // 数据已经获取完，跳出循环
        } else if (ret != SQLITE_ROW) {
            std::cout << "sqlite3_step return SQLITE_ROW" << std::endl;
            break;  // 获取某一行数据失败，跳出循环
        }

        for (int i = 0; i < col_count; i++) {
            // 需要判断当前列的类型，根据类型调用对应的API
            int col_type = sqlite3_column_type(stmt, i);
            if (col_type == SQLITE_INTEGER) {
                int val = sqlite3_column_int(stmt, i);
                std::cout << "int value = " << val << std::endl;
            }else if (col_type == SQLITE_FLOAT) {
                double val = sqlite3_column_double(stmt, i);
                std::cout << "double value = " << val << std::endl;
            }else if (col_type == SQLITE_TEXT) {
                const char* val = (const char*)sqlite3_column_text(stmt, i);
                std::cout << "text value = " << val << std::endl;
            }else if (col_type == SQLITE_NULL) {
                std::cout << "null value" << std::endl;
            }
        }
    }
    // stmt用完后要释放，否则会内存泄露
    sqlite3_finalize(stmt);

    // 关闭数据库
    sqlite3_close(db_handle);
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "\n*** Sqlite3CreateTest ***" << std::endl;
    Sqlite3CreateTest();

    std::cout << "\n*** Sqlite3InsertTest ***" << std::endl;
    Sqlite3InsertTest();

    std::cout << "\n*** Sqlite3QueryTest ***" << std::endl;
    Sqlite3QueryTest();

    return 0;
}
