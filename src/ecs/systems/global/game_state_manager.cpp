/**
 * @file game_state_manager.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏状态系统
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/global/game_state_manager.hpp"
#include "base/assert_msg.hpp"
#include "ecs/components/global/game_state_components.hpp"
#include <algorithm>
#include <memory>
#include <ranges>
#include <spdlog/spdlog.h>

namespace tnrw::ecs {

    game_state_manager::game_state_manager() noexcept {
        push_state(game_state_enum::main_menu);
        m_dispatcher.sink<game_state_push_event>().connect<&game_state_manager::on_push_state>(this);
        m_dispatcher.sink<game_state_pop_event>().connect<&game_state_manager::on_pop_state>(this);
    }
    void game_state_manager::push_state(game_state_enum state) noexcept {
        if (!m_states.empty()) {
            m_states.back().back()->on_pause();
        }
        std::unique_ptr<game_state_base> state_will_push = make_corresponding_state(state, m_dispatcher);
        if (state_will_push->should_block_passing_down()) {
            m_states.emplace_back();
        }
        m_states.back().push_back(std::move(state_will_push));
    }
    void game_state_manager::pop_state() noexcept {
        ASSERT_MSG(!m_states.empty(), "用户错误地尝试对空的栈执行弹出操作");
        m_states.back().pop_back();
        if (m_states.back().empty()) {
            // 删除无效的状态
            m_states.pop_back();
        }
        if (!m_states.empty()) {
            m_states.back().back()->on_resume();
        }
    }
    void game_state_manager::reset() noexcept {
        m_states.clear();
        m_dispatcher.clear();
        push_state(game_state_enum::main_menu);
        m_dispatcher.sink<game_state_push_event>().connect<&game_state_manager::on_push_state>(this);
        m_dispatcher.sink<game_state_pop_event>().connect<&game_state_manager::on_pop_state>(this);
    }
    bool game_state_manager::handle_event(const sf::Event &event) noexcept {
        return std::ranges::any_of(
            m_states.back() | std::views::reverse,
            [&](std::unique_ptr<game_state_base> &state) { return state->handle_event(event); });
    }
    void game_state_manager::update(milliseconds_f delta_time) noexcept {
        // 顺序应该不重要
        for (auto &state : m_states.back()) { state->update(delta_time); }
        m_dispatcher.update();
    }
    void game_state_manager::draw(sf::RenderTarget &render) noexcept {
        // 后面覆盖前面，所以前面先绘制
        for (auto &state : m_states.back()) { state->draw(render); }
    }
    void game_state_manager::on_push_state(game_state_push_event event) noexcept {
        push_state(event.state);
    }
    void game_state_manager::on_pop_state([[maybe_unused]] game_state_pop_event event) noexcept {
        pop_state();
    }

} // namespace tnrw::ecs
