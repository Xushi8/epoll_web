#pragma once

#include <epoll_web/common/common.hpp>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <boost/endian.hpp>
#include <array>

EPOLL_WEB_BEGIN_NAMESPACE

namespace endian = boost::endian;

inline void server2(uint16_t port)
{
    int lfd = socket(AF_INET6, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in6 addr = {.sin6_family = AF_INET6, .sin6_port = endian::native_to_big(port), .sin6_flowinfo = {}, .sin6_addr = in6addr_any, .sin6_scope_id = {}};
    int ret = bind(lfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
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

    // epoll
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event ev{.events = EPOLLIN, .data{.fd = lfd}};

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl add");
        exit(1);
    }

    constexpr size_t maxx = 1024;
    std::array<struct epoll_event, maxx> evs; // NOLINT(cppcoreguidelines-pro-type-member-init)

    while (1)
    {
        int cnt = epoll_wait(epfd, evs.data(), maxx, -1);
        for (int i = 0; i < cnt; i++)
        {
            int curfd = evs[i].data.fd;
            if (curfd == lfd)
            {
                int client_fd = accept(curfd, nullptr, nullptr);
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                if (ret == -1)
                {
                    perror("epoll_ctl accept");
                    exit(1);
                }
            }
            else
            {
                char buf[1024];
                int len = recv(curfd, buf, sizeof(buf), 0);
                if (len > 0)
                {
                    printf("客户端 say: %s\n", buf);
                    send(curfd, buf, len, 0);
                }
                else if (len == 0)
                {
                    printf("客户端已经断开连接\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, nullptr);
                    close(curfd);
                }
                else
                {
                    perror("recv");
                    exit(1);
                }
            }
        }
    }
}

EPOLL_WEB_END_NAMESPACE
