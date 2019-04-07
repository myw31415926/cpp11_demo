/**
 * desc: 设计模式 单例模板
 * file: singleton.h
 *
 * author:  myw31415926
 * date:    20190306
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_SINGLETON_H_
#define UTIL_SINGLETON_H_

#include <stdexcept>

namespace util {

template<typename T>
class Singleton
{
public:
    // 初始化单例对象
    template<typename... Args>
    static T* Instance(Args&&... args)
    {
        if (instance_ == nullptr) {
            instance_ = new T(std::forward<Args>(args)...);
        }
        return instance_;
    }

    // 获取单例
    static T* GetInstance()
    {
        if (instance_ == nullptr) {
            throw std::logic_error("the Instance is not init, please initialize the instance first");
        }
        return instance_;
    }

    // 销毁单例
    static void Destroy()
    {
        if (instance_ != nullptr) {
            delete instance_;
            instance_ = nullptr;
        }
    }

private:
    Singleton() = default;
    virtual ~Singleton() = default;

    // 禁止复制和赋值
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    static T* instance_;
};

template<typename T>
T* Singleton<T>::instance_ = nullptr;

} // namespace util

#endif // UTIL_SINGLETON_H_