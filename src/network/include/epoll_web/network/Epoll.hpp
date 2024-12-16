#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/check_error.hpp>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <array>
#include <boost/container/vector.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

namespace container = boost::container;

static constexpr size_t max_event = 1024;

struct Epoll
{
    Epoll(Epoll&& that) noexcept = default;

    Epoll() // NOLINT(cppcoreguidelines-pro-type-member-init)
    {
        m_epfd = epoll_create1(0);
        check_error("Epoll()", m_epfd);
    }

    void add_fd(int fd, uint32_t op) const
    {
        struct epoll_event ev{.events = op, .data{.fd = fd}};
        check_error("Epoll::add_fd(int, uint32_t)", epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev));
    }

    void delete_fd(int fd) const
    {
        check_error("Epoll::delete_fd(int)", epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr));
    }

    container::vector<epoll_event> poll(int timeout = -1)
    {
        int cnt = epoll_wait(m_epfd, m_events.data(), max_event, timeout);
        check_error("Epoll::poll(int)", cnt);
        container::vector<epoll_event> res(cnt, container::default_init);
        std::copy(m_events.begin(), m_events.begin() + cnt, res.begin());
        return res;
    }

    ~Epoll()
    {
        if (m_epfd != -1)
        {
            close(m_epfd);
        }
    }

private:
    int m_epfd;
    std::array<epoll_event, max_event> m_events;
};

EPOLL_WEB_END_NAMESPACE