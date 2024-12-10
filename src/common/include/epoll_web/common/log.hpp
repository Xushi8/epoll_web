#pragma once

#include <epoll_web/common/common.hpp>
#include <string>
#include <spdlog/spdlog.h>

EPOLL_WEB_BEGIN_NAMESPACE

struct log_options
{
    spdlog::level::level_enum level = spdlog::level::info;
    std::string log_name{"/dev/stdout"};  // NOLINT(readability-redundant-member-init)
    std::string file_path{}; // NOLINT(readability-redundant-member-init)
    bool with_time = false;
};

EPOLL_WEB_EXPORT
void set_default_log(const log_options& opts);

EPOLL_WEB_END_NAMESPACE
