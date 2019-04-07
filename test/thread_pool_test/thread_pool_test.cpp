/**
 * desc: 线程池 ThreadPool 测试
 * file: thread_pool_test.cpp
 *
 * author:  myw31415926
 * date:    20190308
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "thread_pool.h"

#include <iostream>
#include <ctime>
#include <string>

void ThreadPoolTest1()
{
    util::ThreadPool pool(2);

    std::thread thd1([&pool] {
        for (int i = 0; i < 10; i++) {
            auto thd_id = std::this_thread::get_id();
            pool.AddTask([thd_id] {
                std::cout << "synchronous thread1 ID: " << thd_id << std::endl;
            });
        }
    });

    std::thread thd2([&pool] {
        for (int i = 0; i < 10; i++) {
            auto thd_id = std::this_thread::get_id();
            pool.AddTask([thd_id] {
                std::cout << "synchronous thread2 ID: " << thd_id << std::endl;
            });
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::getchar();
    pool.Stop();
    thd1.join();
    thd2.join();
}

///////////////////////////////////////////////////////////////////////
void TestFunc()
{
    char buf[64];

    // 获取当前时间，t的类型 std::time_t
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %X", std::localtime(&t))) {
        std::cout << "now: " << buf     // 2019-03-06 10:03:03
                  << ", synchronous thread ID: " << std::this_thread::get_id()
                  << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ThreadPoolTest2()
{
    util::ThreadPool pool(2);

    std::thread thd([&pool] {
        for (int i = 0; i < 20; i++) {
            pool.AddTask(TestFunc);
        }
    });

    //std::this_thread::sleep_for(std::chrono::seconds(3));
    //std::getchar();
    do {
        std::cout << "thread pool task count " << pool.TaskCount() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (pool.TaskCount() != 0);

    std::cout << "has no task and stop thread pool" << std::endl;
    pool.Stop();    // Stop when has no task; 
    thd.join();
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "*** ThreadPoolTest1 ***" << std::endl;
    ThreadPoolTest1();

    std::cout << "*** ThreadPoolTest2 ***" << std::endl;
    ThreadPoolTest2();

    return 0;
}
