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

#include "util.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>  // std::put_time
#include <string>

///////////////////////////////////////////////////////////////////////
// 时间差
void DurationTest()
{
    std::chrono::system_clock::time_point t1(std::chrono::system_clock::now());
    std::cout << "Hello World!" << std::endl;
    std::chrono::system_clock::time_point t2(std::chrono::system_clock::now());

    // 计算时钟周期
    std::cout << (t2 - t1).count() << " tick count" << std::endl;

    // 转成成其他周期
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
              << " microseconds" << std::endl;
}


///////////////////////////////////////////////////////////////////////
// 时间格式化
void TimeStringTest()
{
    // 获取当前时间，t的类型 std::time_t
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // 自g++5起才有std::put_time，g++4.8不支持
    // std::cout << std::put_time(std::localtime(&t), "%Y-%m-%d %X") << std::endl;
    // std::cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H.%M.%S") << std::endl;
    char buf[64];
    if (std::strftime(buf, sizeof(buf), "%x %X", std::localtime(&t))) {
        std::cout << "now: " << buf << std::endl;   // 03/06/19 10:03:03
    }

    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %X", std::localtime(&t))) {
        std::cout << "now: " << buf << std::endl;   // 2019-03-06 10:03:03
    }

    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H.%M.%S", std::localtime(&t))) {
        std::cout << "now: " << buf << std::endl;   // 2019-03-06 10.03.03
    }
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** DurationTest ***" << std::endl;
    util::TimeSpan ts;
    DurationTest();
    std::cout << ts.Span() << " ms" << std::endl;
    std::cout << ts.SpanMicro() << " us" << std::endl;
    std::cout << ts.SpanNano() << " ns" << std::endl;
    std::cout << ts.SpanSeconds() << " s" << std::endl;
    std::cout << ts.SpanMinutes() << " min" << std::endl;
    std::cout << ts.SpanHours() << " hour" << std::endl;

    std::cout << "\n*** TimeStringTest ***" << std::endl;
    ts.Reset();
    TimeStringTest();
    std::cout << ts.Span() << " ms" << std::endl;
    std::cout << ts.SpanMicro() << " us" << std::endl;
    std::cout << ts.SpanNano() << " ns" << std::endl;
    std::cout << ts.SpanSeconds() << " s" << std::endl;
    std::cout << ts.SpanMinutes() << " min" << std::endl;
    std::cout << ts.SpanHours() << " hour" << std::endl;

    return 0;
}
