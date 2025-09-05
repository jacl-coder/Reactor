#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

/*
    EchoServer: A simple echo server that uses TcpServer and Connection classes.
*/

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_;

public:
    EchoServer(const char *ip, uint16_t port, int subthreadnum = 4, int workthreadnum = 4);
    ~EchoServer();

    void Start();

    void HandleNewConnection(Connection *conn);
    void HandleClose(Connection *conn);
    void HandleError(Connection *conn);
    void HandleMessage(Connection *conn, std::string &msg);
    void HandleSendComplete(Connection *conn);
    // void HandleTimeOut(EventLoop *loop);

    void OnMessage(Connection *conn, std::string &msg);
};
