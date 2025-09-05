#include "EchoServer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s ip port\n", argv[0]);
        printf("example: ./echoserver  127.0.0.1 5005\n");
        return -1;
    }

    // TcpServer tcpserver(argv[1], atoi(argv[2]));
    // tcpserver.start();

    EchoServer echoserver(argv[1], atoi(argv[2]));
    echoserver.Start();

    return 0;
}