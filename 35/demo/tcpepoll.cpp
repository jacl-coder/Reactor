#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>

void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

void fun(int sig)
{
    printf("catch a signal %d\n", sig);
    alarm(5);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s ip port\n", argv[0]);
        printf("example: ./tcpepoll 127.0.0.1 5005\n");
        return 1;
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd == -1)
    {
        perror("socket()");
        return 1;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof(opt)));

    setnonblocking(listenfd);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("bind()");
        close(listenfd);
        return 1;
    }

    if (listen(listenfd, 128) != 0)
    {
        perror("listen()");
        close(listenfd);
        return 1;
    }

    int epollfd = epoll_create(1);

    struct epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN; // Level-triggered read
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

    struct epoll_event events[1024];

    int tfd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.it_value.tv_sec = 5; // Initial expiration
    timeout.it_interval.tv_sec = 0; // No periodic expiration
    timerfd_settime(tfd, 0, &timeout, NULL);
    ev.data.fd = tfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, tfd, &ev);

    // sigset_t sigset;
    // sigemptyset(&sigset);
    // sigaddset(&sigset, SIGINT);
    // sigaddset(&sigset, SIGTERM);
    // sigprocmask(SIG_BLOCK, &sigset, NULL);
    // int sigfd = signalfd(-1, &sigset, 0);
    // ev.data.fd = sigfd;
    // ev.events = EPOLLIN;
    // epoll_ctl(epollfd, EPOLL_CTL_ADD, sigfd, &ev);

    while (true)
    {
        int nfds = epoll_wait(epollfd, events, 1024, -1);
        if (nfds == -1)
        {
            perror("epoll_wait()");
            break;
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == listenfd)
            {
                // Accept new connections
                struct sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(clientaddr);
                int connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
                setnonblocking(connfd);
                
                printf("New connection from %s:%d, fd: %d\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port),connfd);

                ev.data.fd = connfd;
                ev.events = EPOLLIN | EPOLLET; // Edge-triggered read
                epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else
            {
                // Handle data from clients
                if(events[i].events & EPOLLRDHUP)
                {
                    // Client disconnected
                    printf("11Client fd %d disconnected\n", events[i].data.fd);
                    close(events[i].data.fd);
                }
                else if(events[i].events & (EPOLLIN|EPOLLPRI))
                {
                    char buffer[4096];
                    while(true)
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(events[i].data.fd, buffer, sizeof(buffer));
                        if(nread > 0)
                        {
                            printf("Received from fd %d: %s\n", events[i].data.fd, buffer);
                            send(events[i].data.fd, buffer, nread, 0); // Echo back
                        }
                        else if(nread == -1 && (errno == EINTR))
                        {
                            continue; // Interrupted, try again
                        }
                        else if(nread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                        {
                            // No more data to read
                            break;
                        }
                        else if(nread == 0)
                        {
                            // Client closed connection
                            printf("22Client fd %d closed connection\n", events[i].data.fd);
                            close(events[i].data.fd);
                            break;
                        }
                    }
                }
                else if(events[i].events & EPOLLOUT)
                {
                    // Ready to write (not used in this example)
                }
                else
                {
                    printf("Unexpected event for fd %d\n", events[i].data.fd);
                    close(events[i].data.fd);
                }
            }
        }
    }
    close(listenfd);
    close(epollfd);
    return 0;
}