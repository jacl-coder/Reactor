#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    if (pipe2(pipefd, O_NONBLOCK) == -1)
    {
        perror("pipe error");
        exit(1);
    }

    write(pipefd[1], "hello world",11);

    ssize_t ret;
    char buf[128] = {0};

    ret = read(pipefd[0], buf, sizeof(buf));
    printf("ret=%d, buf=%s\n", (int)ret, buf);

    ret = read(pipefd[0], buf, sizeof(buf));
    printf("ret=%d, buf=%s\n", (int)ret, buf);

    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}