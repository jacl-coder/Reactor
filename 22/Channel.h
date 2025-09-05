#pragma once
#include <sys/epoll.h>
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"

class EventLoop;

class Channel
{
private:
    int fd_ = -1;
    EventLoop *loop_ = nullptr;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    std::function<void()> readcallback_;
    std::function<void()> closecallback_;
    std::function<void()> errorcallback_;
    std::function<void()> writecallback_;

public:
    Channel(EventLoop *ep, int fd);
    ~Channel();

    int fd();
    void useet();
    void enablereading();
    void disablereading();
    void enablewriting();
    void disablewriting();
    void setinepoll();
    void setrevents(uint32_t ev);
    bool ispoll();
    uint32_t events();
    uint32_t revents();

    void handleevent();

    // void onmessage();

    void setreadcallback(std::function<void()> fn);
    void setclosecallback(std::function<void()> fn);
    void seterrorcallback(std::function<void()> fn);
    void setwritecallback(std::function<void()> fn);
};
