#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
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
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));

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

void TcpServer::onmessage(Connection *conn, std::string msg)
{
    msg = "reply: " + msg;

    int len = msg.size();
    std::string tmpbuf((char *)&len, 4);
    tmpbuf.append(msg);

    // send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn->seed(tmpbuf.data(), tmpbuf.size());
}

void TcpServer::sendcomplete(Connection *conn)
{
    printf("send complete for fd %d\n", conn->fd());
}

void TcpServer::epolltimeout(EventLoop *loop)
{
    printf("epoll_wait() timeout\n");
}