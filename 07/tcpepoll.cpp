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
   
    Epoll ep;
    Channel *servchannel = new Channel(&ep, servsock.fd(), true);
    servchannel->enablereading();

    while (true)
    {
        std::vector<Channel *> channels = ep.loop();
        for (auto &ch : channels)
        {
            ch->handleevent(&servsock);
            /*
            // Handle data from clients
            if(ch->revents() & EPOLLRDHUP)
            {
                // Client disconnected
                printf("Client fd %d disconnected\n", ch->fd());
                close(ch->fd());
            }
            else if(ch->revents() & (EPOLLIN|EPOLLPRI))
            {
                if (ch == servchannel )
                {
                    InetAddress clientaddr;
                    Socket *clientsock = new Socket(servsock.accept(clientaddr));
                    
                    printf("New connection from %d:%s, fd: %d\n",clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    // ep.addfd(clientsock->fd(), EPOLLIN | EPOLLET); // Edge-triggered read
                    Channel *clientchannel = new Channel(&ep, clientsock->fd(), false);
                    clientchannel->useet(); 
                    clientchannel->enablereading();
                }
                else
                {
                    char buffer[4096];
                    while(true)
                    {  
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(ch->fd(), buffer, sizeof(buffer));
                        if(nread > 0)
                        {
                            printf("Received from fd %d: %s\n", ch->fd(), buffer);
                            send(ch->fd(), buffer, nread, 0); // Echo back
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
                            printf("Client fd %d closed connection\n", ch->fd());
                            close(ch->fd());
                            break;
                        }
                    }
                }
            }
            else if(ch->revents() & EPOLLOUT)
            {
                // Ready to write (not used in this example)
            }
            else
            {
                printf("Unexpected event for fd %d\n",ch->fd());
                close(ch->fd());
            }
            */
        }
    }
    return 0;
}