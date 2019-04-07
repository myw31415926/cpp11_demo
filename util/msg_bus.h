/**
 * desc: msg bus 消息总线模板
 * file: msg_bus.h
 *
 * author:  myw31415926
 * date:    201903011
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_MSG_BUS_H_
#define UTIL_MSG_BUS_H_

#include "function_traits.h"
#include "any.h"

#include <functional>
#include <string>
#include <map>

namespace util {

class MsgBus
{
    using Iterator = std::multimap<std::string, util::Any>::iterator;
public:
    MsgBus() = default;
    virtual ~MsgBus() = default;

    // 注册消息
    template<typename F>
    void Register(F&& f, const std::string& topic = "")
    {
        auto func = util::ToFucntion(std::forward<F>(f));
        Add(topic, std::move(func));
    }

    // 发送消息
    template<typename R>
    void Send(const std::string& topic = "")
    {
        using FunctionType = std::function<R()>;

        // topic + 函数名
        std::string msg_type = topic + typeid(FunctionType).name();
        auto range = msg_map_.equal_range(msg_type);
        for (Iterator it = range.first; it != range.second; ++it) {
            auto func = it->second.AnyCast<FunctionType>();
            func();
        }
    }

    template<typename R, typename... Args>
    void Send(Args&&... args, const std::string& topic = "")
    {
        using FunctionType = std::function<R(Args...)>;

        // topic + 函数名
        std::string msg_type = topic + typeid(FunctionType).name();
        auto range = msg_map_.equal_range(msg_type);
        for (Iterator it = range.first; it != range.second; ++it) {
            auto func = it->second.AnyCast<FunctionType>();
            func(std::forward<Args>(args)...);
        }
    }

    // 移除消息，需要主题和消息类型
    template<typename R, typename... Args>
    void Remove(const std::string& topic = "")
    {
        using FunctionType = std::function<R(Args...)>;

        std::string msg_type = topic + typeid(FunctionType).name();
        if (msg_map_.count(msg_type) > 0) {
            auto range = msg_map_.equal_range(msg_type);
            msg_map_.erase(range.first, range.second);
        }
    }

private:
    // 禁止复制和赋值
    MsgBus(const MsgBus&) = delete;
    MsgBus& operator=(const MsgBus&) = delete;

    // 将消息加入map中
    template<typename F>
    void Add(const std::string& topic, F&& f)
    {
        std::string msg_type = topic + typeid(F).name();
        msg_map_.emplace(std::move(msg_type), std::forward<F>(f));
    }

private:
    std::multimap<std::string, util::Any> msg_map_;
};

} // namespace util

#endif // UTIL_MSG_BUS_H_
