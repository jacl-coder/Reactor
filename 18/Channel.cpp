#include "Channel.h"

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd)
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
    loop_->updatechannel(this);
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

void Channel::handleevent()
{
    // Handle data from clients
    if (revents_ & EPOLLRDHUP)
    {
        // Client disconnected
        // printf("Client fd %d disconnected\n", fd_);
        // close(fd_);
        closecallback_();
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        readcallback_();
    }
    else if (revents_ & EPOLLOUT)
    {
        // Ready to write (not used in this example)
    }
    else
    {
        // printf("Unexpected event for fd %d\n", fd_);
        // close(fd_);
        errorcallback_();
    }
}

void Channel::onmessage()
{
    char buffer[4096];
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));
        if (nread > 0)
        {
            printf("Received from fd %d: %s\n", fd_, buffer);
            send(fd_, buffer, nread, 0); // Echo back
        }
        else if (nread == -1 && (errno == EINTR))
        {
            continue; // Interrupted, try again
        }
        else if (nread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            // No more data to read
            break;
        }
        else if (nread == 0)
        {
            // Client closed connection
            // printf("Client fd %d closed connection\n", fd_);
            // close(fd_);
            closecallback_();
            break;
        }
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_ = fn;
}