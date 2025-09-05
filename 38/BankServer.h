#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

/*
    BankServer
*/

class UserInfo
{
private:
    int fg;
    std::string ip_;
    bool login_ = false;
    std::string name;
public:
    UserInfo(int fd,const std::string &ip) : fg(fd), ip_(ip) {}
    void setLogin(bool login) { login_ = login; }
    bool isLogin() const { return login_; }
};

class BankServer
{
private:
    using spUserInfo = std::shared_ptr<UserInfo>;
    TcpServer tcpserver_;
    ThreadPool threadpool_;
    std::mutex mutex_;
    std::map<int, spUserInfo> usermap_;

public:
    BankServer(const char *ip, uint16_t port, int subthreadnum = 4, int workthreadnum = 4);
    ~BankServer();

    void Start();
    void Stop();

    void HandleNewConnection(spConnection conn);
    void HandleClose(spConnection conn);
    void HandleError(spConnection conn);
    void HandleMessage(spConnection conn, std::string &msg);
    void HandleSendComplete(spConnection conn);
    // void HandleTimeOut(EventLoop *loop);

    void OnMessage(spConnection conn, std::string &msg);

    void HandleRemove(int fd);
};
