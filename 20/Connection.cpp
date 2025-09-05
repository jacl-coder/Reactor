#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock)
    : loop_(loop), clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientchannel_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    // printf("Client fd %d disconnected\n", fd());
    // close(fd());
    closecallback_(this);
}

void Connection::errorcallback()
{
    // printf("Unexpected event for fd %d\n", fd());
    // close(fd());
    errorcallback_(this);
}

void Connection::setclosecallback(std::function<void(Connection *)> fn)
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(Connection *)> fn)
{
    errorcallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(Connection *, std::string)> fn)
{
    onmessagecallback_ = fn;
}

void Connection::onmessage()
{
    char buffer[4096];
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));
        if (nread > 0)
        {
            // printf("Received from fd %d: %s\n", fd(), buffer);
            // send(fd(), buffer, nread, 0); // Echo back
            inputbuffer_.append(buffer, nread);
        }
        else if (nread == -1 && (errno == EINTR))
        {
            continue; // Interrupted, try again
        }
        else if (nread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            // No more data to read
            // printf("Received from fd %d: %s\n", fd(), inputbuffer_.data());
            while (true)
            {
                int len;
                memcpy(&len, inputbuffer_.data(), 4);
                if(inputbuffer_.size() < (size_t)(len + 4)) break;

                std::string msg(inputbuffer_.data() + 4, len);
                inputbuffer_.erase(0, len + 4);
                printf("message from fd %d: %s\n", fd(), msg.c_str());
                
                /*
                msg = "reply: " + msg;

                len = msg.size();
                std::string tmpbuf((char *)&len, 4);
                tmpbuf.append(msg);

                send(fd(), tmpbuf.data(), tmpbuf.size(), 0);
                */
                onmessagecallback_(this, msg);
            }
            break;
        }
        else if (nread == 0)
        {
            closecallback();
            break;
        }
    }
}

