#pragma once
#include <functional>
#include <sys/syscall.h>
#include <unistd.h>
#include "Epoll.h"
#include <memory>

class Epoll;
class Channel;

class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop *)> epolltimeoutcallback_;
public:
    EventLoop();
    ~EventLoop();

    void run();

    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop *)> fn);
};