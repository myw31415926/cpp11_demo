/**
 * desc: 可变参数模板
 * file: optional.h
 *
 * author:  myw31415926
 * date:    20190226
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_OPTIONAL_H_
#define UTIL_OPTIONAL_H_

#include <type_traits>
#include <stdexcept>

namespace util {

template<typename T>
class Optional
{
    // 定义内存对齐的缓冲区类型
    using data_t = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

public:
    Optional() : has_init_(false) {}

    Optional(const T& v) : has_init_(false)
    {
        Create(v);  // 创建对象
    }

    // 右值构造函数
    Optional(T&& v) : has_init_(false)
    {
        Create(std::move(v));
    }

    Optional(const Optional& other) : has_init_(false)
    {
        if (other.IsInit()) {
            Assign(other);
        }
    }

    // 右值拷贝构造函数
    Optional(Optional&& other) : has_init_(false)
    {
        if (other.IsInit()) {
            Assign(std::move(other));
            other.Destroy();
        }
    }

    Optional& operator=(const Optional& other)
    {
        Assign(other);
        return *this;
    }

    Optional& operator=(Optional&& other)
    {
        Assign(std::move(other));
        return *this;
    }

    virtual ~Optional()
    {
        Destroy();
    }

    // 根据参数创建对象
    template<typename... Args>
    void Emplace(Args&&... args)
    {
        Destroy();
        Create(std::forward<Args>(args)...);
    }

    // 是否已经初始化
    bool IsInit() const
    {
        return has_init_;
    }

    // if语句中判断是否已经初始化
    explicit operator bool() const
    {
        return IsInit();
    }

    T& operator*()
    {
        if (IsInit()) {
            return *((T*)(&data_));
        }

        throw std::logic_error{"try to get data in a Optional which is not initialized"};
    }

    const T& operator*() const
    {
        if (IsInit()) {
            return *((T*)(&data_));
        }

        throw std::logic_error{"try to get data in a Optional which is not initialized"};
    }

    T* operator->()
    {
        if (IsInit()) {
            return ((T*)(&data_));
        }

        throw std::logic_error{"try to get data in a Optional which is not initialized"};
    }

    const T* operator->() const
    {
        if (IsInit()) {
            return ((T*)(&data_));
        }

        throw std::logic_error{"try to get data in a Optional which is not initialized"};
    }

    bool operator==(const Optional<T>& rhs) const
    {
        // 若有一方未初始化，返回false
        // 都未初始化，返回true
        // 都初始化，则比较数据 data_
        return (!bool(*this)) != (!rhs) ? false : (!bool(*this) ? true : (*(*this)) == (*rhs));
    }

    bool operator!=(const Optional<T>& rhs) const
    {
        return !(*this == (rhs));
    }

    bool operator<(const Optional<T>& rhs) const
    {
        // 右值未初始化，返回false
        // 左值未初始化，右值已初始化，返回true
        // 都已初始化，则比较数据 data_
        return !rhs ? false : (!bool(*this) ? true : (*(*this) < (*rhs)));
    }

private:
    template<typename... Args>
    void Create(Args&&... args)
    {
        // 通过 placement new 创建对象
        new (&data_) T(std::forward<Args>(args)...);    // 使用转发forword消除拷贝
        has_init_ = true;
    }

    void Destroy()
    {
        if (has_init_) {
            has_init_ = false;
            ((T*)(&data_))->~T();   // 使用 placement new 创建对象，需要手动调用析构
        }
    }

    void Assign(const Optional& other)
    {
        if (other.IsInit()) {
            Copy(other.data_);
            has_init_ = true;
        } else {
            Destroy();
        }
    }

    void Assign(Optional&& other)
    {
        if (other.IsInit()) {
            Move(std::move(other.data_));
            has_init_ = true;
            other.Destroy();
        } else {
            Destroy();
        }
    }

    void Copy(const data_t& val)
    {
        Destroy();
        new (&data_) T(*((T*)(&val)));
    }

    void Move(data_t&& val)
    {
        Destroy();
        new (&data_) T(std::move(*((T*)(&val))));
    }

private:
    bool   has_init_;
    data_t data_;
};

} // namespace util

#endif // UTIL_OPTIONAL_H_
