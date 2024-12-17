#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/check_error.hpp>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <boost/endian.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

namespace endian = boost::endian;

struct Socket
{
    Socket(Socket&& that) noexcept = default;

    Socket()
    {
        m_fd = socket(AF_INET6, SOCK_STREAM, 0);
        check_error("Socket()", m_fd);
        int opt = 1;
        check_error("Socket()", setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)));
    }

    explicit Socket(int fd) :
        m_fd(fd)
    {
        check_error("Socket(int fd)", fd);
    }

    void bind(uint16_t port) const
    {
        sockaddr_in6 addr = {.sin6_family = AF_INET6, .sin6_port = endian::native_to_big(port), .sin6_flowinfo = {}, .sin6_addr = in6addr_any, .sin6_scope_id = {}};
        check_error("Socket::bind(uint16_t)", ::bind(m_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)));
    }

    void listen() const
    {
        check_error("Socket::listen()", ::listen(m_fd, SOMAXCONN));
    }

    int accept() const
    {
        int client_fd = ::accept(m_fd, nullptr, nullptr);
        check_error("Socket::accept()", client_fd);
        return client_fd;
    }

    int get_fd() const noexcept
    {
        return m_fd;
    }

    ~Socket()
    {
        if (m_fd != -1)
        {
            close(m_fd);
        }
    }

private:
    int m_fd;
};

EPOLL_WEB_END_NAMESPACE