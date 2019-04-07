/**
 * desc: Any元素容器测试
 * file: any_test.cpp
 *
 * author:  myw31415926
 * date:    20190301
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "any.h"

#include <iostream>
#include <string>

// 若使用 gcc 或相似者则须通过 c++filt -t 过滤。
// ./any_test | c++filt -t
// 否则输出的是经过修饰的符号：
//
//  它的输出格式为 [指针][名称空间][类别][模板]
// 
// [指针]：若是指针则输出P。
// 
// [名称空间]：若是std则输出St，若是自定义的名称空间则输出字符数及它的名字，
//             并在开头加N，在结尾加E。
// 
// [类别]：若是自定义的名称空间则输出字符数及它的名字，若内建类型输出如下：
// 　　bool: b
// 　　char: c
// 　　signed char: a
// 　　unsigned char: h
// 　　(signed) short (int): s
// 　　unsigned short (int): t
// 　　(signed) (int): i
// 　　unsigned (int): j
// 　　(signed) long (int): l
// 　　unsigned long (int): m
// 　　(signed) long long (int): x
// 　　unsigned long long (int): y
// 　　float: f
// 　　double: d
// 　　long double: e
// 
// [模板] 类型模板以I开头，以E结尾；常数模板以L开头，以E结尾。
//        只有整型变量(int、char之类的)才能做为常数模板，浮点数不行。

void TestAny()
{
    int myint = 50;
    std::string mystr = "string";
    double *mydoubleptr = nullptr;
 
    std::cout << "myint has type: " << typeid(myint).name() << '\n'
              << "mystr has type: " << typeid(mystr).name() << '\n'
              << "mydoubleptr has type: " << typeid(mydoubleptr).name() << '\n';

    util::Any n;
    if (n.IsNull()) {   // true
        std::cout << "n is null" << std::endl;
    } else {
        std::cout << "n is not null" << std::endl;
    }

    std::string s1 = "hello";
    n = s1;
    try {
        n.AnyCast<int>();   // cast error and throw std::bad_cast
    } catch (const std::bad_cast& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    

    util::Any n1 = 1;
    if (n1.Is<int>()) { // true
        std::cout << "n1.AnyCast<int>() = " << n1.AnyCast<int>() << std::endl;
    } else {
        std::cerr << "n1.AnyCast<int>() false" << std::endl;
    }
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    TestAny();
    return 0;
}
