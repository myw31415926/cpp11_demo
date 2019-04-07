/**
 * desc: thread和mutex测试
 * file: thread_test.cpp
 *
 * author:  myw31415926
 * date:    20190305
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <future>   // for std::future, std::promise, std::async

///////////////////////////////////////////////////////////////////////
// 带超时的互斥量
std::timed_mutex timed_mtx;

void TimedMutexFunc()
{
    std::chrono::milliseconds timeout(100);

    while (true) {
        if (timed_mtx.try_lock_for(timeout)) {
            std::cout << std::this_thread::get_id()
                      << ": do work with the mutex" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            timed_mtx.unlock();

            std::cout << "timed_mtx.unlock()" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            break;
        } else {
            std::cout << std::this_thread::get_id()
                      << ": do work without mutex" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void TimedMutexTest()
{
    std::thread t1(TimedMutexFunc);
    std::thread t2(TimedMutexFunc);

    t1.join();
    t2.join();
}

///////////////////////////////////////////////////////////////////////
// 异步操作函数
void AsyncTest()
{
    std::future<int> f1 = std::async(std::launch::async, []() {
        std::cout << "async f1 ..." << std::endl;
        return 8;
    });

    std::cout << "*** f1.get start ***" << std::endl;
    // get 等待异步操作结束，并返回结果
    std::cout << "f1.get() = " << f1.get() << std::endl;    // output 8
    std::cout << "*** f1.get end ***" << std::endl;

    std::future<int> f2 = std::async(std::launch::async, []() {
        std::cout << "async f2 ..." << std::endl;
        return 9;
    });

    std::cout << "*** f2.wait start ***" << std::endl;
    // wait 只等待异步操作完成，无返回值
    f2.wait();
    std::cout << "*** f2.wait end ***" << std::endl;

    std::future<int> f3 = std::async(std::launch::async, []() {
        std::cout << "async f3 ..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 10;
    });

    std::cout << "*** f3.wait_for start ... ***" << std::endl;
    std::future_status status;
    do {
        // wait_for 超时等待，返回异步操作的结果
        status = f3.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred" << std::endl;
        } else if (status == std::future_status::timeout) {
            std::cout << "timeout" << std::endl;
        } else if (status == std::future_status::ready) {
            std::cout << "ready" << std::endl;
        }
    } while (status != std::future_status::ready);

    std::cout << "f3.get() = " << f3.get() << std::endl;
    std::cout << "*** f3.wait_for end ***" << std::endl;
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** TimedMutexTest ***" << std::endl;
    TimedMutexTest();

    std::cout << "\n*** AsyncTest ***" << std::endl;
    AsyncTest();

    return 0;
}
