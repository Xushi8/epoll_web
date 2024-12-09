#pragma once

#include <epoll_web/common/common.hpp>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <boost/endian.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

namespace endian = boost::endian;

inline void server1()
{
    int lfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        exit(1);
    }

    sockaddr_in6 addr = {.sin6_family = AF_INET6, .sin6_port = endian::native_to_big(static_cast<uint16_t>(10000)), .sin6_flowinfo = {}, .sin6_addr = in6addr_any, .sin6_scope_id = {}};
    int ret = bind(lfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(lfd, SOMAXCONN);
    if (ret == -1)
    {
        perror("listen");
        exit(1);
    }

    // 4. 阻塞等待并接受客户端连接
    sockaddr_in cliaddr; // NOLINT(cppcoreguidelines-pro-type-member-init)
    unsigned int clilen = sizeof(cliaddr);
    int cfd = accept(lfd, reinterpret_cast<sockaddr*>(&cliaddr), &clilen);
    if (cfd == -1)
    {
        perror("accept");
        exit(1);
    }
    // 打印客户端的地址信息
    char ip[24] = {0};
    printf("客户端的IP地址: %s, 端口: %d\n",
        inet_ntop(AF_INET6, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
        ntohs(cliaddr.sin_port));

    // 5. 和客户端通信
    while (1)
    {
        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int len = read(cfd, buf, sizeof(buf));
        if (len > 0)
        {
            printf("客户端say: %s\n", buf);
            ssize_t x = write(cfd, buf, len);
            if (x == -1)
            {
                fprintf(stderr, "回复错误\n");
                break;
            }
        }
        else if (len == 0)
        {
            printf("客户端断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }

    close(cfd);
    close(lfd);
}

EPOLL_WEB_END_NAMESPACE
