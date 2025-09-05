#include "Channel.h"

Channel::Channel(Epoll *ep, int fd, bool islisten)
    : ep_(ep), fd_(fd), islisten_(islisten)
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

void Channel::handleevent(Socket* servsock)
{
    // Handle data from clients
    if(revents_ & EPOLLRDHUP)
    {
        // Client disconnected
        printf("Client fd %d disconnected\n", fd_);
        close(fd_);
    }
    else if(revents_ & (EPOLLIN|EPOLLPRI))
    {
        if (islisten_ == true )
        {
            InetAddress clientaddr;
            Socket *clientsock = new Socket(servsock->accept(clientaddr));
            
            printf("New connection from %d:%s, fd: %d\n",clientsock->fd(), clientaddr.ip(), clientaddr.port());

            // ep.addfd(clientsock->fd(), EPOLLIN | EPOLLET); // Edge-triggered read
            Channel *clientchannel = new Channel(ep_, clientsock->fd(),false);
            clientchannel->useet(); 
            clientchannel->enablereading();
        }
        else
        {
            char buffer[4096];
            while(true)
            {  
                bzero(&buffer, sizeof(buffer));
                ssize_t nread = read(fd_, buffer, sizeof(buffer));
                if(nread > 0)
                {
                    printf("Received from fd %d: %s\n", fd_, buffer);
                    send(fd_, buffer, nread, 0); // Echo back
                }
                else if(nread == -1 && (errno == EINTR))
                {
                    continue; // Interrupted, try again
                }
                else if(nread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                {
                    // No more data to read
                    break;
                }
                else if(nread == 0)
                {
                    // Client closed connection
                    printf("Client fd %d closed connection\n", fd_);
                    close(fd_);
                    break;
                }
            }
        }
    }
    else if(revents_ & EPOLLOUT)
    {
        // Ready to write (not used in this example)
    }
    else
    {
        printf("Unexpected event for fd %d\n",fd_);
        close(fd_);
    }
}