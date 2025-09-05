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

    void HandleNewConnection(spConnection conn);
    void HandleClose(spConnection conn);
    void HandleError(spConnection conn);
    void HandleMessage(spConnection conn, std::string &msg);
    void HandleSendComplete(spConnection conn);
    // void HandleTimeOut(EventLoop *loop);

    void OnMessage(spConnection conn, std::string &msg);
};
