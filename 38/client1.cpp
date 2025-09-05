#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

ssize_t tcpsend(int fd, void *data, size_t size)
{
    char tmpbuf[1024];
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, &size, 4);
    memcpy(tmpbuf + 4, data, size);

    return send(fd, tmpbuf, size + 4, 0);
}

ssize_t tcprecv(int fd, void *data, size_t size)
{
    int len;
    recv(fd, &len, 4, 0);
    return recv(fd, data, len, 0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s server_ip server_port\n", argv[0]);
        printf("example: ./client 127.0.0.1 5005\n");
        return 1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s\n", strerror(errno));
    }

    printf("Connected to %s:%s\n", argv[1], argv[2]);

    printf("start time: %ld\n", time(NULL));
    // login
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "<bizcode>00101</bizcode><username>jacl</username><password>123456</password>");
    printf("send to server:%s\n", buf);
    if (tcpsend(sockfd, buf, strlen(buf)) <= 0)
    {
        printf("tcpsend error: %s\n", strerror(errno));
        return -1;
    }
    memset(buf, 0, sizeof(buf));
    if (tcprecv(sockfd, buf, sizeof(buf)) <= 0)
    {
        printf("tcprecv error: %s\n", strerror(errno));
        return -1;
    }
    printf("recv from server:%s\n", buf);

    // query
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "<bizcode>00201</bizcode><cardid>6222020200041234567</cardid>");
    if (tcpsend(sockfd, buf, strlen(buf)) <= 0)
    {
        printf("tcpsend error: %s\n", strerror(errno));
        return -1;
    }
    printf("send to server:%s\n", buf);
    memset(buf, 0, sizeof(buf));
    if (tcprecv(sockfd, buf, sizeof(buf)) <= 0)
    {
        printf("tcprecv error: %s\n", strerror(errno));
        return -1;
    }
    printf("recv from server:%s\n", buf);

    // heartbeat
    while (true)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "<bizcode>00001</bizcode>");
        if (tcpsend(sockfd, buf, strlen(buf)) <= 0)
        {
            printf("tcpsend error: %s\n", strerror(errno));
            return -1;
        }
        printf("send to server:%s\n", buf);
        memset(buf, 0, sizeof(buf));
        if (tcprecv(sockfd, buf, sizeof(buf)) <= 0)
        {
            printf("tcprecv error: %s\n", strerror(errno));
            return -1;
        }
        printf("recv from server:%s\n", buf);
        sleep(5);
    }

    // logout
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "<bizcode>00901</bizcode>");
    if (tcpsend(sockfd, buf, strlen(buf)) <= 0)
    {
        printf("tcpsend error: %s\n", strerror(errno));
        return -1;
    }
    printf("send to server:%s\n", buf);
    memset(buf, 0, sizeof(buf));
    if (tcprecv(sockfd, buf, sizeof(buf)) <= 0)
    {
        printf("tcprecv error: %s\n", strerror(errno));
        return -1;
    }
    printf("recv from server:%s\n", buf);

    printf("end time: %ld\n", time(NULL));
}