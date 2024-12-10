#include <epoll_web/common/log.hpp>

int main()
{
    epoll_web::set_default_log({.log_name = "epoll_web", .file_path = "/dev/stdout", .with_time = true});

    spdlog::info("test log");
}
