#include "TcpServer.h"

/*
class TcpServer
{
private:
    EventLoop loop_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();
};
*/

TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    Socket *servsock = new Socket(createNonblocking());
    InetAddress servaddr(ip, port);
    servsock->setreuseaddr(true);
    servsock->setreuseport(true);
    servsock->setnodelay(true);
    servsock->setkeepalive(true);
    servsock->bind(servaddr);
    servsock->listen(128);
    
    Channel *servchannel = new Channel(&loop_, servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    servchannel->enablereading();
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    loop_.run();
}