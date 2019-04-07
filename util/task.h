/**
 * desc: C++11轻量级并行task任务
 * file: task.h
 *
 * author:  myw31415926
 * date:    20190404
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_TASK_H_
#define UTIL_TASK_H_

#include <functional>

namespace util {

template<typename T>
class Task;

template<typename R, typename... Args>
class Task<R(Args...)>
{
public:
    //Task(std::function<const R(Args...)>& func) : func_(func) {}
    Task(const std::function<R(Args...)>& func) : func_(func) {}
    Task(std::function<R(Args...)>&& func) : func_(std::move(func)) {}

    virtual ~Task() {}

    // 等待异步操作完成
    void Wait()
    {
        std::async(func_).wait();
    }

    // 获取异步操作结果
    template<typename... Args>
    R Get(Args&&... args)
    {
        return std::async(func_, std::forward<Args>(args)...).get();
    }

    // 发起异步操作
    std::shared_future<R> Run()
    {
        return std::async(func_);
    }

private:
    std::function<R(Args...)> func_;
};

} // namespace util

#endif // UTIL_TASK_H_
