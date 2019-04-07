/**
 * desc: 对象池模板
 * file: object_pool.h
 *
 * author:  myw31415926
 * date:    20190306
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_OBJECT_POOL_H_
#define UTIL_OBJECT_POOL_H_

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <iostream>

namespace util {

// 对象池最大容量
const int g_object_max = 10;

template<typename T>
class ObjectPool
{
    // 定义带参数的函数，返回对象的智能指针
    template<typename... Args>
    using Constructor = std::function<std::shared_ptr<T>(Args...)>;
public:
    ObjectPool() : clear_flag_(false) {};

    virtual ~ObjectPool()
    {
        clear_flag_ = true;
        object_map_.clear();
    };

    // 初始化。num 默认创建多少个对象
    template<typename... Args>
    void Init(size_t num, Args&&... args)
    {
        if (num <= 0 || num > g_object_max) {
            throw std::logic_error("object num out of range");
        }

        std::string consturct_name = typeid(Constructor<Args...>).name();  // 不区分应用
        std::cout << "Init consturct_name = " << consturct_name << std::endl;
        for (size_t i = 0; i < num; i++) {
            object_map_.emplace(consturct_name, std::shared_ptr<T>(new T(std::forward<Args>(args)...), 
                [this, consturct_name](T* p) {
                    // 删除器中不直接删除对象，而是回收到对象池中，以供下次使用
                    if (!clear_flag_) {     // 析构时不需要将对象回收
                        std::cout << "reset object pool" << std::endl;
                        object_map_.emplace(std::move(consturct_name), std::shared_ptr<T>(p));
                    }
                }));
        }
    }

    // 从对象池中获取一个元素
    template<typename... Args>
    std::shared_ptr<T> Get()
    {
        std::string consturct_name = typeid(Constructor<Args...>).name();
        std::cout << "Get consturct_name = " << consturct_name << std::endl;

        auto range = object_map_.equal_range(consturct_name);
        for (auto it = range.first; it != range.second; ++it) {
            auto ptr = it->second;
            object_map_.erase(it);  // 获取到后从pool中删除
            return ptr;
        }
        return nullptr;
    }

private:
    // 禁止复制和赋值
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

private:
    bool clear_flag_;
    // 多索引容器，存储对象
    std::multimap<std::string, std::shared_ptr<T>> object_map_;
};

} // namespace util

#endif // UTIL_OBJECT_POOL_H_
