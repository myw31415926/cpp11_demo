/**
 * desc: 简单迭代器类的测试
 * file: range.h
 *
 * author:  myw31415926
 * date:    20190218
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "range.h"
#include <iostream>

// 迭代器类的测试
void RangeTest(void)
{
    std::cout << "Range(15): ";
    for (int i : util::Range(15)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    std::cout << "Range(2, 6): ";
    for (auto i : util::Range(2, 6)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    const int x = 2, y = 6, z = 3;
    std::cout << "Range(2, 6, 3): ";
    for (auto i : util::Range(x, y, z)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    std::cout << "Range(-2, -6, -3): ";
    for (auto i : util::Range(-2, -6, -3)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;
    
    std::cout << "Range(10.5, 15.5): ";
    for (auto i : util::Range(10.5, 15.5)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    std::cout << "Range(35, 27, -1): ";
    for (auto i : util::Range(35, 27, -1)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    std::cout << "Range(2, 8, 0.5): ";
    for (auto i : util::Range(2, 8, 0.5)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    std::cout << "Range(8, 7, -0.1): ";
    for (auto i : util::Range(8, 7, -0.1)) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    std::cout << "Range('a', 'z'): ";
    for (auto i : util::Range('a', 'z')) {
        std::cout << " " << i;
    }
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    RangeTest();
    return 0;
}
