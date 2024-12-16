#include <epoll_web/network/client1.hpp>
#include <epoll_web/common/log.hpp>

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

    if (argc != 3)
    {
        spdlog::error("Usage: {} <ip/domain> <port>\n", argv[0]);
        exit(1);
    }

    epoll_web::client1(argv[1], argv[2]);
}
