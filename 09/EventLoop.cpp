#include "EventLoop.h"

/*
class EventLoop
{
private:
    Epoll ep_;
public:
    EventLoop();
    ~EventLoop();

    void run();
};
*/

EventLoop::EventLoop(): ep_(new Epoll)
{
}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
        while (true)
    {
        std::vector<Channel *> channels = ep_->loop();
        for (auto &ch : channels)
        {
            ch->handleevent();
        }
    }
}

Epoll *EventLoop::ep()
{
    return ep_;
}