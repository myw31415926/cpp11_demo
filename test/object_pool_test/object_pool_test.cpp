/**
 * desc: 对象池ObjectPool 测试
 * file: object_pool_test.cpp
 *
 * author:  myw31415926
 * date:    20190307
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "object_pool.h"
#include <iostream>

struct BigObject
{
    BigObject() :n1_(0), n2_(0) {}

    BigObject(const int n1, const int n2) :n1_(n1), n2_(n2) {}

    BigObject(const std::string name) :n1_(0), n2_(0), name_(name) {}

    ~BigObject() { std::cout << "~BigObject : name = " << name_ << std::endl; }

    void Print(const std::string& str)
    {
        if (name_.empty()) name_ = str;

        std::cout << str << " - name[" << name_ 
                  << "], n1[" << n1_ << "], n2[" << n2_ << "]" << std::endl;;
    }
private:
    int n1_, n2_;
    std::string name_;
};

void Print(std::shared_ptr<BigObject> obj, const std::string& str)
{
    if (obj != nullptr) {
        obj->Print(str);
    } else {
        std::cerr << str << ": object is null" << std::endl;
    }
}

void ObjectPoolTest()
{
    util::ObjectPool<BigObject> pool;
    pool.Init(2);   // 初始化对象池，创建2个对象
    {
        // 出了作用域后，对象池返回的对象会自动回收
        auto p1 = pool.Get();
        Print(p1, "p1");
        auto p2 = pool.Get();
        Print(p2, "p2");
    }

    auto p1 = pool.Get();
    auto p2 = pool.Get();
    auto p3 = pool.Get();   // nullptr
    Print(p1, "p1");
    Print(p2, "p2");
    Print(p3, "p3");

    // 对象池支持重载构造
    pool.Init(2, 100, 200);
    auto p4 = pool.Get<int, int>();
    Print(p4, "p4");

    std::string str = "object_pool_test";
    pool.Init(2, str);
    auto p5 = pool.Get<std::string&>();     // 需要引用
    Print(p5, "p5");
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** ObjectPoolTest ***" << std::endl;
    ObjectPoolTest();

    return 0;
}
