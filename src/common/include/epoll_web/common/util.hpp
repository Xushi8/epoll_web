#pragma once

#include <epoll_web/common/common.hpp>
#include <fcntl.h>

EPOLL_WEB_BEGIN_NAMESPACE

inline int setnonblocking(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

[[noreturn]] inline void unreachable()
{
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}

EPOLL_WEB_END_NAMESPACE