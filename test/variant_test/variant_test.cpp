/**
 * desc: chrono time 测试
 * file: time_test.cpp
 *
 * author:  myw31415926
 * date:    20190306
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "variant.h"

#include <iostream>
#include <string>

void VariantTest()
{
    using Var = util::Variant<int, double, std::string, int>;

    // 根据index获取索引类型
    std::cout << "typeid(Var::IndexType<0>).name()=" << typeid(Var::IndexType<0>).name() << std::endl;
    std::cout << "typeid(Var::IndexType<1>).name()=" << typeid(Var::IndexType<1>).name() << std::endl;
    std::cout << "typeid(Var::IndexType<2>).name()=" << typeid(Var::IndexType<2>).name() << std::endl;
    std::cout << "typeid(Var::IndexType<3>).name()=" << typeid(Var::IndexType<3>).name() << std::endl;
    //std::cout << "typeid(Var::IndexType<4>).name()=" << typeid(Var::IndexType<4>).name() << std::endl;

    // 根据类型获取索引
    Var v;
    std::cout << "v.GetIndexOf<int>()=" << v.GetIndexOf<int>() << std::endl;
    std::cout << "v.GetIndexOf<double>()=" << v.GetIndexOf<double>() << std::endl;
    std::cout << "v.GetIndexOf<std::string>()=" << v.GetIndexOf<std::string>() << std::endl;
    std::cout << "v.GetIndexOf<int>()=" << v.GetIndexOf<int>() << std::endl;
    std::cout << "v.GetIndexOf<char>()=" << v.GetIndexOf<char>() << std::endl;

    std::cout << "v.Empty()=" << (v.Empty() ? "true" : "false") << std::endl;
    v = 10;
    std::cout << "v.Empty()=" << (v.Empty() ? "true" : "false") << std::endl;
    if (!v.Empty()) {
        std::cout << "v.Type().name()=" << v.Type().name() << std::endl;
    }

    try {
        std::cout << "v.Get<int>()=" << v.Get<int>() << std::endl;
        std::cout << "v.Get<double>()=" << v.Get<double>() << std::endl;
    } catch (const std::bad_cast& e) {
        std::cerr << "bad_cast: " << e.what() << std::endl;
    }
}

//////////////////////////////////////////////////////////////
// linux需要"c++filt -t "进行过滤： ./variant_test 2>&1 | c++filt -t
int main(int argc, char const *argv[])
{
    std::cout << "\n*** VariantTest ***" << std::endl;
    VariantTest();

    return 0;
}
