/**
 * desc: 惰性求值模板
 * file: lazy.h
 *
 * author:  myw31415926
 * date:    20190226
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_LAZY_H_
#define UTIL_LAZY_H_

#include "optional.h"
#include <functional>

namespace util {

template<typename T>
struct Lazy
{
    Lazy() {}

    template<typename Func, typename... Args>
    Lazy(Func&& f, Args&&... args)  // 必须用Func&& f，否则gcc4.8.5编译错误，lazy中找不到匹配的函数
    {
        // lambda表达式保存需要延迟执行的函数，gcc4.8.5不支持
        // func_ = [&f, &args...] { return f(args...); };
        func_ = std::bind(f, std::forward<Args>(args)...);
    }

    // 延迟执行，结构保存到Optional中，下次不用重新计算即可返回结果
    T& Value()
    {
        if (!value_.IsInit()) {
            value_ = func_();
        }
        return *value_;
    }

    bool IsInit() const
    {
        return value_.IsInit();
    }

private:
    std::function<T()> func_;
    Optional<T>        value_;
};

// 辅助函数，返回一个可延迟执行的Lazy对象
template<typename Func, typename... Args>
Lazy<typename std::result_of<Func(Args...)>::type> 
lazy(Func&& fun, Args&&... args)
{
    return Lazy<typename std::result_of<Func(Args...)>::type>(std::forward<Func>(fun), 
        std::forward<Args>(args)...);
}

} // namespace util

#endif // UTIL_LAZY_H_
