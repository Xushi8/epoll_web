#include <epoll_web/network/server1.h>
#include <epoll_web/common/get_hardware_concurrency.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <threads.h>

struct hold_args
{
    int epfd, lfd;
};

static int hold(void* args)
{
    struct hold_args arg = *(struct hold_args*)args;
    int epfd = arg.epfd;
    int lfd = arg.lfd;

    const size_t maxx = 50;
    struct epoll_event evs[maxx];

    while (1)
    {
        int cnt = epoll_wait(epfd, evs, maxx, -1);
        for (int i = 0; i < cnt; i++)
        {
            int curfd = evs[i].data.fd;
            if (curfd == lfd)
            {
                int client_fd = accept(curfd, nullptr, nullptr);
                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                if (ret == -1)
                {
                    perror("epoll_ctl accept");
                    exit(1);
                }
            }
            else
            {
                char buf[1024];
                int len = recv(curfd, buf, sizeof(buf), 0);
                if (len > 0)
                {
                    printf("客户端 say: %s\n", buf);
                    send(curfd, buf, len, 0);
                }
                else if (len == 0)
                {
                    printf("客户端已经断开连接\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, nullptr);
                    close(curfd);
                }
                else
                {
                    perror("recv");
                    exit(1);
                }
            }
        }
    }

    return 0;
}

void server3(void)
{
    int lfd = socket(AF_INET6, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in6 addr = {};
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(10000);
    addr.sin6_addr = in6addr_any;
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(lfd, SOMAXCONN);
    if (ret == -1)
    {
        perror("listen");
        exit(1);
    }

    // epoll
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl add");
        exit(1);
    }

    struct hold_args args;
    args.epfd = epfd;
    args.lfd = lfd;

    const int thread_num = get_hardware_concurrency();
    thrd_t threads[thread_num];
    for (int i = 0; i < thread_num; i++)
    {
        ret = thrd_create(&threads[i], hold, &args);
        if (ret != thrd_success)
        {
            fprintf(stderr, "Failed to create thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < thread_num; i++)
    {
        ret = thrd_join(threads[i], nullptr);
        if (ret != thrd_success)
        {
            fprintf(stderr, "Failed to create thread\n");
            exit(1);
        }
    }
}
