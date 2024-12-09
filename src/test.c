#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
int main()
{
    int cnt = 1;

    int pid;
    for (; (pid = fork() && pid != 0);)
    {
        printf("%d\n", cnt);
        if (pid == -1)
        {
            break;
        }
        cnt++;
    }
}
