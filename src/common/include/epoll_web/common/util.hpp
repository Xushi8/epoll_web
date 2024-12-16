#pragma once

#include <epoll_web/common/common.hpp>
#include <fcntl.h>

EPOLL_WEB_BEGIN_NAMESPACE

inline void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

EPOLL_WEB_END_NAMESPACE