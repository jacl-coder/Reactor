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
#include "EventLoop.h"

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
   
    EventLoop loop;
    // Epoll ep;
    Channel *servchannel = new Channel(&loop, servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &servsock));
    servchannel->enablereading();

    loop.run();
    /*
    while (true)
    {
        std::vector<Channel *> channels = ep.loop();
        for (auto &ch : channels)
        {
            ch->handleevent();
        }
    }
    */
    return 0;
}