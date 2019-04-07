/**
 * desc: 线程池模板实现
 * file: thread_pool.h
 *
 * author:  myw31415926
 * date:    20190308
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_THREAD_POOL_H_
#define UTIL_THREAD_POOL_H_

#include "sync_queue.h"

#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

namespace util {

class ThreadPool
{
public:
    using Task = std::function<void()>;

    ThreadPool(int thread_num = std::thread::hardware_concurrency())
        : queue_(max_task_num_)
    {
        Start(thread_num);
    }

    virtual ~ThreadPool(void)
    {
        // 停止线程池
        Stop();
    }

    // 停止线程池
    void Stop()
    {
        // 保证多线程情况下只调用一次
        std::call_once(onceflag_, [this] { StopThreadGroup(); });
    }

    void AddTask(const Task& t) { queue_.Push(t); }
    void AddTask(Task&& t) { queue_.Push(std::forward<Task>(t)); }

    size_t TaskCount() { return queue_.Size() + tasking_num_.load(); }

private:
    // 开始线程池
    void Start(int thread_num)
    {
        running_ = true;
        tasking_num_ = 0;

        // 创建线程组
        for (int i = 0; i < thread_num; i++) {
            threadgroup_.push_back(
                std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
        }
    }

    // 执行同步队列中的任务
    void RunInThread()
    {
        while (running_) {
            // 取任务执行
            /* std::list<Task> tasks;   // 批量取出任务
            / queue_.Pop(tasks);

            tasking_num_ = tasks.size();
            for (auto& task : tasks) {
                if (!running_) {
                    return;
                }
                task();
                tasking_num_--;
            }*/

            Task t;
            queue_.Pop(t);

            if (!running_) {
                return;
            }

            t();
        }
    }

    // 停止线程组
    void StopThreadGroup()
    {
        queue_.Stop();      // 停止同步队列中的线程
        running_ = false;   // 置为false，让内部线程跳出循环并退出

        for (auto thd : threadgroup_) {
            if (thd) {
                thd->join();
            }
        }
        threadgroup_.clear();
    }

private:
    static int max_task_num_;   // 最大线程数

    std::once_flag        onceflag_;
    std::atomic_bool      running_;     // 运行标志位
    std::atomic_size_t    tasking_num_; // 运行标志位
    util::SyncQueue<Task> queue_;       // 同步队列
    std::list<std::shared_ptr<std::thread>> threadgroup_;   // 处理任务的线程组
};

int ThreadPool::max_task_num_ = 100;    // 默认最大线程数100

} // namespace util

#endif // UTIL_THREAD_POOL_H_
