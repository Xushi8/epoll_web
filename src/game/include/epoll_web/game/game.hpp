#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/util.hpp>
#include <epoll_web/game/snake.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <random>
#include <nlohmann/json.hpp>

EPOLL_WEB_BEGIN_NAMESPACE

// singleton
struct Game
{
    Game(Game&&) noexcept = delete;

    static Game& get_instance();

    std::pair<uint32_t, uint32_t> get_unique_head()
    {
        thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<uint32_t> uni_x(1, max_x - 1), uni_y(1, max_y - 1);
        for (size_t i = 0; i < 20; i++)
        {
            uint32_t x = uni_x(rng);
            uint32_t y = uni_y(rng);
            if (!m_visited.contains({x, y}))
            {
                m_visited.emplace(x, y);
                return {x, y};
            }
        }

        // 保底
        for (uint32_t i = 1; i < max_x - 1; i++)
        {
            for (uint32_t j = 1; j < max_y - 1; j++)
            {
                uint32_t x = i, y = j;
                if (!m_visited.contains({x, y}))
                {
                    m_visited.emplace(x, y);
                    return {x, y};
                }
            }
        }

        unreachable();
    }

    void add_snake(Snake snake)
    {
        m_snakes.emplace_back(std::move(snake));
    }

    void check_valid()
    {
        for (auto& snake : m_snakes)
        {
            auto [x, y] = snake.get_head();
            if (m_visited.contains(snake.get_head()) || (x == 0 || x == max_x || y == 0 || y == max_y))
            {
                auto const& body = snake.get_body();
                for (auto const& location : body)
                {
                    mvaddch(location.second, location.first, ' ');
                    m_visited.erase(location);
                }

                snake.reset(get_unique_head());
                m_visited.emplace(snake.get_head());
            }
            else
            {
                m_visited.emplace(snake.get_head());
            }
        }
    }

    void update()
    {
        std::ranges::for_each(m_snakes, [](Snake& snake)
            { snake.update(); });

        check_valid();
    }

    void print() const
    {
        std::ranges::for_each(m_snakes, [](Snake const& snake)
            { snake.print(); });
        refresh();
    }

    friend void to_json(nlohmann::json& j, const Game& game)
    {
        // 将 m_visited 转换为 JSON 数组
        j["visited"] = nlohmann::json::array();
        for (const auto& pair : game.m_visited)
        {
            j["visited"].push_back(pair);
        }

        // 将 m_snakes 转换为 JSON 数组
        j["snakes"] = nlohmann::json::array();
        for (const auto& snake : game.m_snakes)
        {
            j["snakes"].push_back(snake);
        }

        j["max_x"] = epoll_web::Game::max_x;
        j["max_y"] = epoll_web::Game::max_y;
    }

    friend void from_json(const nlohmann::json& j, Game& game)
    {
        // 从 JSON 数组读取 m_visited
        game.m_visited.clear();
        for (const auto& item : j.at("visited"))
        {
            game.m_visited.insert(item.get<std::pair<uint32_t, uint32_t>>());
        }

        // 从 JSON 数组读取 m_snakes
        game.m_snakes.clear();
        for (const auto& item : j.at("snakes"))
        {
            Snake snake;
            item.get_to(snake);
            game.m_snakes.emplace_back(std::move(snake));
        }
    }

private:
    Game() = default;

    boost::unordered::unordered_flat_set<std::pair<uint32_t, uint32_t>> m_visited;
    std::vector<Snake> m_snakes;

    static constexpr uint32_t max_x = 30;
    static constexpr uint32_t max_y = 30;
};

EPOLL_WEB_END_NAMESPACE