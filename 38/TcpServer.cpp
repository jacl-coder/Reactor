#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadnum)
    : threadnum_(threadnum), mainloop_(new EventLoop(true)), acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum_, "IO")
{
    // mainloop_ = new EventLoop;
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));

    // acceptor_ = new Acceptor(mainloop_, ip, port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));

    // threadpool_ = new ThreadPool(threadnum_, "IO");

    for (int i = 0; i < threadnum_; i++)
    {
        subloops_.emplace_back(new EventLoop(false, 5, 10));
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        subloops_[i]->settimercallback(std::bind(&TcpServer::removeconn, this, std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subloops_[i].get()));
    }
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::stop()
{
    mainloop_->stop();
    printf("TcpServer main loop stopped.\n");

    for (int i = 0; i < threadnum_; i++)
    {
        subloops_[i]->stop();
    }
    printf("TcpServer sub loops stopped.\n");

    threadpool_.stop();
    printf("TcpServer IO threads stopped.\n");
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

    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_[fd] = conn;
    }
    subloops_[fd % threadnum_]->newconnection(conn);

    if (newconnectioncb_)
        newconnectioncb_(conn);
}

void TcpServer::closeconnection(spConnection conn)
{
    if (closeconnectioncb_)
        closeconnectioncb_(conn);

    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(conn->fd());
    }
}

void TcpServer::errorconnection(spConnection conn)
{
    if (errorconnectioncb_)
        errorconnectioncb_(conn);

    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(conn->fd());
    }
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

void TcpServer::removeconn(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(fd);
    }
    if (removeconnectioncb_)
        removeconnectioncb_(fd);
}

void TcpServer::setremoveconnectioncb(std::function<void(int)> fn)
{
    removeconnectioncb_ = fn;
}
