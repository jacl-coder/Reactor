#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for (auto &p : conns_)
    {
        delete p.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newconnection(Socket *clientsock)
{
    Connection *conn = new Connection(&loop_, clientsock);
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));

    printf("New connection from %s:%d, fd: %d\n", conn->ip().c_str(), conn->port(), conn->fd());

    conns_[conn->fd()] = conn;
}

void TcpServer::closeconnection(Connection *conn)
{
    printf("Client fd %d disconnected\n", conn->fd());
    // close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection *conn)
{
    printf("Unexpected event for fd %d\n", conn->fd());
    // close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}