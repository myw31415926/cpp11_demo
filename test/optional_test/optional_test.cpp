/**
 * desc: 可变参数模板测试
 * file: optional_test.h
 *
 * author:  myw31415926
 * date:    20190226
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "optional.h"
#include "lazy.h"

#include <iostream>
#include <string>
#include <memory>

//////////////////////////////////////////////////////////////
// Optional test
struct Mystruct
{
    int num1;
    int num2;

    Mystruct (int n1, int n2) : num1(n1), num2(n2) {}
};

void OptionalTest()
{
    util::Optional<std::string> s1("ok1");
    util::Optional<std::string> s2("ok2");
    util::Optional<std::string> s3("ok3");
    s3 = s1;
    std::cout << "s1:" << *s1 << std::endl;
    std::cout << "s2:" << *s2 << std::endl;
    std::cout << "s3:" << *s3 << std::endl;

    util::Optional<Mystruct> op;
    if (op) {   // op是否被初始化
        std::cout << "op0: " << op->num1 << ", " << op->num2 << std::endl;
    }

    op.Emplace(1, 2);
    std::cout << "op(1, 2): " << op->num1 << ", " << op->num2 << std::endl;

    op.Emplace(3, 4);
    std::cout << "op(3, 4): " << op->num1 << ", " << op->num2 << std::endl;
}

//////////////////////////////////////////////////////////////
// Lazy test
struct BigObject
{
    BigObject()
    {
        std::cout << "BigObject() ..." << std::endl;
    }
};

struct MyLazyObject
{
    MyLazyObject()
    {
        std::cout << "MyLazyObject() ..." << std::endl;
        obj_ = util::lazy([]{ return std::make_shared<BigObject>(); });
    }

    void Load()
    {
        std::cout << "MyLazyObject::Load() ..." << std::endl;
        std::cout << "MyLazyObject::Load(): " << obj_.Value() << std::endl;
    }

    util::Lazy<std::shared_ptr<BigObject>> obj_;
};

int Foo(int x)
{
    return x * 2;
}

void LazyTest()
{
    // 带参数的普通函数
    int y = 4;
    auto lazyer1 = util::lazy(Foo, y);
    std::cout << "lazy(Foo, y): " << lazyer1.Value() << std::endl;

    // 不带参数的lambda
    util::Lazy<int> lazyer2 = util::lazy([] { return 2222222; });
    std::cout << "lazy(lambda): " << lazyer2.Value() << std::endl;

    // 带参数的function
    std::function<int(int)> f = [] (int x) { return x + 1000; };
    auto lazyer3 = util::lazy(f, 3);
    std::cout << "lazy(function(3)): " << lazyer3.Value() << std::endl;

    // 延迟加载大对象
    MyLazyObject t;
    t.Load();
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    OptionalTest();

    std::cout << "****************************" << std::endl;
    LazyTest();

    return 0;
}
