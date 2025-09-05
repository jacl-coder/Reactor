#pragma once
#include "Epoll.h"

class Epoll;
class Channel;

class EventLoop
{
private:
    Epoll *ep_;
public:
    EventLoop();
    ~EventLoop();

    void run();
    void updatechannel(Channel *channel);
};