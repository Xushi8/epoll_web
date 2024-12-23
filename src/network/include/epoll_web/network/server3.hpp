#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/check_error.hpp>
#include <epoll_web/network/Socket.hpp>
#include <epoll_web/network/Epoll.hpp>
#include <sys/socket.h>

EPOLL_WEB_BEGIN_NAMESPACE

inline void server3(uint16_t port)
{
    Socket sock;
    sock.bind(port);
    sock.listen();

    Epoll ep;
    ep.add_fd(sock.get_fd(), EPOLLIN);

    while (1)
    {
        auto events = ep.poll();
        size_t n = events.size();
        for (size_t i = 0; i < n; i++)
        {
            int curfd = events[i].data.fd;
            if (curfd == sock.get_fd()) // accept
            {
                int client_fd = sock.accept();
                ep.add_fd(client_fd, EPOLLIN);
            }
            else // recv
            {
                std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

                int len = recv(curfd, buf.data(), buf.size() - 1, 0);

                if (len > 0)
                {
                    spdlog::trace("客户端说: {}", std::string_view(buf.data(), len));
                    check_error("send", send(curfd, buf.data(), len, 0));
                }
                else if (len == 0)
                {
                    spdlog::info("客户端已经断开连接");
                    ep.delete_fd(curfd);
                    close(curfd);
                }
                else
                {
                    if (errno == ECONNRESET)
                    {
                        spdlog::info("客户端已经断开连接");
                        ep.delete_fd(curfd);
                        close(curfd);
                    }
                    else
                    {
                        throw std::runtime_error(fmt::format("recv: {}", strerror(errno)));
                    }
                }
            }
        }
    }
}

EPOLL_WEB_END_NAMESPACE
