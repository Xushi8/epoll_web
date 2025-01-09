#pragma once

#include <epoll_web/common/common.hpp>
#include <epoll_web/common/util.hpp>
#include <epoll_web/game/snack.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/intrusive/bs_set.hpp>
#include <random>

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

    void add_snack(Snack& snack)
    {
        m_snacks.insert(snack);
    }

    void check_valid()
    {
        for (auto& snack : m_snacks)
        {
            auto [x, y] = snack.get_head();
            if (m_visited.contains(snack.get_head()) || (x == 0 || x == max_x || y == 0 || y == max_y))
            {
                auto const& body = snack.get_body();
                for (auto const& location : body)
                {
                    mvaddch(location.second, location.first, ' ');
                    m_visited.erase(location);
                }

                snack.reset(get_unique_head());
                m_visited.emplace(snack.get_head());
            }
            else
            {
                m_visited.emplace(snack.get_head());
            }
        }
    }

    void update()
    {
        std::ranges::for_each(m_snacks, [](Snack& snack)
            { snack.update(); });

        check_valid();
    }

    void print() const
    {
        std::ranges::for_each(m_snacks, [](Snack const& snack)
            { snack.print(); });
        refresh();
    }

private:
    Game() = default;

    boost::unordered::unordered_flat_set<std::pair<uint32_t, uint32_t>> m_visited;
    boost::intrusive::bs_set<Snack> m_snacks;

    static constexpr uint32_t max_x = 30;
    static constexpr uint32_t max_y = 30;
};

EPOLL_WEB_END_NAMESPACE