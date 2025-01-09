#include <epoll_web/common/log.hpp>
#include <epoll_web/network/server.hpp>
#include <epoll_web/common/string_convert.hpp>

#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif

#include <csignal>

namespace
{

void signal_handler([[maybe_unused]] int signal)
{
    if (signal == SIGINT)
    {
        spdlog::info("ctrl + c pressed");
    }
    else if (signal == SIGTERM)
    {
        spdlog::info("Process is killed");
    }
    exit(1);
}

} // namespace

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

    epoll_web::set_default_log({.level = spdlog::level::trace, .log_name = "epoll_web", .with_time = true});

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (argc != 2)
    {
        spdlog::error("Usage: {} <port>", argv[0]);
        exit(1);
    }

    std::optional<uint16_t> port = epoll_web::from_string<uint16_t>(argv[1]);
    if (!port.has_value()) [[unlikely]]
    {
        spdlog::error("Invalid port format: {}", argv[1]);
        exit(1);
    }

    epoll_web::server(*port);
}
