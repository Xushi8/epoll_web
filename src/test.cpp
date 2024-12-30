#include <fmt/core.h>
#include <signal.h>
#include <unistd.h>
#include <atomic>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

namespace
{

std::atomic<int> EndFlag(0);

pid_t pid1, pid2;

void IntDelete([[maybe_unused]] int sig)
{
    // 给子进程发送信号
    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR2);
    EndFlag = 1;
}

void Int1([[maybe_unused]] int sig)
{
    fmt::print("child process 1 is killed by parent!\n");
    exit(0);
}

void Int2([[maybe_unused]] int sig)
{
    fmt::print("child process 2 is killed by parent!\n");
    exit(0);
}

} // namespace

int main()
{
    int exitcode;

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    while ((pid1 = fork()) == -1) {}

    if (pid1 == 0)
    {
        signal(SIGUSR1, Int1);
        signal(SIGINT, SIG_IGN);
        pause();
        exit(0);
    }
    else
    {
        while ((pid2 = fork()) == -1) {}

        if (pid2 == 0)
        {
            signal(SIGUSR2, Int2);
            signal(SIGINT, SIG_IGN);
            pause();
            exit(0);
        }
        else
        {
            signal(SIGINT, IntDelete);
            waitpid(pid1, &exitcode, 0);
            waitpid(pid2, &exitcode, 0);

            fmt::print("parent process is killed\n");
            exit(0);
        }
    }
}
