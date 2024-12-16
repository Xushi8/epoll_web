#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/log.hpp>
#include <string_view>

EPOLL_WEB_BEGIN_NAMESPACE

inline void check_error(std::string_view msg, int res)
{
    if (res < 0) [[unlikely]]
    {
        spdlog::warn("{}: {}", msg, strerror(errno));
        throw;
    }
}

inline void check_error(std::string_view msg, void* ptr)
{
    if (ptr == nullptr) [[unlikely]]
    {
        spdlog::warn("{}: {}", msg, strerror(errno));
        throw;
    }
}

EPOLL_WEB_END_NAMESPACE