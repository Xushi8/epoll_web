#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/log.hpp>
#include <string_view>

EPOLL_WEB_BEGIN_NAMESPACE

inline void check_error(std::string_view msg, int res)
{
    if (res < 0) [[unlikely]]
    {
        throw std::runtime_error(fmt::format("{}: {}", msg, strerror(errno)));
    }
}

inline void check_error(std::string_view msg, void* ptr)
{
    if (ptr == nullptr) [[unlikely]]
    {
        throw std::runtime_error(fmt::format("{}: {}", msg, strerror(errno)));
    }
}

EPOLL_WEB_END_NAMESPACE