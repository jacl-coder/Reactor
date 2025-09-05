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
#include "Socket.h"
#include "Epoll.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s ip port\n", argv[0]);
        printf("example: ./tcpepoll 127.0.0.1 5005\n");
        return -1;
    }

    Socket servsock(createNonblocking());
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.setnodelay(true);
    servsock.setkeepalive(true);
    servsock.bind(servaddr);
    servsock.listen(128); 

    /*
    int epollfd = epoll_create(1);

    epoll_event ev;
    ev.data.fd = servsock.fd();
    ev.events = EPOLLIN; // Level-triggered read
    epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock.fd(), &ev);

    epoll_event events[1024];
    */
   
    Epoll epoll;
    epoll.addfd(servsock.fd(), EPOLLIN); // Level-triggered read
    std::vector<epoll_event> evs;

    while (true)
    {   
        /*
        int nfds = epoll_wait(epollfd, events, 1024, -1);
        if (nfds == -1)
        {
            perror("epoll_wait()");
            break;
        }
        if (nfds == 0)
        {
            // Timeout, no events
            printf("epoll_wait() timeout\n"); 
            continue;
        }
        */
        evs = epoll.loop();

        // for (int i = 0; i < nfds; ++i)
        for (auto &ev : evs)
        {
            // Handle data from clients
            if(ev.events & EPOLLRDHUP)
            {
                // Client disconnected
                printf("Client fd %d disconnected\n", ev.data.fd);
                close(ev.data.fd);
            }
            else if(ev.events & (EPOLLIN|EPOLLPRI))
            {
                if (ev.data.fd == servsock.fd())
                {
                    InetAddress clientaddr;
                    Socket *clientsock = new Socket(servsock.accept(clientaddr));
                    
                    printf("New connection from %d:%s, fd: %d\n",clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    // ev.data.fd = clientsock->fd();
                    // ev.events = EPOLLIN | EPOLLET; // Edge-triggered read
                    // epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
                    epoll.addfd(clientsock->fd(), EPOLLIN | EPOLLET); // Edge-triggered read
                }
                else
                {
                    char buffer[4096];
                    while(true)
                    {  
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(ev.data.fd, buffer, sizeof(buffer));
                        if(nread > 0)
                        {
                            printf("Received from fd %d: %s\n", ev.data.fd, buffer);
                            send(ev.data.fd, buffer, nread, 0); // Echo back
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
                            printf("Client fd %d closed connection\n", ev.data.fd);
                            close(ev.data.fd);
                            break;
                        }
                    }
                }
            }
            else if(ev.events & EPOLLOUT)
            {
                // Ready to write (not used in this example)
            }
            else
            {
                printf("Unexpected event for fd %d\n", ev.data.fd);
                close(ev.data.fd);
            }
        }
    }
    return 0;
}