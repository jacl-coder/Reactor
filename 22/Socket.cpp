#include "Socket.h"

int createNonblocking()
{
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd < 0)
    {
        // perror("socket()");
        printf("%s:%s:%s listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return fd;
}

Socket::Socket(int fd) : fd_(fd)
{
}

Socket::~Socket()
{
    ::close(fd_);
}

int Socket::fd() const
{
    return fd_;
}

std::string Socket::ip() const
{
    return ip_;
}

uint16_t Socket::port() const
{
    return port_;
}

void Socket::setreuseaddr(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::setreuseport(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::setnodelay(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::setkeepalive(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::bind(const InetAddress &servaddr)
{
    if (::bind(fd_, servaddr.addr(), sizeof(servaddr)) != 0)
    {
        printf("%s:%s:%s bind error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    setipport(servaddr.ip(), servaddr.port());
}

void Socket::setipport(const std::string &ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;
}

void Socket::listen(int n)
{
    if (::listen(fd_, n) != 0)
    {
        printf("%s:%s:%s listen error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
}

int Socket::accept(InetAddress& clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = ::accept4(fd_, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);
    clientaddr.setaddr(peeraddr);

    return clientfd;
} 