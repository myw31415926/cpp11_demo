/**
 * desc: 单例模板测试
 * file: singleton_test.cpp
 *
 * author:  myw31415926
 * date:    20190306
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "singleton.h"

#include <iostream>
#include <string>

struct S1
{
    S1(const std::string& str) { std::cout << "S1 lvalue " << str << std::endl; }
    S1(std::string&& str) { std::cout << "S1 rvalue " << str << std::endl; }
};

struct S2
{
    S2(const std::string& str) { std::cout << "S2 lvalue " << str << std::endl; }
    S2(std::string&& str) { std::cout << "S2 rvalue " << str << std::endl; }
};

struct S3
{
    S3(int x, double y) : x_(x), y_(y) {}

    void Print() { std::cout << "S3 x = " << x_ << ", y = " << y_ << std::endl; }
private:
    int x_;
    double y_;
};

void SingletonTest()
{
    std::string str = "singleton test";

    // 左值创建S1类型的单例
    util::Singleton<S1>::Instance(str);

    std::cout << "str = " << str << std::endl;
    // 右值创建S2类型的单例
    util::Singleton<S2>::Instance(std::move(str));
    std::cout << "str = " << str << std::endl;

    // 创建S3类型的单例，含2个参数
    util::Singleton<S3>::Instance(1, 3.1415926);
    util::Singleton<S3>::GetInstance()->Print();

    // 释放单例
    util::Singleton<S1>::Destroy();
    util::Singleton<S2>::Destroy();
    util::Singleton<S3>::Destroy();
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** SingletonTest ***" << std::endl;
    SingletonTest();

    return 0;
}
