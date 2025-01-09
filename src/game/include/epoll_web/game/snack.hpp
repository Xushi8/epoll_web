#pragma once

#include <epoll_web/common/common.hpp>
#include <cstdint>
#include <deque>
#include <ncurses.h>
#include <boost/intrusive/bs_set.hpp>
#include <boost/intrusive/bs_set_hook.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

enum class Direction : uint8_t
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

constexpr int dx[4] = {-1, 1, 0, 0};
constexpr int dy[4] = {0, 0, -1, 1};

struct Snack : public boost::intrusive::bs_set_base_hook<>
{
    Snack(char character, int fd, std::pair<uint32_t, uint32_t> body_head) :
        m_character(character), m_fd(fd), m_body(1, body_head)
    {
    }

    Snack(Snack&&) noexcept = default;

    Snack& operator=(Snack&&) noexcept = default;

    friend constexpr auto operator<=>(Snack const& lhs, Snack const& rhs) noexcept
    {
        return lhs.get_fd() <=> rhs.get_fd();
    }

    void change_dir(Direction dir)
    {
        m_dir = dir;
    }

    void print() const
    {
        for (auto [x, y] : m_body)
        {
            mvaddch(y, x, m_character);
        }
    }

    void update()
    {
        auto [x, y] = m_body.front();
        int tx = x + dx[static_cast<size_t>(m_dir)];
        int ty = y + dy[static_cast<size_t>(m_dir)];
        m_body.emplace_front(tx, ty);
    }

    void reset(std::pair<uint32_t, uint32_t> body_head)
    {
        m_dir = Direction::DOWN;
        m_body.clear();
        m_body.emplace_back(body_head);
    }

    constexpr int get_fd() const noexcept
    {
        return m_fd;
    }

    std::pair<uint32_t, uint32_t> get_head() const noexcept
    {
        return m_body.front();
    }

    constexpr std::deque<std::pair<uint32_t, uint32_t>>& get_body() noexcept
    {
        return m_body;
    }

    constexpr std::deque<std::pair<uint32_t, uint32_t>> const& get_body() const noexcept
    {
        return m_body;
    }

private:
    Direction m_dir{Direction::DOWN};
    char m_character;
    int m_fd;
    std::deque<std::pair<uint32_t, uint32_t>> m_body;
};

EPOLL_WEB_END_NAMESPACE