#include "BankServer.h"

BankServer::BankServer(const char *ip, uint16_t port, int subthreadnum, int workthreadnum)
    : tcpserver_(ip, port, subthreadnum), threadpool_(workthreadnum, "WORKS")
{
    tcpserver_.setnewconnectioncb(std::bind(&BankServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&BankServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&BankServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&BankServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&BankServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&BankServer::HandleTimeOut, this, std::placeholders::_1));
    tcpserver_.setremoveconnectioncb(std::bind(&BankServer::HandleRemove, this, std::placeholders::_1));
}

BankServer::~BankServer()
{
}

void BankServer::Start()
{
    tcpserver_.start();
}

void BankServer::Stop()
{
    threadpool_.stop();
    printf("BankServer stopped.\n");

    tcpserver_.stop();
    printf("BankServer TcpServer stopped.\n");
}

void BankServer::HandleNewConnection(spConnection conn)
{
    spUserInfo userinfo(new UserInfo(conn->fd(), conn->ip()));
    {
        std::lock_guard<std::mutex> gd(mutex_);
        usermap_[conn->fd()] = userinfo;
    }
    printf("%s new connection %d from %s:%d.\n", Timestamp::now().tostring().c_str(), conn->fd(), conn->ip().c_str(), conn->port());
}

void BankServer::HandleClose(spConnection conn)
{
    printf("%s connection %d from %s:%d closed.\n", Timestamp::now().tostring().c_str(), conn->fd(), conn->ip().c_str(), conn->port());
    {
        std::lock_guard<std::mutex> gd(mutex_);
        usermap_.erase(conn->fd());
    }
}

void BankServer::HandleError(spConnection conn)
{
    HandleClose(conn);
}

void BankServer::HandleMessage(spConnection conn, std::string &msg)
{
    if (threadpool_.size() == 0)
    {
        OnMessage(conn, msg);
    }
    else
    {
        threadpool_.addtask(std::bind(&BankServer::OnMessage, this, conn, msg));
    }
}

bool getxmlbuffer(const std::string &xmlbuff, const std::string &fieldname, std::string &value, const int ilen = 0)
{
    std::string start = "<" + fieldname + ">";
    std::string end = "</" + fieldname + ">";

    int startp = xmlbuff.find(start);
    if (startp == std::string::npos)
        return false;

    int endp = xmlbuff.find(end, startp + start.size());
    if (endp == std::string::npos)
        return false;

    int itmplen = endp - startp - start.length();
    if (ilen > 0 && ilen < itmplen)
        itmplen = ilen;
    value = xmlbuff.substr(startp + start.length(), itmplen);

    return true;
}

void BankServer::OnMessage(spConnection conn, std::string &msg)
{
    spUserInfo userinfo = usermap_[conn->fd()];

    std::string bizcode;
    std::string replaymsg;

    getxmlbuffer(msg, "bizcode", bizcode);

    if (bizcode == "00101")
    {
        std::string username, password;
        getxmlbuffer(msg, "username", username);
        getxmlbuffer(msg, "password", password);
        if (username == "jacl" && password == "123456")
        {
            userinfo->setLogin(true);
            replaymsg = "<bizcode>00102</bizcode><retcode>0</retcode><msg>login success</msg>";
        }
        else
        {
            replaymsg = "<bizcode>00102</bizcode><retcode>-1</retcode><msg>login failed</msg>";
        }
    }
    else if (bizcode == "00201")
    {
        if (!userinfo->isLogin())
        {
            replaymsg = "<bizcode>00202</bizcode><retcode>-1</retcode><msg>not login</msg>";
        }
        else
        {
            replaymsg = "<bizcode>00202</bizcode><retcode>0</retcode><msg>1000.00</msg>";
        }
    }
    else if (bizcode == "00901")
    {
        if (!userinfo->isLogin())
        {
            replaymsg = "<bizcode>00902</bizcode><retcode>-1</retcode><msg>not login</msg>";
        }
        else
        {
            replaymsg = "<bizcode>00902</bizcode><retcode>0</retcode><msg>logout success</msg>";
            userinfo->setLogin(false);
        }
    }
    else if (bizcode == "00001")
    {
        if(!userinfo->isLogin())
        {
            replaymsg = "<bizcode>00002</bizcode><retcode>-1</retcode><msg>not login</msg>";
        }
        else
        {
            replaymsg = "<bizcode>00002</bizcode><retcode>0</retcode><msg>ok</msg>";
        }
    }
    conn->seed(replaymsg.data(), replaymsg.size());
}

void BankServer::HandleSendComplete(spConnection conn)
{
}

/*
void BankServer::HandleTimeOut(EventLoop *loop)
{
    std::cout << "BankServer timeout" << std::endl;
}
*/

void BankServer::HandleRemove(int fd)
{
    printf("fd=%d removed from usermap_\n", fd);
    std::lock_guard<std::mutex> gd(mutex_);
    usermap_.erase(fd);
}