/**
 * desc: IoC容器模板测试
 * file: ioc_container_test.cpp
 *
 * author:  myw31415926
 * date:    20190306
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "ioc_container.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////
struct Bus
{
    Bus() {}
    void Test() const { std::cout << "Bus::Test()" << std::endl; }
};

struct Car
{
    Car(int n, double f) : n_(n), f_(f) {}
    void Test() const { std::cout << "Car::Test(): n = " << n_ << ", f = " << f_ << std::endl; }

private:
    int    n_;
    double f_;
};

struct Base
{
    virtual ~Base() {}
    virtual void Func() = 0;
};

struct DerivedB : public Base
{
    void Func() override
    {
        std::cout << "call func in DerivedB" << std::endl;
    }
};

struct DerivedC : public Base
{
    DerivedC(int n) : n_(n) {}

    void Func() override
    {
        std::cout << "call func in DerivedC, n = " << n_ << std::endl;
    }

private:
    int n_;
};

struct DerivedD : public Base
{
    DerivedD(int n, const std::string& s) : n_(n), str_(s) {}

    void Func() override
    {
        std::cout << "call func in DerivedD, n = " << n_ << ", str = " << str_ << std::endl;
    }

private:
    int n_;
    std::string str_;
};

struct SA
{
    SA(Base* p) : ptr(p) {}

    ~SA()
    {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }

    void Func()
    {
        std::cout << "SA Func" << std::endl;
        ptr->Func();
    }
private:
    Base* ptr;
};

void IocContainerTest()
{
    util::IocContainer ioc;

    ioc.RegisterSimple<Bus>("bus");
    auto bus = ioc.ResolveShared<Bus>("bus");
    bus->Test();

    ioc.RegisterSimple<Car, int, double>("car");
    auto car = ioc.ResolveShared<Car>("car", 10, 20.02);
    car->Test();

    ioc.Register<Base, DerivedB>("B");
    ioc.Register<Base, DerivedC, int>("C");
    ioc.Register<Base, DerivedD, int, std::string>("D");

    auto pbb = ioc.ResolveShared<Base>("B");
    pbb->Func();
    auto pbc = ioc.ResolveShared<Base>("C", 100);
    pbc->Func();
    std::string str = "string200";  // 不能直接写入字符串常量，否则参数会被解析成char*
    auto pbd = ioc.ResolveShared<Base>("D", 200, str);
    pbd->Func();

    ioc.Register<SA, DerivedB>("AB");
    ioc.Register<SA, DerivedC, int>("AC");
    ioc.Register<SA, DerivedD, int, std::string>("AD");

    auto pab = ioc.ResolveShared<SA>("AB");
    pab->Func();
    auto pac = ioc.ResolveShared<SA>("AC", 1000);
    pac->Func();
    str = "string2000";
    auto pad = ioc.ResolveShared<SA>("AD", 2000, str);
    pad->Func();
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** IocContainerTest ***" << std::endl;
    IocContainerTest();

    return 0;
}
