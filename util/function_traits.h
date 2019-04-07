/**
 * desc: 将可调用对象转换为std::function和函数指针
 * file: function_traits.h
 *
 * author:  myw31415926
 * date:    201903011
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_FUNCTION_TRAITS_H_
#define UTIL_FUNCTION_TRAITS_H_

#include <functional>
#include <tuple>

namespace util {

// 将普通函数，函数指针，function/lambda，成员函数，函数对象
// 转换为std::function和函数指针.

// 前置声明
template<typename T>
struct function_traits;

// 普通函数
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
public:
    enum { arity = sizeof...(Args) };   // 参数数量

    using FunctionType = std::function<Ret(Args...)>;
    using FunctionPointer = Ret (*)(Args...);

    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        // 获取指定参数的类型
        using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
    };
};

// 模板特化，函数指针
template<typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)> {};

// 模板特化，std::function
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

// 模板特化，可调用对象
template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())> {};

// 模板特化，member function
#define FUNCTION_TRAITS(...) \
    template<typename Ret, typename Class, typename... Args>    \
    struct function_traits<Ret(Class::*)(Args...) __VA_ARGS__> :\
        function_traits<Ret(Args...)> {};                       \

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

// 封装成C接口调用
template<typename Func>
typename function_traits<Func>::FunctionType ToFucntion(const Func& lambda)
{
    return static_cast<typename function_traits<Func>::FunctionType>(lambda);
}

template<typename Func>
typename function_traits<Func>::FunctionType ToFucntion(Func&& lambda)
{
    return static_cast<typename function_traits<Func>::FunctionType>(std::forward<Func>(lambda));
}

template<typename Func>
typename function_traits<Func>::FunctionPointer ToFucntionPointer(const Func& lambda)
{
    return static_cast<typename function_traits<Func>::FunctionPointer>(lambda);
}

} // namespace util

#endif // UTIL_FUNCTION_TRAITS_H_
