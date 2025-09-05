#include "Channel.h"

/*
class Channel
{
private:
    int fd_ = -1;
    Epoll *ep_ = nullptr;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
public:
    Channel(Epoll *ep, int fd);
    ~Channel();

    int fd();
    void useet();
    void enableReading();
    void setinepoll();
    void setrevents(uint32_t revt);
    bool ispoll();
    uint32_t events();
    uint32_t revents();
};
*/

Channel::Channel(Epoll *ep, int fd)
    : ep_(ep), fd_(fd)
{
}

Channel::~Channel()
{
}

int Channel::fd()
{
    return fd_;
}

void Channel::useet()
{
    events_ |= EPOLLET;
}

void Channel::enablereading()
{
    events_ |= EPOLLIN;
    ep_->updatechannel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

bool Channel::ispoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_;
}