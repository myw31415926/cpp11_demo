/**
 * desc: result_of test
 * file: result_of_test.h
 *
 * author:  myw31415926
 * date:    20190226
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include <iostream>
#include <string>
#include <type_traits>

int fn(int i) { return i; }     // function

typedef int (&fn_ref)(int);     // function reference
typedef int (*fn_ptr)(int);     // function pointer

struct fn_class { int operator()(int i) { return i; } };    // function-like class

void result_of_test()
{
    typedef std::result_of<decltype(fn)&(int)>::type A1;     // int
    typedef std::result_of<decltype(fn)*(int)>::type A2;     // int
    typedef std::result_of<fn_ref(int)>::type B;            // int
    typedef std::result_of<fn_ptr(int)>::type C;            // int
    typedef std::result_of<fn_class(int)>::type D;          // int

    std::cout << std::boolalpha;
    std::cout << "typedefs of int: " << std::endl;

    std::cout << "A1: " << std::is_same<int, A1>::value << std::endl;
    std::cout << "A2: " << std::is_same<int, A2>::value << std::endl;
    std::cout << "B: " << std::is_same<int, B>::value << std::endl;
    std::cout << "C: " << std::is_same<int, C>::value << std::endl;
    std::cout << "D: " << std::is_same<int, D>::value << std::endl;
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
         ToString(T t) { return std::to_string(t); }

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
         ToString(T t) { return t; }
/*
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
         ToString(T& t) { return std::to_string(t); }

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
         ToString(T& t) { return t; }
*/

int main()
{
    result_of_test();

    std::cout << "is_arithmetic<int> = " << std::is_arithmetic<int>::value << std::endl;

    std::cout << ToString(111111111) << std::endl;
    std::cout << ToString(0.111111111) << std::endl;
    std::cout << ToString(std::string("0.111111111")) << std::endl;
    int n = 123;
    std::cout << ToString(n) << std::endl;
    double f = 0.123;
    std::cout << ToString(f) << std::endl;
    std::string s = "0.12345";
    std::cout << ToString(s) << std::endl;

    return 0;
}

