/**
 * desc: IoC容器模板实现
 * file: ioc_container.h
 *
 * author:  myw31415926
 * date:    20190308
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_IOC_CONTAINER_H_
#define UTIL_IOC_CONTAINER_H_

#include "any.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <stdexcept>

namespace util {

// Ioc(Inversion of Control)，控制翻转
// IoC容器 让应用本身不依赖对象的创建和维护，而是交给IoC容器来负责，降低了对象之间直接依赖的耦合性
class IocContainer
{
public:
    IocContainer() = default;
    virtual ~IocContainer() = default;

    // 注册需要创建对象的key和构造器
    // 传入一个唯一标识key，以便创造对象时查找对应的构造函数
    template<class T, typename... Args>  // 单独的类
    void RegisterSimple(const std::string& key)
    {
        // 通过闭包擦除参数类型，即通过lambda来实现
        std::function<T*(Args...)> func = [] (Args... args) {
            return new T(args...);
        };
        RegisterType(key, func);
    }

    // Derived 派生类
    template<class T, typename Derived, typename... Args>
    typename std::enable_if<std::is_base_of<T, Derived>::value>::type
    Register(const std::string& key)
    {
        // 通过闭包擦除参数类型，即通过lambda来实现
        std::function<T*(Args...)> func = [] (Args... args) {
            return new Derived(args...);
        };
        RegisterType(key, func);
    }

    // Depend 依赖类
    template<class T, typename Depend, typename... Args>
    typename std::enable_if<!std::is_base_of<T, Depend>::value>::type
    Register(const std::string& key)
    {
        // 通过闭包擦除参数类型，即通过lambda来实现
        std::function<T*(Args...)> func = [] (Args... args) {
            return new T(new Depend(args...));
        };
        RegisterType(key, func);
    }

    // 根据标识查找对应的构造器，并创建对象指针
    template<class T, typename... Args>
    T* Resolve(const std::string& key, Args... args)
    {
        if (creator_map_.find(key) == creator_map_.end()) {
            return nullptr;
        }

        util::Any resolver = creator_map_[key];
        std::function<T*(Args...)> func = resolver.AnyCast<std::function<T*(Args...)>>();
        return func(args...);
    }

    // 根据标识查找对应的构造器，并创建智能对象指针
    template<class T, typename... Args>
    std::shared_ptr<T> ResolveShared(const std::string& key, Args... args)
    {
        T* ptr = Resolve<T>(key, args...);
        return std::shared_ptr<T>(ptr);
    }

private:
    // 禁止复制和赋值
    IocContainer(const IocContainer&) = delete;
    IocContainer& operator=(const IocContainer&) = delete;

    // 注册类型和构造器
    void RegisterType(const std::string& key, util::Any creator)
    {
        if (creator_map_.find(key) != creator_map_.end()) {
            throw std::invalid_argument("this key has already exist!");
        }
        // 通过Any擦除不同类型的构造器
        creator_map_.emplace(key, creator);
    }

private:
    // 对象构造器map：key -- creator。通过Any擦除不同类型的构造器
    std::unordered_map<std::string, util::Any> creator_map_;
};

} // namespace util

#endif // UTIL_IOC_CONTAINER_H_
