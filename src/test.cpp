#include <ncurses.h>
#include <epoll_web/game/game.hpp>
#include <thread>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>

int main()
{
    using namespace std::chrono_literals;
    auto head1 = epoll_web::Game::get_instance().get_unique_head();
    auto head2 = epoll_web::Game::get_instance().get_unique_head();
    epoll_web::Snake snake1{'*', 423, head1};
    epoll_web::Snake snake2{'#', 231, head2};

    epoll_web::Game::get_instance().add_snake(std::move(snake1));
    epoll_web::Game::get_instance().add_snake(std::move(snake2));

    // 序列化
    nlohmann::json j = epoll_web::Game::get_instance();
    std::cout << j.dump() << std::endl;

    j.get_to(epoll_web::Game::get_instance());

    std::this_thread::sleep_for(5s);

    // 初始化 ncurses
    initscr();
    noecho();
    curs_set(0);

    for (size_t i = 0; i < 30; i++)
    {
        epoll_web::Game::get_instance().print();
        std::this_thread::sleep_for(0.3s);
        epoll_web::Game::get_instance().update();
    }

    // 等待按键退出
    getch();
    endwin();

    return 0;
}
