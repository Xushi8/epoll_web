#include <epoll_web/network/server1.hpp>
#include <epoll_web/network/server2.hpp>

#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif

int main(void)
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

    // server1();
    epoll_web::server2();
}
