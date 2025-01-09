#include <ncurses.h>
#include <epoll_web/game/game.hpp>
#include <thread>
#include <fmt/format.h>
#include <fmt/ranges.h>

int main()
{
    using namespace std::chrono_literals;
    auto head1 = epoll_web::Game::get_instance().get_unique_head();
    auto head2 = epoll_web::Game::get_instance().get_unique_head();
    epoll_web::Snack snack1{'*', 423, head1};
    epoll_web::Snack snack2{'#', 231, head2};

    epoll_web::Game::get_instance().add_snack(snack1);
    epoll_web::Game::get_instance().add_snack(snack2);

    fmt::print(stderr, "{} {}\n", snack1.get_head(), snack2.get_head());

    std::this_thread::sleep_for(0.5s);

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
