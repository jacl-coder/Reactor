#include "Epoll.h"

/*
class Epoll
{
private:
    static const int MaxEvents = 1024; 
    int epollfd_ = -1;
    epoll_event events_[MaxEvents];
public:
    Epoll();
    ~Epoll();

    void addfd(int fd, uint32_t op);
    std::vector<epoll_event> loop(int timeout = -1);
};
*/

Epoll::Epoll()
{
    epollfd_ = epoll_create(1);
    if (epollfd_ == -1)
    {
        perror("epoll_create()");
        exit(-1);
    }
}

Epoll::~Epoll()
{
    if (epollfd_ != -1)
    {
        close(epollfd_);
    }
}

void Epoll::addfd(int fd, uint32_t op)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = op; // EPOLLIN | EPOLLET
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll_ctl()");
        exit(-1);
    }
}

std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;
    bzero(&events_, sizeof(events_));
    int nfds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
    if (nfds == -1)
    {
        perror("epoll_wait()");
        exit(-1);
    }

    if (nfds == 0)
    {
        // Timeout, no events
        printf("epoll_wait() timeout\n");
        return evs;
    }
    
    for (int i = 0; i < nfds; ++i)
    {
        evs.push_back(events_[i]);
    }
    return evs;
}