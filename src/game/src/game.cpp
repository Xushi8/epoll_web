#include <epoll_web/game/game.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

 Game& Game::get_instance()
{
    static Game instance;
    return instance;
}

EPOLL_WEB_END_NAMESPACE
