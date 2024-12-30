#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <fmt/core.h>
#include <cstdlib>
#include <climits>
#include <stdexcept>
#include <string_view>

namespace
{

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

constexpr std::string_view BAD_REQUEST = "HTTP/1.1 400 Bad Request\r\n\r\n";
constexpr std::string_view NOT_FOUND = "HTTP/1.1 404 File Not Found\r\n\r\n";
constexpr std::string_view OK_HEADER = "HTTP/1.1 200 OK\r\n";

constexpr std::string_view WEB_ROOT = "/tmp/web";

void check_error(std::string_view msg, int res)
{
    if (res < 0) [[unlikely]]
    {
        throw std::runtime_error(fmt::format("{}: {}", msg, strerror(errno)));
    }
}

void set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    check_error("fcntl F_GETFL", flags);
    check_error("fcntl F_SETFL", fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

void do_400(int fd, int epoll_fd)
{
    send(fd, BAD_REQUEST.data(), BAD_REQUEST.size(), 0);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
}

void do_404(int fd, int epoll_fd)
{
    send(fd, NOT_FOUND.data(), NOT_FOUND.size(), 0);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
}

void handle_request(int client_fd, int epoll_fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t len = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (len <= 0)
    {
        if (len == 0)
        {
            fmt::print("Client disconnected.\n");
        }
        else
        {
            fmt::print("Error receiving request: {}\n", strerror(errno));
        }
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
        close(client_fd);
        return;
    }

    buffer[len] = '\0';
    // fmt::print("Received request: {}\n", buffer);

    if (strncmp(buffer, "GET ", 4) != 0)
    {
        do_400(client_fd, epoll_fd);
        return;
    }

    std::stringstream ss(buffer);
    std::string method, path, version;
    ss >> method >> path >> version;

    if (path.find("..") != std::string::npos)
    {
        do_400(client_fd, epoll_fd);
        return;
    }

    std::ifstream file(WEB_ROOT.data() + path);
    if (file.good())
    {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string header = std::string(OK_HEADER) + "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n";
        send(client_fd, header.c_str(), header.size(), 0);
        send(client_fd, content.c_str(), content.size(), 0);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
        close(client_fd);
    }
    else
    {
        do_404(client_fd, epoll_fd);
    }
}

} // namespace

int main()
{
    try
    {
        int server_fd = socket(AF_INET6, SOCK_STREAM, 0);
        check_error("socket", server_fd);
        int opt = 1;
        check_error("setsockopt", setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)));

        set_non_blocking(server_fd);

        sockaddr_in6 server_addr = {};
        server_addr.sin6_family = AF_INET6;
        server_addr.sin6_addr = in6addr_any;
        server_addr.sin6_port = htons(8888);

        int res = bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr));
        check_error("bind", res);

        res = listen(server_fd, 10);
        check_error("listen", res);

        int epoll_fd = epoll_create1(0);
        check_error("epoll_create1", epoll_fd);

        struct epoll_event ev = {};
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = server_fd;
        res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
        check_error("epoll_ctl", res);

        struct epoll_event events[MAX_EVENTS];
        while (1)
        {
            int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            check_error("epoll_wait", n);

            for (int i = 0; i < n; i++)
            {
                if (events[i].data.fd == server_fd) // new connection
                {
                    while (1)
                    {
                        sockaddr_in6 client_addr = {};
                        socklen_t client_len = sizeof(client_addr);
                        int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
                        // check_error("accept", client_fd);
                        if (client_fd == -1)
                        {
                            if (errno == EAGAIN || EWOULDBLOCK)
                            {
                                break;
                            }
                            else
                            {
                                check_error("accept", -1);
                            }
                        }

                        set_non_blocking(client_fd);

                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = client_fd;
                        res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                        check_error("epoll_ctl", res);
                    }
                }
                else
                {
                    handle_request(events[i].data.fd, epoll_fd);
                }
            }
        }

        close(server_fd);
        close(epoll_fd);
    }
    catch (std::exception const& e)
    {
        fmt::print(stderr, "Exception: {}\n", e.what());
        exit(1);
    }

    return 0;
}
