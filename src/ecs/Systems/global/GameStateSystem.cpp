/**
 * @file GameStateSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏状态系统
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/Systems/global/GameStateSystem.hpp"
#include "base/assert_msg.hpp"
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

namespace tnrw::ecs {

    bool GameStateSystem::createGameState(entt::registry &registry) noexcept {
        if (registry.ctx().contains<GameState>()) {
            return false;
        }
        spdlog::info("游戏状态组件创建");
        registry.ctx().emplace<GameState>();
        return true;
    }

    void GameStateSystem::pushTopState(entt::registry &registry, const GameState::State state) noexcept {
        assert_msg(registry.ctx().contains<GameState>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        auto &val = registry.ctx().get<GameState>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::GameState::toString(state));
        val.states.push_back(state);
    }
    void GameStateSystem::popTopState(entt::registry &registry) noexcept {
        assert_msg(registry.ctx().contains<GameState>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        auto &val = registry.ctx().get<GameState>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::GameState::toString(val.states.back()));
        val.states.pop_back();
    }
    [[nodiscard]] GameState::State
    GameStateSystem::getTopState(const entt::registry &registry) noexcept {
        assert_msg(registry.ctx().contains<GameState>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        const auto &val = registry.ctx().get<GameState>();
        return val.states.back();
    }
    [[nodiscard]] const std::vector<GameState::State> &
    GameStateSystem::getStates(const entt::registry &registry) noexcept {
        assert_msg(registry.ctx().contains<GameState>(),
                   "参数 `registry` 没有 `tnrw::ecs::GameState` 组件");
        return registry.ctx().get<GameState>().states;
    }

    [[nodiscard]] bool GameStateSystem::tryPushTopState(entt::registry        &registry,
                                                        const GameState::State state) noexcept {
        if (!registry.ctx().contains<GameState>()) {
            return false;
        }
        auto &val = registry.ctx().get<GameState>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::GameState::toString(state));
        val.states.push_back(state);
        return true;
    }
    [[nodiscard]] bool GameStateSystem::tryPopTopState(entt::registry &registry) noexcept {
        if (!registry.ctx().contains<GameState>()) {
            return false;
        }
        auto &val = registry.ctx().get<GameState>();
        spdlog::info("添加游戏状态：{}", tnrw::ecs::GameState::toString(val.states.back()));
        val.states.pop_back();
        return true;
    }
    [[nodiscard]] std::optional<GameState::State>
    GameStateSystem::tryGetTopState(const entt::registry &registry) noexcept {
        if (!registry.ctx().contains<GameState>()) {
            return std::nullopt;
        }
        const auto &val = registry.ctx().get<GameState>();
        return val.states.back();
    }
    [[nodiscard]] const std::vector<GameState::State> *
    GameStateSystem::tryGetStates(const entt::registry &registry) noexcept {
        if (!registry.ctx().contains<GameState>()) {
            return nullptr;
        }
        return &registry.ctx().get<GameState>().states;
    }

} // namespace tnrw::ecs
