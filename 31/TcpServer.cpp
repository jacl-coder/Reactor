#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadnum)
    : threadnum_(threadnum), mainloop_(new EventLoop), acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum_, "IO")
{
    // mainloop_ = new EventLoop;
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));

    // acceptor_ = new Acceptor(mainloop_, ip, port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));

    // threadpool_ = new ThreadPool(threadnum_, "IO");

    for (int i = 0; i < threadnum_; i++)
    {
        subloops_.emplace_back(new EventLoop);
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subloops_[i].get()));
    }
}

TcpServer::~TcpServer()
{
    // delete acceptor_;
    // delete mainloop_;

    /*
    for (auto &p : subloops_)
    {
        delete p;
    }
    */
    // delete threadpool_;
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)
{
    int fd = clientsock->fd();
    spConnection conn(new Connection(subloops_[fd % threadnum_].get(), std::move(clientsock)));
    // spConnection conn(new Connection(subloops_[clientsock->fd() % threadnum_], std::move(clientsock)));
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));

    conns_[conn->fd()] = conn;

    if (newconnectioncb_)
        newconnectioncb_(conn);
}

void TcpServer::closeconnection(spConnection conn)
{
    if (closeconnectioncb_)
        closeconnectioncb_(conn);
    conns_.erase(conn->fd());
    // delete conn;
}

void TcpServer::errorconnection(spConnection conn)
{
    if (errorconnectioncb_)
        errorconnectioncb_(conn);
    conns_.erase(conn->fd());
    // delete conn;
}

void TcpServer::onmessage(spConnection conn, std::string &msg)
{

    if (onmessagecb_)
        onmessagecb_(conn, msg);
}

void TcpServer::sendcomplete(spConnection conn)
{
    if (sendcompletecb_)
        sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop *loop)
{
    if (timeoutcb_)
        timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    newconnectioncb_ = fn;
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    closeconnectioncb_ = fn;
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    errorconnectioncb_ = fn;
}

void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &msg)> fn)
{
    onmessagecb_ = fn;
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_ = fn;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop *)> fn)
{
    timeoutcb_ = fn;
}
