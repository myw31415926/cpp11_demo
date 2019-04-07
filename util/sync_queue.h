/**
 * desc: 同步队列模板实现
 * file: sync_queue.h
 *
 * author:  myw31415926
 * date:    20190308
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_SYNC_QUEUE_H_
#define UTIL_SYNC_QUEUE_H_

#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace util {

template<typename T>
class SyncQueue
{
public:
    SyncQueue(int max_size) : max_size_(max_size), stop_flag_(false) {}

    void Push(const T& t) { Add(t); }
    void Push(T&& t) { Add(std::forward<T>(t)); }

    void Pop(std::list<T>& queue)
    {
        // wait前必须先获得 std::unique_lock<std::mutex> 
        std::unique_lock<std::mutex> locker(mtx_);
        not_empty_.wait(locker, [this] { return stop_flag_ || NotEmpty(); });

        if (stop_flag_) {
            return;
        }
        queue = std::move(queue_);
        not_full_.notify_one();
    }

    void Pop(T& t)
    {
        // wait前必须先获得 std::unique_lock<std::mutex> 
        std::unique_lock<std::mutex> locker(mtx_);
        not_empty_.wait(locker, [this] { return stop_flag_ || NotEmpty(); });

        if (stop_flag_) {
            return;
        }
        t = queue_.front();
        queue_.pop_front();
        not_full_.notify_one();
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> locker(mtx_);
            stop_flag_ = true;
        }
        // 通知结束wait
        not_full_.notify_all();
        not_empty_.notify_all();
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return queue_.empty();
    }

    bool Full()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return queue_.size() >= max_size_;
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return queue_.size();
    }

private:
    bool NotFull() const
    {
        bool full = queue_.size() >= max_size_;
        if (full) {
            std::cout << "queue is full, please wait..." << std::endl;
        }
        return !full;
    }

    bool NotEmpty() const
    {
        bool empty = queue_.empty();
        if (empty) {
            std::cout << "queue is empty, please wait... thread id: "
                      << std::this_thread::get_id() << std::endl;
        }
        return !empty;
    }

    template<typename F>
    void Add(F&& x)
    {
        // wait前必须先获得 std::unique_lock<std::mutex> 
        std::unique_lock<std::mutex> locker(mtx_);
        not_full_.wait(locker, [this] { return stop_flag_ || NotFull(); });

        if (stop_flag_) {
            return;
        }
        queue_.push_back(std::forward<F>(x));
        not_empty_.notify_one();
    }

private:
    int  max_size_;         // 同步队列最大size
    bool stop_flag_;        // 停在标志
    std::list<T> queue_;    // 缓冲区
    std::mutex   mtx_;      // 缓冲区互斥锁
    std::condition_variable not_empty_; // 队列不空的条件
    std::condition_variable not_full_;  // 队列不满的条件
};

} // namespace util

#endif // UTIL_SYNC_QUEUE_H_
