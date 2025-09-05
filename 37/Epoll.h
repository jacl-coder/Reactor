#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <vector>
#include "Channel.h"

class Channel;

class Epoll
{
private:
    static const int MaxEvents = 1024; 
    int epollfd_ = -1;
    epoll_event events_[MaxEvents];
public:
    Epoll();
    ~Epoll();

    // void addfd(int fd, uint32_t op);
    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    std::vector<Channel *> loop(int timeout = -1);
};

