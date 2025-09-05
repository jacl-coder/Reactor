#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port)
    : loop_(loop), servsock_(createNonblocking()), acceptchannel_(loop, servsock_.fd())
{
    // servsock_ = new Socket(createNonblocking());
    InetAddress servaddr(ip, port);
    servsock_.setreuseaddr(true);
    servsock_.setreuseport(true);
    servsock_.setnodelay(true);
    servsock_.setkeepalive(true);
    servsock_.bind(servaddr);
    servsock_.listen(128);

    // acceptchannel_ = new Channel(loop_, servsock_.fd());
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnection, this));
    acceptchannel_.enablereading();
}

Acceptor::~Acceptor()
{
    // delete servsock_;
    // delete acceptchannel_;
}

void Acceptor::newconnection()
{
    InetAddress clientaddr;
    std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
    clientsock->setipport(clientaddr.ip(), clientaddr.port());

    newconnectioncb_(std::move(clientsock));
}

void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn)
{
    newconnectioncb_ = fn;
}
