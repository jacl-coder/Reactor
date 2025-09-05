#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/eventfd.h>

int main(int argc, char *argv[])
{
    int efd = eventfd(0, EFD_SEMAPHORE);

    uint64_t buf = 1;
    ssize_t ret;

    ret = write(efd, &buf, sizeof(uint64_t));
    ret = write(efd, &buf, sizeof(uint64_t));

    ret = read(efd, &buf, sizeof(uint64_t));
    printf("ret=%d, buf=%d\n", ret, buf);

    ret = read(efd, &buf, sizeof(uint64_t));
    printf("ret=%d, buf=%d\n", ret, buf);

    close(efd);
    return 0;
}