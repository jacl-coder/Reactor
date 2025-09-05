#include "Connection.h"

/*
class Connection
{
private:
    EventLoop *loop_;
    Socket* clientsock_;
    Channel* clientchannel_;
public:
    Connection(EventLoop* loop, Socket* clientsock);
    ~Connection();
};
*/

Connection::Connection(EventLoop *loop, Socket *clientsock)
    : loop_(loop), clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage, clientchannel_));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientchannel_;
}