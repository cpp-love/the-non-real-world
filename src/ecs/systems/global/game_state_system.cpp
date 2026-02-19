/**
 * @file game_state_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏状态系统
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/global/game_state_system.hpp"
#include "base/assert_msg.hpp"
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

namespace tnrw::ecs {

    bool game_state_system::create_game_state(entt::registry &registry) noexcept {
        if (registry.ctx().contains<game_state>()) {
            return false;
        }
        spdlog::info("游戏状态组件创建");
        registry.ctx().emplace<game_state>();
        return true;
    }

    void game_state_system::push_top_state(entt::registry         &registry,
                                           const game_state::state state) noexcept {
        assert_msg(registry.ctx().contains<game_state>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        auto &val = registry.ctx().get<game_state>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::game_state::to_string(state));
        val.m_states.push_back(state);
    }
    void game_state_system::pop_top_state(entt::registry &registry) noexcept {
        assert_msg(registry.ctx().contains<game_state>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        auto &val = registry.ctx().get<game_state>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::game_state::to_string(val.m_states.back()));
        val.m_states.pop_back();
    }
    [[nodiscard]] game_state::state
    game_state_system::get_top_state(const entt::registry &registry) noexcept {
        assert_msg(registry.ctx().contains<game_state>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        const auto &val = registry.ctx().get<game_state>();
        return val.m_states.back();
    }
    [[nodiscard]] const std::vector<game_state::state> &
    game_state_system::get_states(const entt::registry &registry) noexcept {
        assert_msg(registry.ctx().contains<game_state>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        return registry.ctx().get<game_state>().m_states;
    }

    [[nodiscard]] bool game_state_system::try_push_top_state(entt::registry         &registry,
                                                             const game_state::state state) noexcept {
        if (!registry.ctx().contains<game_state>()) {
            return false;
        }
        auto &val = registry.ctx().get<game_state>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::game_state::to_string(state));
        val.m_states.push_back(state);
        return true;
    }
    [[nodiscard]] bool game_state_system::try_pop_top_state(entt::registry &registry) noexcept {
        if (!registry.ctx().contains<game_state>()) {
            return false;
        }
        auto &val = registry.ctx().get<game_state>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::game_state::to_string(val.m_states.back()));
        val.m_states.pop_back();
        return true;
    }
    [[nodiscard]] std::optional<game_state::state>
    game_state_system::try_get_top_state(const entt::registry &registry) noexcept {
        if (!registry.ctx().contains<game_state>()) {
            return std::nullopt;
        }
        const auto &val = registry.ctx().get<game_state>();
        return val.m_states.back();
    }
    [[nodiscard]] const std::vector<game_state::state> *
    game_state_system::try_get_states(const entt::registry &registry) noexcept {
        if (!registry.ctx().contains<game_state>()) {
            return nullptr;
        }
        return &registry.ctx().get<game_state>().m_states;
    }

} // namespace tnrw::ecs
