#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/check_error.hpp>
#include <epoll_web/network/Socket.hpp>
#include <epoll_web/network/Epoll.hpp>
#include <epoll_web/game/game.hpp>
#include <sys/socket.h>
#include <nlohmann/json.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

inline void server3(uint16_t port)
{
    Socket sock;
    sock.bind(port);
    sock.listen();

    Epoll ep;
    ep.add_fd(sock.get_fd(), EPOLLIN | EPOLLET);

    boost::unordered::unordered_flat_set<int> all_fd;

    std::jthread t_recv(
        [&]
        {
            while (1)
            {
                auto events = ep.poll();
                size_t n = events.size();
                for (size_t i = 0; i < n; i++)
                {
                    int curfd = events[i].data.fd;
                    try
                    {
                        if (curfd == sock.get_fd()) // accept
                        {
                            while (1)
                            {
                                int client_fd = sock.accept();
                                if (client_fd == -1)
                                {
                                    if (errno == EAGAIN || errno == EWOULDBLOCK) [[likely]] // 没有数据了
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        throw std::runtime_error(fmt::format("accept: {}", strerror(errno)));
                                    }
                                }
                                check_error("setnonblocking", setnonblocking(client_fd));
                                ep.add_fd(client_fd, EPOLLIN | EPOLLET);
                                all_fd.emplace(client_fd);
                            }
                        }
                        else // recv
                        {
                            std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

                            while (1)
                            {
                                ssize_t len = recv(curfd, buf.data(), buf.size() - 1, 0);

                                if (len > 0)
                                {
                                    spdlog::trace("客户端说: {}", std::string_view(buf.data(), len));

                                    std::string_view sv(buf.data(), len);
                                    using namespace std::string_view_literals;
                                    Snake& snake = Game::get_instance().get_snake(curfd);
                                    if (sv == "w"sv)
                                    {
                                        snake.change_dir(Direction::UP);
                                    }
                                    else if (sv == "s"sv)
                                    {
                                        snake.change_dir(Direction::DOWN);
                                    }
                                    else if (sv == "a"sv)
                                    {
                                        snake.change_dir(Direction::LEFT);
                                    }
                                    else // d
                                    {
                                        snake.change_dir(Direction::RIGHT);
                                    }
                                }
                                else if (len == 0)
                                {
                                    spdlog::info("客户端已经断开连接");
                                    Game::get_instance().delete_snake(curfd);
                                    ep.delete_fd(curfd);
                                    close(curfd);
                                    all_fd.erase(curfd);
                                    break;
                                }
                                else
                                {
                                    if (errno == EAGAIN || errno == EWOULDBLOCK) [[likely]] // 没有数据了
                                    {
                                        break;
                                    }
                                    else if (errno == ECONNRESET)
                                    {
                                        spdlog::info("客户端已经断开连接");
                                        Game::get_instance().delete_snake(curfd);
                                        ep.delete_fd(curfd);
                                        close(curfd);
                                        all_fd.erase(curfd);
                                        break;
                                    }
                                    else [[unlikely]]
                                    {
                                        throw std::runtime_error(fmt::format("recv: {}", strerror(errno)));
                                    }
                                }
                            }
                        }
                    }
                    catch (std::exception const& e)
                    {
                        spdlog::warn("Exception: {}", e.what());
                        Game::get_instance().delete_snake(curfd);
                        ep.delete_fd(curfd);
                        close(curfd);
                        all_fd.erase(curfd);
                    }
                }
            }
        });

    std::jthread j_send(
        [&]
        {
            while (1)
            {
                // 序列化
                nlohmann::json j = epoll_web::Game::get_instance();
                std::string s = j.dump();

                for (int fd : all_fd)
                {
                    send(fd, s.c_str(), s.size(), 0);
                }

                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1s);
            }
        });
}

EPOLL_WEB_END_NAMESPACE
