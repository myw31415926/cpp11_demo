/**
 * desc: rapidjson_test 测试
 * file: rapidjson_test.cpp
 *
 * author:  myw31415926
 * date:    20190327
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>

///////////////////////////////////////////////////////////////////////
// 读json串
void ReadJsonTest()
{
    const char* json_str1 = "{\"ID\" : 5000, \"Name\" : \"Peter5000\", \"Address\" : \"address - 5000\"}";
    const char* json_str2 = "{\
        \"1\" : {\"ID\" : 5000, \"Name\" : \"Peter5000\", \"Address\" : \"address - 5000\"},\
        \"2\" : {\"ID\" : 5001, \"Name\" : \"Peter5001\", \"Address\" : \"address - 5001\"},\
        \"3\" : {\"ID\" : 5002, \"Name\" : \"Peter5002\", \"Address\" : \"address - 5002\"} \
    }";

    rapidjson::Document doc1;
    doc1.Parse(json_str1);

    for (auto& v : doc1.GetObject()) {
        std::cout << "key=" << v.name.GetString()
                  << ", type=" << v.value.GetType() << ", value=";
        if (v.value.IsInt()) {
            std::cout << v.value.GetInt() << std::endl;
        } else if (v.value.IsString()) {
            std::cout << v.value.GetString() << std::endl;
        }
    }

    rapidjson::Document doc2;
    doc2.Parse(json_str2);

    for (auto& v : doc2.GetObject()) {
        if (v.value.IsObject()) {
            std::cout << "key=" << v.name.GetString() << std::endl;
            for (auto& m : v.value.GetObject()) {
                if (m.value.IsInt()) {
                    std::cout << "\t" << m.name.GetString() << " : " << m.value.GetInt() << std::endl;
                } else if (m.value.IsString()) {
                    std::cout << "\t" <<  m.name.GetString() << " : " << m.value.GetString() << std::endl;
                }
            }
        } else {
            std::cout << "key=" << v.name.GetString() << ", type=" << v.value.GetType() << std::endl;
        }
    }
}

///////////////////////////////////////////////////////////////////////
// 写json串
void WriteJsonTest1()
{
    rapidjson::Document doc1;
    std::ofstream out;

    doc1.SetObject();
    doc1.AddMember("ID", 6000, doc1.GetAllocator());
    doc1.AddMember("Name", "Peter6000", doc1.GetAllocator());
    doc1.AddMember("Address", "Address - 6000", doc1.GetAllocator());

    // 写入到输出流
    rapidjson::StringBuffer buffer1;
    rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
    doc1.Accept(writer1);
    std::cout << "json1: \n" << buffer1.GetString();

    out.open("test1.json", std::ios::out);
    out << buffer1.GetString();
    out.close();

    rapidjson::Document doc2;
    doc2.SetObject();

    for (int i = 1; i < 100; i++) {
        std::string      str;
        rapidjson::Value val(rapidjson::kStringType);
        rapidjson::Value obj(rapidjson::kObjectType);

        obj.AddMember("ID", 6000 + i, doc2.GetAllocator());

        str = "Peter600" + std::to_string(i);
        val.SetString(str.c_str(), doc2.GetAllocator());
        obj.AddMember("Name", val, doc2.GetAllocator());

        str = "Address - 600" + std::to_string(i);
        val.SetString(str.c_str(), doc2.GetAllocator());
        obj.AddMember("Address", val, doc2.GetAllocator());

        // 构造外层key值，插入obj对象
        rapidjson::Value key(std::to_string(i).c_str(), doc2.GetAllocator());
        doc2.AddMember(key, obj, doc2.GetAllocator());
    }

    rapidjson::StringBuffer buffer2;
    rapidjson::Writer<rapidjson::StringBuffer> writer2(buffer2);
    doc2.Accept(writer2);
    std::cout << "json1: \n" << buffer2.GetString();

    out.open("test2.json", std::ios::out);
    out << buffer2.GetString();
    out.close();
}

///////////////////////////////////////////////////////////////////////
// 写json串
/**
 * 生成json串
 *  {  
 *      "id":"000001",
 *      "name":"Peter",
 *      "flag":true,
 *      "tags":["tag1", "tag2", "tag3"],
 *      "urls": {
 *          "success_url":"http://www.success_url.com",
 *          "cannel_url":"http://www.cannel_url.com"
 *      },
 *      "info": {
 *          "desc":"description",
 *          "date":"2019-03-16",
 *          "price":{
 *              "amount":8.8,
 *              "currency":"CNY"
 *          }
 *      }
 *  }
 */
static const char KEY_ID[] = "id";
static const char KEY_NAME[] = "name";
static const char KEY_FLAG[] = "flag";
static const char KEY_TAGS[] = "tags";
static const char KEY_URLS[] = "urls";
static const char KEY_URLS_SUCCESS[] = "success_url";
static const char KEY_URLS_CANNEL[]  = "cannel_url";
static const char KEY_INFO[] = "info";
static const char KEY_INFO_DESC[]  = "desc";
static const char KEY_INFO_DATE[]  = "date";
static const char KEY_INFO_PRICE[] = "price";
static const char KEY_INFO_PRICE_AMOUNT[]   = "amount";
static const char KEY_INFO_PRICE_CURRENCY[] = "currency";

