#include <epoll_learn/network/client1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

void client1(void)
{
    // 1. 获取服务器地址信息
    struct addrinfo hints = {}, *res;
    int ret;

    // 设置 hints
    hints.ai_family = AF_INET6;      // IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    // 使用 getaddrinfo 获取服务器的地址信息
    ret = getaddrinfo("localhost", "10000", &hints, &res);
    if (ret != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

    // 2. 创建套接字
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == -1)
    {
        perror("socket");
        freeaddrinfo(res); // 释放 getaddrinfo 返回的内存
        exit(1);
    }

    // 3. 连接服务器
    ret = connect(fd, res->ai_addr, res->ai_addrlen);
    if (ret == -1)
    {
        perror("connect");
        close(fd);
        freeaddrinfo(res); // 释放 getaddrinfo 返回的内存
        exit(1);
    }

    freeaddrinfo(res); // 连接成功后可以释放 res

    // 4. 和服务器端通信
    int number = 0;
    while (1)
    {
        // 发送数据
        char buf[1024];
        sprintf(buf, "你好, 服务器...%d\n", number++);
        write(fd, buf, strlen(buf) + 1);

        // 接收数据
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if (len > 0)
        {
            printf("服务器say: %s\n", buf);
        }
        else if (len == 0)
        {
            printf("服务器断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        sleep(1); // 每隔1s发送一条数据
    }

    close(fd);
}