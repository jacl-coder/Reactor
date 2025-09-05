#include "BankServer.h"
#include <signal.h>

BankServer *bankserver;

void Stop(int sig)
{
    printf("catch signal %d\n", sig);
    bankserver->Stop();
    printf("BankServer exiting...\n");
    delete bankserver;
    printf("BankServer exited.\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s ip port\n", argv[0]);
        printf("example: ./bankserver  127.0.0.1 5005\n");
        return -1;
    }

    signal(SIGINT, Stop);
    signal(SIGTERM, Stop);

    bankserver = new BankServer(argv[1], atoi(argv[2]), 25, 0);
    bankserver->Start();

    return 0;
}