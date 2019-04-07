/**
 * desc: msg bus 消息总线测试
 * file: msg_bus_test.cpp
 *
 * author:  myw31415926
 * date:    201903011
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include "msg_bus.h"

#include <iostream>

//////////////////////////////////////////////////////////////
// function_traits 测试
void FunctionTraitsTest()
{
    auto f1 = util::ToFucntion([](int i) { return i; });
    std::function<int(int)> f2 = [](int i) { return i; };
    if (std::is_same<decltype(f1), decltype(f2)>::value) {
        std::cout << "f1 and f2 is same : " << typeid(f1).name() << std::endl;;
    }
}

//////////////////////////////////////////////////////////////
// msg_bus 测试
void MsgBusTest()
{
    util::MsgBus bus;

    // 注册消息
    bus.Register([](int i) {std::cout << "no reference, i = " << i << std::endl; });
    bus.Register([](int& i) {std::cout << "lvalue reference, i = " << i << std::endl; });
    bus.Register([](int&& i) {std::cout << "rvalue reference, i = " << i << std::endl; });
    bus.Register([](const int& i) {std::cout << "const lvalue reference, i = " << i << std::endl; });
    bus.Register([](int i) {
        std::cout << "const lvalue reference with topic, i = " << i << std::endl;
        return i;
    }, "i");    // 带topic

    // 发送消息
    int i = 100;
    bus.Send<void, int>(200);
    bus.Send<void, int&>(i);
    bus.Send<void, const int&>(400);
    bus.Send<void, int&&>(500);
    bus.Send<int, int>(300, "a");
    bus.Send<int, int>(300, "i");

    // 移除消息
    bus.Remove<void, int>();
    bus.Remove<void, int&>();
    bus.Remove<void, const int&>();
    bus.Remove<void, int&&>();
    bus.Remove<int, int>("i");

    // 发送消息
    std::cout << "bus send again after remove" << std::endl;
    bus.Send<void, int>(200);
    bus.Send<void, int&>(i);
    bus.Send<void, const int&>(400);
    bus.Send<void, int&&>(500);
    bus.Send<int, int>(300, "i");
}

//////////////////////////////////////////////////////////////
// 多对象 msg_bus 测试，且向调用者回馈消息
util::MsgBus g_bus;
const std::string g_topic = "Drive";
const std::string g_topic_ok = "DriveOK";

// 发送消息对象
class Subject
{
public:
    Subject()
    {
        g_bus.Register([this] { DriveOK(); }, g_topic_ok);
    }

    void SendReq(const std::string& topic)
    {
        g_bus.Send<void, int>(50, topic);
    }

    void DriveOK()
    {
        std::cout << "Subject drive ok" << std::endl;
    }
};

// 接收消息对象
class Car
{
public:
    Car()
    {
        g_bus.Register([this](int speed) { Drive(speed); }, g_topic);
    }

    void Drive(const int speed)
    {
        std::cout << "Car drive is " << speed << std::endl;
        g_bus.Send<void>(g_topic_ok);
    }
};

class Bus
{
public:
    Bus()
    {
        g_bus.Register([this](int speed) { Drive(speed); }, g_topic);
    }

    void Drive(const int speed)
    {
        std::cout << "Bus drive is " << speed << std::endl;
        g_bus.Send<void>(g_topic_ok);
    }
};

class Truck
{
public:
    Truck()
    {
        g_bus.Register([this](int speed) { Drive(speed); });
    }

    void Drive(const int speed)
    {
        std::cout << "Truck drive is " << speed << std::endl;
        g_bus.Send<void>(g_topic_ok);
    }
};

void SubjectMsgBusTest()
{
    Subject sub;
    Car cat;
    Bus bus;
    Truck truck;

    sub.SendReq(g_topic);
    sub.SendReq("");

    std::cout << "msgbus remove<void, int>()" << std::endl; 

    g_bus.Remove<void, int>();
    sub.SendReq(g_topic);
    sub.SendReq("");
}

//////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    std::cout << "\n*** FunctionTraitsTest ***" << std::endl;
    FunctionTraitsTest();

    std::cout << "\n*** MsgBusTest ***" << std::endl;
    MsgBusTest();

    std::cout << "\n*** SubjectMsgBusTest ***" << std::endl;
    SubjectMsgBusTest();

    return 0;
}
