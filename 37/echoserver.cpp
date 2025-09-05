#include "EchoServer.h"
#include <signal.h>

EchoServer *echoserver;

void Stop(int sig)
{
    printf("catch signal %d\n", sig);
    echoserver->Stop();
    printf("EchoServer exiting...\n");
    delete echoserver;
    printf("EchoServer exited.\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s ip port\n", argv[0]);
        printf("example: ./echoserver  127.0.0.1 5005\n");
        return -1;
    }

    signal(SIGINT, Stop);
    signal(SIGTERM, Stop);

    echoserver = new EchoServer(argv[1], atoi(argv[2]), 25, 0);
    echoserver->Start();

    return 0;
}