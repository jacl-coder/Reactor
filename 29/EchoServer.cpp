#include "EchoServer.h"

EchoServer::EchoServer(const char *ip, uint16_t port, int subthreadnum, int workthreadnum)
    : tcpserver_(ip, port, subthreadnum), threadpool_(workthreadnum, "WORKS")
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

void EchoServer::HandleNewConnection(spConnection conn)
{
    std::cout << "New connection from " << conn->ip() << ":" << conn->port() << std::endl;
    // printf("EchoServer::HandleNewConnection thread is %d.\n", syscall(SYS_gettid));
}

void EchoServer::HandleClose(spConnection conn)
{
    std::cout << "Connection closed from " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleError(spConnection conn)
{
    std::cout << "Connection error from " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleMessage(spConnection conn, std::string &msg)
{
    // printf("EchoServer::HandleMessage thread is %d.\n", syscall(SYS_gettid));

    threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, msg));
}


void EchoServer::OnMessage(spConnection conn, std::string &msg)
{
    msg = "reply: " + msg;
    conn->seed(msg.data(), msg.size());
}

void EchoServer::HandleSendComplete(spConnection conn)
{
    std::cout << "Send complete to " << conn->ip() << ":" << conn->port() << std::endl;
}

/*
void EchoServer::HandleTimeOut(EventLoop *loop)
{
    std::cout << "EchoServer timeout" << std::endl;
}
*/