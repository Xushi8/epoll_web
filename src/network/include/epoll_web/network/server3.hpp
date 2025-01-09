#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/check_error.hpp>
#include <epoll_web/network/Socket.hpp>
#include <epoll_web/network/Epoll.hpp>
#include <sys/socket.h>

EPOLL_WEB_BEGIN_NAMESPACE

template <size_t N>
inline void handle_recv(int fd, std::array<char, N> const& buf, ssize_t n)
{
    (void)fd;
    std::string_view sv{buf.data(), static_cast<size_t>(n)};
}

inline void server3(uint16_t port)
{
    Socket sock;
    sock.bind(port);
    sock.listen();

    Epoll ep;
    ep.add_fd(sock.get_fd(), EPOLLIN | EPOLLET);

    while (1)
    {
        auto events = ep.poll();
        size_t n = events.size();
        for (size_t i = 0; i < n; i++)
        {
            int curfd = events[i].data.fd;
            try
            {
                if (curfd == sock.get_fd()) // accept
                {
                    while (1)
                    {
                        int client_fd = sock.accept();
                        if (client_fd == -1)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) [[likely]] // 没有数据了
                            {
                                break;
                            }
                            else
                            {
                                throw std::runtime_error(fmt::format("accept: {}", strerror(errno)));
                            }
                        }
                        check_error("setnonblocking", setnonblocking(client_fd));
                        ep.add_fd(client_fd, EPOLLIN | EPOLLET);
                    }
                }
                else // recv
                {
                    std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

                    while (1)
                    {
                        ssize_t len = recv(curfd, buf.data(), buf.size() - 1, 0);

                        if (len > 0)
                        {
                            // spdlog::trace("客户端说: {}", std::string_view(buf.data(), len));
                            // check_error("send", send(curfd, buf.data(), len, 0));
                            handle_recv(curfd, buf, len);
                        }
                        else if (len == 0)
                        {
                            spdlog::info("客户端已经断开连接");
                            ep.delete_fd(curfd);
                            close(curfd);
                            break;
                        }
                        else
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) [[likely]] // 没有数据了
                            {
                                break;
                            }
                            else if (errno == ECONNRESET)
                            {
                                spdlog::info("客户端已经断开连接");
                                ep.delete_fd(curfd);
                                close(curfd);
                                break;
                            }
                            else [[unlikely]]
                            {
                                throw std::runtime_error(fmt::format("recv: {}", strerror(errno)));
                            }
                        }
                    }
                }
            }
            catch (std::exception const& e)
            {
                spdlog::warn("Exception: {}", e.what());
                ep.delete_fd(curfd);
                close(curfd);
            }
        }
    }
}

EPOLL_WEB_END_NAMESPACE
