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
#include "InetAddress.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s ip port\n", argv[0]);
        printf("example: ./tcpepoll 127.0.0.1 5005\n");
        return 1;
    }

    int listenfd = socket(AF_INET, SOCK_STREAM|O_NONBLOCK, IPPROTO_TCP);
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

    // sockaddr_in servaddr;
    // memset(&servaddr, 0, sizeof(servaddr));
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    // servaddr.sin_port = htons(atoi(argv[2]));
    InetAddress servaddr(argv[1], atoi(argv[2]));

    if (bind(listenfd, servaddr.addr(), sizeof(servaddr)) != 0)
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

    epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN; // Level-triggered read
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

    epoll_event events[1024];

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
            // Handle data from clients
            if(events[i].events & EPOLLRDHUP)
            {
                // Client disconnected
                printf("Client fd %d disconnected\n", events[i].data.fd);
                close(events[i].data.fd);
            }
            else if(events[i].events & (EPOLLIN|EPOLLPRI))
            {
                if (events[i].data.fd == listenfd)
                {
                    // Accept new connections
                    sockaddr_in peeraddr;
                    socklen_t clientlen = sizeof(peeraddr);
                    int connfd = accept4(listenfd, (sockaddr *)&peeraddr, &clientlen, SOCK_NONBLOCK);

                    InetAddress clientaddr(peeraddr);
                    
                    printf("New connection from %s:%d, fd: %d\n", clientaddr.ip(), clientaddr.port(), connfd); 

                    ev.data.fd = connfd;
                    ev.events = EPOLLIN | EPOLLET; // Edge-triggered read
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
                }
                else
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
                            printf("Client fd %d closed connection\n", events[i].data.fd);
                            close(events[i].data.fd);
                            break;
                        }
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
    close(listenfd);
    close(epollfd);
    return 0;
}