void WriteJsonTest2()
{
    static std::map<std::string, std::string> data_map {
        {KEY_ID, "000001"},
        {KEY_NAME, "Peter"},
        {KEY_FLAG, "1"},
        {KEY_TAGS, "tag"},
        {KEY_URLS_SUCCESS, "http://www.success_url.com"},
        {KEY_URLS_CANNEL, "http://www.cannel_url.com"},
        {KEY_INFO_DESC, "description"},
        {KEY_INFO_DATE, "2019-03-16"},
        {KEY_INFO_PRICE_AMOUNT, "8.8"},
        {KEY_INFO_PRICE_CURRENCY, "CNY"},
    };

    std::ofstream out;
    rapidjson::Document doc(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    rapidjson::Value tags_obj(rapidjson::kArrayType);
    rapidjson::Value urls_obj(rapidjson::kObjectType);
    rapidjson::Value price_obj(rapidjson::kObjectType);
    rapidjson::Value info_obj(rapidjson::kObjectType);

    // 对于字符指针，RapidJSON需要用StringRef函数作一个标记，代表它不复制也是安全的
    doc.AddMember(KEY_ID, rapidjson::StringRef(data_map[KEY_ID].c_str()), allocator);
    doc.AddMember(KEY_NAME, rapidjson::StringRef(data_map[KEY_NAME].c_str()), allocator);
    doc.AddMember(KEY_FLAG, (std::atoi(data_map[KEY_FLAG].c_str()) == 1), allocator);

    for (int i = 1; i <=3; i++) {
        std::string str_tag = data_map[KEY_TAGS] + std::to_string(i);
        rapidjson::Value val(str_tag.c_str(), allocator);
        tags_obj.PushBack(val, allocator);
    }
    doc.AddMember(KEY_TAGS, tags_obj, allocator);

    urls_obj.AddMember(KEY_URLS_SUCCESS,
        rapidjson::StringRef(data_map[KEY_URLS_SUCCESS].c_str()), allocator);
    urls_obj.AddMember(KEY_URLS_CANNEL,
        rapidjson::StringRef(data_map[KEY_URLS_CANNEL].c_str()), allocator);
    doc.AddMember(KEY_URLS, urls_obj, allocator);

    price_obj.AddMember(KEY_INFO_PRICE_AMOUNT,
        std::atof(data_map[KEY_INFO_PRICE_AMOUNT].c_str()), allocator);
    price_obj.AddMember(KEY_INFO_PRICE_CURRENCY,
        rapidjson::StringRef(data_map[KEY_INFO_PRICE_CURRENCY].c_str()), allocator);

    info_obj.AddMember(KEY_INFO_DESC,
        rapidjson::StringRef(data_map[KEY_INFO_DESC].c_str()), allocator);
    info_obj.AddMember(KEY_INFO_DATE,
        rapidjson::StringRef(data_map[KEY_INFO_DATE].c_str()), allocator);
    info_obj.AddMember(KEY_INFO_PRICE, price_obj, allocator);

    doc.AddMember(KEY_INFO, info_obj, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::cout << "json: \n" << buffer.GetString();

    out.open("test3.json", std::ios::out);
    out << buffer.GetString();
    out.close();
}

///////////////////////////////////////////////////////////////////////
// 写json串
void WriteJsonTest3()
{
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("hello");
    writer.String("world");

    writer.Key("t");
    writer.Bool(true);

    writer.Key("f");
    writer.Bool(false);

    writer.Key("n");
    writer.Null();

    writer.Key("i");
    writer.Int(123);

    writer.Key("pi");
    writer.Double(3.1416);

    writer.Key("a");
    writer.StartArray();
    for (unsigned i = 0; i < 4; i++)
        writer.Uint(i);
    writer.EndArray();

    writer.Key("array");
    writer.StartArray();
    for (unsigned i = 0; i < 4; i++) {
        std::string str;
        writer.StartObject();

        writer.Key("ID");
        writer.Uint(i + 1000);

        str = "Peter" + std::to_string(i + 1000);
        writer.Key("Name");
        writer.String(str.c_str());

        str = "Address" + std::to_string(i + 1000);
        writer.Key("Address");
        writer.String(str.c_str());

        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();

    std::cout << s.GetString() << std::endl;
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** ReadJsonTest ***" << std::endl;
    ReadJsonTest();

    std::cout << "\n*** WriteJsonTest1 ***" << std::endl;
    WriteJsonTest1();

    std::cout << "\n*** WriteJsonTest2 ***" << std::endl;
    WriteJsonTest2();

    std::cout << "\n*** WriteJsonTest3 ***" << std::endl;
    WriteJsonTest3();

    return 0;
}
