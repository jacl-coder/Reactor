#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port)
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
    // acceptchannel_->setreadcallback(std::bind(&Channel::newconnection, acceptchannel_, servsock_));
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconnection, this));
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete acceptchannel_;
    delete servsock_;
}

#include "Connection.h"
void Acceptor::newconnection()
{
    InetAddress clientaddr;
    Socket *clientsock = new Socket(servsock_->accept(clientaddr));

    // printf("New connection from %s:%d, fd: %d\n", clientaddr.ip(), clientaddr.port(), clientsock->fd());

    // Connection *conn = new Connection(loop_, clientsock);
    newconnectioncb_(clientsock);
}

void Acceptor::setnewconnectioncb(std::function<void(Socket *)> fn)
{
    newconnectioncb_ = fn;
}
