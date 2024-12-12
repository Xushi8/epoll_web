#include <epoll_web/common/log.hpp>
#include <epoll_web/network/server1.hpp>
#include <epoll_web/network/server2.hpp>
#include <epoll_web/common/string_convert.hpp>

#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
    // -finput-charset=utf-8 -fexec-charset=utf-8
    setlocale(LC_ALL, ".UTF-8");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#elif defined __APPLE__
    setlocale(LC_ALL, "UTF-8");
#elif defined __linux__
    setlocale(LC_ALL, "C.UTF-8");
#endif

    epoll_web::set_default_log({.log_name = "epoll_web", .with_time = true});

    if (argc != 2)
    {
        fmt::print(stderr, "Usage: {} <port>\n", argv[0]);
        exit(1);
    }

    std::optional<uint16_t> port = epoll_web::from_string<uint16_t>(argv[1]);
    if (!port.has_value()) [[unlikely]]
    {
        fmt::print(stderr, "Invalid port\n");
        exit(1);
    }

    epoll_web::server1(*port);
    // epoll_web::server2(*port);
}
