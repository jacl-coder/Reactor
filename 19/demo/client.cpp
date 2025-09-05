#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

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

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ printf("socket error: %s\n", strerror(errno)); return 1; }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s\n", strerror(errno));
    }
    
    printf("Connected to %s:%s\n", argv[1], argv[2]);

    for(int i = 0; i < 1000; ++i)
    {
        memset(buf, 0, sizeof(buf));
        // printf("please input: ");
        // scanf("%s", buf);
        sprintf(buf, "Hello %d", i);

        if(send(sockfd, buf, strlen(buf), 0) <= 0)
        {
            printf("send error: %s\n", strerror(errno));
        }

        printf("Received: %s\n", buf);
    }
}