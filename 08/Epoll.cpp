#include "Epoll.h"

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

/*
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
*/

void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();

    if(ch->ispoll())
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl() MOD");
            exit(-1);
        }
    }
    else
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl() ADD");
            exit(-1);
        }
        ch->setinepoll();
    }
}

// std::vector<epoll_event> Epoll::loop(int timeout)
// {
//     std::vector<epoll_event> evs;
//     bzero(&events_, sizeof(events_));
//     int nfds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
//     if (nfds == -1)
//     {
//         perror("epoll_wait()");
//         exit(-1);
//     }

//     if (nfds == 0)
//     {
//         // Timeout, no events
//         printf("epoll_wait() timeout\n");
//         return evs;
//     }
    
//     for (int i = 0; i < nfds; ++i)
//     {
//         evs.push_back(events_[i]);
//     }
//     return evs;
// }

std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels;
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
        return channels;
    }
    
    for (int i = 0; i < nfds; ++i)
    {
        Channel *ch = static_cast<Channel *>(events_[i].data.ptr);
        ch->setrevents(events_[i].events);
        channels.push_back(ch);
    }
    return channels;
}