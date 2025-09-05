#include "EchoServer.h"

/*
class EchoServer
{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const char *ip, uint16_t port);
    ~EchoServer();

    void Start();

    void HandleNewConnection(Socket *clientsock);
    void HandleClose(Connection *conn);
    void HandleError(Connection *conn);
    void HandleMessage(Connection *conn, std::string msg);
    void HandleSendComplete(Connection *conn);
    void HandleTimeOut(EventLoop *loop);
};
*/

EchoServer::EchoServer(const char *ip, uint16_t port) : tcpserver_(ip, port)
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}

void EchoServer::Start()
{
    tcpserver_.start();
}

void EchoServer::HandleNewConnection(Connection *conn)
{
    std::cout << "New connection from " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleClose(Connection *conn)
{
    std::cout << "Connection closed from " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleError(Connection *conn)
{
    std::cout << "Connection error from " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleMessage(Connection *conn, std::string msg)
{
    msg = "reply: " + msg;

    int len = msg.size();
    std::string tmpbuf((char *)&len, 4);
    tmpbuf.append(msg);

    // send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn->seed(tmpbuf.data(), tmpbuf.size());
}

void EchoServer::HandleSendComplete(Connection *conn)
{
    std::cout << "Send complete to " << conn->ip() << ":" << conn->port() << std::endl;
}

/*
void EchoServer::HandleTimeOut(EventLoop *loop)
{
    std::cout << "EchoServer timeout" << std::endl;
}
*/