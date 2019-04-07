/**
 * desc: 基础操作
 * file: util.h
 *
 * author:  myw31415926
 * date:    20190305
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#include <iostream>
#include <memory>
#include <chrono>
#include <type_traits>

namespace util {

// 封装std::shared_ptr，支持数组
// std::shared_ptr默认删除器不支持数组对象，无法管理数组
template<typename T>
inline std::shared_ptr<T> make_shared_array(size_t size)
{
    return std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
}


// 不同版本编译器的定义
// C++03：__cplusplus = 199711L
// C++11：__cplusplus = 201103L
// C++14：__cplusplus = 201402L
// 使用C++14以下的编译器
#if __cplusplus < 201402L
// C++11中unique_ptr没有提供make_unique的方法，C++14中有
// 普通指针
template<typename T, typename... Args>
inline typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type   // 判断非数组的情况
make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 动态数组，T维度为0（不固定）
template<typename T, typename... Args>
inline typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, 
    std::unique_ptr<T>>::type
make_unique(size_t size)
{
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[size]());
}

// 不支持定长数组
template<typename T, typename... Args>
typename std::enable_if<std::extent<T>::value != 0, void>::type 
make_unique(size_t size) = delete;
#endif

// 计时器，统计执行的时间间隔
// std::chrono::system_clock 系统范围的时钟，时间调整后会跳变
// std::chrono::steady_clock 不能被调整的时钟，开机后CPU时间，不会跳变
// std::chrono::high_resolution_clock 拥有最小计次周期的时钟
class TimeSpan
{
public:
    TimeSpan() : begin_(std::chrono::high_resolution_clock::now()) {}

    void Reset()
    {
        begin_ = std::chrono::high_resolution_clock::now();
    }

    // 计算时间差，默认输出毫秒
    template<typename Duration = std::chrono::milliseconds>
    int64_t Span() const
    {
        return std::chrono::duration_cast<Duration>(
            std::chrono::high_resolution_clock::now() - begin_).count();
    }

    // 微妙
    int64_t SpanMicro() const
    {
        return Span<std::chrono::microseconds>();
    }

    // 纳秒
    int64_t SpanNano() const
    {
        return Span<std::chrono::nanoseconds>();
    }

    // 秒
    int64_t SpanSeconds() const
    {
        return Span<std::chrono::seconds>();
    }

    // 分
    int64_t SpanMinutes() const
    {
        return Span<std::chrono::minutes>();
    }

    // 时
    int64_t SpanHours() const
    {
        return Span<std::chrono::hours>();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
};

} // namespace util

#endif // UTIL_UTIL_H_