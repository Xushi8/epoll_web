#pragma once

#include <epoll_web/common/common.hpp>
#include <cstdint>
#include <deque>
#include <ncurses.h>

#include <nlohmann/json.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

enum class Direction : uint8_t
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

// clang-format off
// Direction 序列化和反序列化
NLOHMANN_JSON_SERIALIZE_ENUM(Direction, {
    {Direction::UP, "UP"},
    {Direction::DOWN, "DOWN"},
    {Direction::LEFT, "LEFT"},
    {Direction::RIGHT, "RIGHT"}
})
// clang-format on

constexpr int dx[4] = {0, 0, -1, 1};
constexpr int dy[4] = {-1, 1, 0, 0};

struct Snake
{
    Snake(char character, int fd, std::pair<uint32_t, uint32_t> body_head) :
        m_character(character), m_fd(fd), m_body(1, body_head)
    {
    }

    Snake() = default;

    Snake(Snake&&) noexcept = default;

    Snake& operator=(Snake&&) noexcept = default;

    friend constexpr auto operator<=>(Snake const& lhs, Snake const& rhs) noexcept
    {
        return lhs.get_fd() <=> rhs.get_fd();
    }

    void change_dir(Direction dir)
    {
        if (m_dir != m_pre_dir)
        {
            m_dir = m_pre_dir;
        }
        
        if (((m_dir == Direction::DOWN || m_dir == Direction::UP) && dir != Direction::DOWN && dir != Direction::UP)
            || ((m_dir == Direction::LEFT || m_dir == Direction::RIGHT) && dir != Direction::LEFT && dir != Direction::RIGHT))
        {
            m_dir = dir;
        }
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

        m_pre_dir = m_dir;
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

    // Snake 的序列化
    friend void to_json(nlohmann::json& j, const Snake& snake)
    {
        j = nlohmann::json{
            {"direction", snake.m_dir},
            {"character", snake.m_character},
            {"fd", snake.m_fd},
            {"body", snake.m_body}};
    }

    // Snake 的反序列化
    friend void from_json(const nlohmann::json& j, Snake& snake)
    {
        j.at("direction").get_to(snake.m_dir);
        j.at("character").get_to(snake.m_character);
        j.at("fd").get_to(snake.m_fd);
        j.at("body").get_to(snake.m_body);
    }

private:
    Direction m_dir{Direction::DOWN};
    Direction m_pre_dir{Direction::DOWN};
    char m_character{};
    int m_fd{};
    std::deque<std::pair<uint32_t, uint32_t>> m_body;
};

EPOLL_WEB_END_NAMESPACE