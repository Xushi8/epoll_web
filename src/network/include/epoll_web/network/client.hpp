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

#include <fmt/format.h>

EPOLL_WEB_BEGIN_NAMESPACE

inline void client(std::string_view addr, std::string_view port)
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

    // 初始化 ncurses
    initscr();
    noecho();
    curs_set(0);

    std::atomic<bool> quit_flag{false};
    std::jthread t_getch([fd, &quit_flag]
        {
            while (1)
            {
                int ch = getch();
                if (ch == 'q')
                {
                    quit_flag = true;
                    return;
                }

                if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd')
                {
                    char tmp[2];
                    tmp[0] = ch;
                    tmp[1] = '\0';
                    int error_check = send(fd, tmp, 2, 0);
                    if (error_check == -1)
                    {
                        break;
                    }
                }
            } });
    t_getch.detach();

    char buf[65536];
    ssize_t len{};
    try
    {
        while (!quit_flag)
        {
            len = recv(fd, buf, sizeof(buf), 0);
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
    }
    catch (std::exception const& e)
    {
        fmt::print(stderr, "len = {}, msg = {}\n", len, buf);
        spdlog::error("Exception: {}", e.what());
    }

    close(fd);
    endwin();

    fmt::print(stderr, "len = {}, msg = {}\n", len, buf);
}

EPOLL_WEB_END_NAMESPACE
