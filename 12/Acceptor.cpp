#include "Acceptor.h"

/*
class Acceptor
{
private:
    EventLoop *loop_;
    Socket* servsock_;
    Channel* acceptchannel_;
public:
    Acceptor(EventLoop* loop, const std::string &ip, const uint16_t port);
    ~Acceptor();
};
*/

Acceptor::Acceptor(EventLoop* loop, const std::string &ip, const uint16_t port)
    : loop_(loop)
{
    servsock_ = new Socket(createNonblocking());
    InetAddress servaddr(ip, port);
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
    servsock_->setnodelay(true);
    servsock_->setkeepalive(true);
    servsock_->bind(servaddr);
    servsock_->listen(128);

    acceptchannel_ = new Channel(loop_, servsock_->fd());
    acceptchannel_->setreadcallback(std::bind(&Channel::newconnection, acceptchannel_, servsock_));
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete acceptchannel_;
    delete servsock_;
}