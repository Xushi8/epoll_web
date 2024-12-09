#pragma once

#include <epoll_web/common/common.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

BASIC_PROJECT_BEGIN_NAMESPACE

inline void client1()
{
    // 1. 获取服务器地址信息
    struct addrinfo hints = {};
    int check_error;

    // 设置 hints
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // 使用 getaddrinfo 获取服务器的地址信息
    struct addrinfo* res;
    check_error = getaddrinfo("localhost", "10000", &hints, &res);
    if (check_error != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(check_error));
        exit(1);
    }

    // 处理返回的 IPv6 地址
    struct addrinfo* p;
    for (p = res; p != NULL; p = p->ai_next)
    {
        // 只会遍历 IPv6 地址
        if (p->ai_family == AF_INET6)
        {
            char addr_str[INET6_ADDRSTRLEN];
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), addr_str, sizeof(addr_str));
            printf("IPv6 address: %s\n", addr_str);
        }
        else
        {
            char addr_str[INET_ADDRSTRLEN];
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), addr_str, sizeof(addr_str));
            printf("IPv4 address: %s\n", addr_str);
        }
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
    check_error = connect(fd, res->ai_addr, res->ai_addrlen);
    if (check_error == -1)
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
        sprintf(buf, "你好, 服务器...%d", number++);
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

        struct timespec sleep_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = 100'000'000;
        (void)sleep_time;
        nanosleep(&sleep_time, nullptr);
    }

    close(fd);
}

BASIC_PROJECT_END_NAMESPACE
