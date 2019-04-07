/**
 * desc: Any元素容器
 * file: any.h
 *
 * author:  myw31415926
 * date:    20190301
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_ANY_H_
#define UTIL_ANY_H_

#include <iostream>
#include <memory>       // std::unique_ptr
#include <typeindex>    // std::type_index
#include <type_traits>

namespace util {

// Any类，只能容纳一个元素的容器，可以擦除类型，给它赋任何值
struct Any
{
    Any(void) : tp_index_(std::type_index(typeid(void))) {}
    Any(const Any& other) : base_ptr_(other.Clone()), tp_index_(other.tp_index_) {}
    Any(Any&& other) : base_ptr_(std::move(other.base_ptr_)), tp_index_(other.tp_index_) {}

    // 创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
    template<typename U, class = typename 
        std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
    Any(U&& value) : 
        base_ptr_(new Derived<typename std::decay<U>::type>(std::forward<U>(value))),
        tp_index_(std::type_index(typeid(typename std::decay<U>::type))) {}

    bool IsNull() const
    {
        return !bool(base_ptr_);
    }

    template<class U>
    bool Is() const
    {
        return tp_index_ == std::type_index(typeid(U));
    }

    // 将Any转换为实际的类型
    template<class U>
    U& AnyCast()
    {
        if (!Is<U>()) {
            std::cout << "can not cast " << typeid(U).name() << " to " << tp_index_.name() << std::endl;
            throw std::bad_cast();
        }

        auto derived = dynamic_cast<Derived<U>*>(base_ptr_.get());
        return derived->value_;
    }

    Any& operator=(const Any& other)
    {
        if (base_ptr_ == other.base_ptr_) {
            return *this;
        }

        base_ptr_ = other.Clone();
        tp_index_ = other.tp_index_;
        return *this;
    }

private:
    // 内部维护一个基类指针，通过基类指针擦除具体类型，通过多态在运行时获取类型
    struct Base
    {
        virtual ~Base() {}
        virtual std::unique_ptr<Base> Clone() const = 0;
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U>
        Derived(U&& value) : value_(std::forward<U>(value)) {}

        std::unique_ptr<Base> Clone() const
        {
            return std::unique_ptr<Base>(new Derived(value_));
        }

        T value_;
    };

    std::unique_ptr<Base> Clone() const
    {
        if (base_ptr_ != nullptr) {
            return base_ptr_->Clone();
        }

        return nullptr;
    }

private:
    std::unique_ptr<Base> base_ptr_;
    // type_index是type_info对象的一个封装类
    // 可以用作关联容器（比如map）和无序关联容器（比如unordered_map）的索引
    std::type_index tp_index_;
}; // class Any

} // namespace util

#endif // UTIL_ANY_H_
