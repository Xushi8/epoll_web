#include <epoll_web/common/get_hardware_concurrency.h>
#include <unistd.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/sysinfo.h>
#endif

int get_hardware_concurrency()
{
#ifdef _WIN32
    // Windows: 使用 GetSystemInfo 获取处理器核心数
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
    // Linux/Unix/macOS: 使用 sysctl 或 sysinfo 获取处理器核心数
    int num_cpus = 0;

#ifdef __linux__
    // Linux 系统：通过 sysctl 获取 CPU 数量
    num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(__APPLE__)
    // macOS: 使用 sysctl 获取 CPU 数量
    size_t size = sizeof(num_cpus);
    sysctlbyname("hw.ncpu", &num_cpus, &size, NULL, 0);
#else
    // 其他类 Unix 系统使用 sysinfo
    struct sysinfo info;
    if (sysinfo(&info) == 0)
    {
        num_cpus = info.procs; // 进程数等于 CPU 核心数（但不是标准化的做法）
    }
#endif
    return num_cpus;
#else
    // 默认返回值
    return 1;
#endif
}
