#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/log.hpp>
#include <epoll_web/game/game.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <nlohmann/json.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

inline void client1(std::string_view addr, std::string_view port)
{
    struct addrinfo hints = {};
    int check_error;

    // 设置 hints
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;
    check_error = getaddrinfo(addr.data(), port.data(), &hints, &res); // NOLINT(bugprone-suspicious-stringview-data-usage)
    if (check_error != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(check_error));
        exit(1);
    }

    struct addrinfo* p;
    for (p = res; p != nullptr; p = p->ai_next)
    {
        if (p->ai_family == AF_INET6)
        {
            char addr_str[INET6_ADDRSTRLEN];
            struct sockaddr_in6* ipv6 = reinterpret_cast<struct sockaddr_in6*>(p->ai_addr);
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), addr_str, sizeof(addr_str));
            printf("IPv6 address: %s\n", addr_str);
        }
        else
        {
            char addr_str[INET_ADDRSTRLEN];
            struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
            inet_ntop(AF_INET, &(ipv4->sin_addr), addr_str, sizeof(addr_str));
            printf("IPv4 address: %s\n", addr_str);
        }
    }

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == -1)
    {
        perror("socket");
        freeaddrinfo(res);
        exit(1);
    }

    check_error = connect(fd, res->ai_addr, res->ai_addrlen);
    if (check_error == -1)
    {
        perror("connect");
        close(fd);
        freeaddrinfo(res);
        exit(1);
    }
    freeaddrinfo(res);

    std::jthread t_getch([fd]
        {
            while (1)
            {
                int ch = getch();
                if (ch == 'q')
                {
                    return;
                }

                if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd')
                {
                    char tmp[2];
                    tmp[0] = ch;
                    tmp[1] = '\0';
                    int res = send(fd, tmp, 1, 0);
                    if (res == -1)
                    {
                        break;
                    }
                }
            } });
    t_getch.detach();

    // 初始化 ncurses
    initscr();
    noecho();
    curs_set(0);

    while (1)
    {
        char buf[65536];

        ssize_t len = recv(fd, buf, sizeof(buf), 0);
        if (len > 0)
        {
            spdlog::trace("服务器说: {}", buf);

            nlohmann::json j = nlohmann::json::parse(buf);
            j.get_to(epoll_web::Game::get_instance());

            epoll_web::Game::get_instance().print();
        }
        else if (len == 0)
        {
            spdlog::info("服务器断开了连接");
            break;
        }
        else
        {
            perror("recv");
            break;
        }
    }

    close(fd);
    endwin();
}

EPOLL_WEB_END_NAMESPACE
