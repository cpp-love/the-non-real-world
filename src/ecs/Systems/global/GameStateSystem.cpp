/**
 * @file GameStateSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了一些状态系统
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/Systems/global/GameStateSystem.hpp"
#include "base/assert_msg.hpp"

namespace tnrw {

    namespace ecs {

        bool GameStateSystem::createGameState(entt::registry &registry) noexcept {
            if (registry.ctx().contains<GameState>())
                return false;
            registry.ctx().emplace<GameState>();
            return true;
        }

        void GameStateSystem::pushTopState(entt::registry        &registry,
                                           const GameState::State state) noexcept {
            assert_msg(
                registry.ctx().contains<GameState>(),
                "参数 `registry` 没有 `tnrw::ecs::GameState` 组件，请将本消息反馈到项目的 Issue 中");
            auto val = registry.ctx().get<GameState>();
            val.states.push_back(state);
        }
        void GameStateSystem::popTopState(entt::registry &registry) noexcept {
            assert_msg(
                registry.ctx().contains<GameState>(),
                "参数 `registry` 没有 `tnrw::ecs::GameState` 组件，请将本消息反馈到项目的 Issue 中");
            auto val = registry.ctx().get<GameState>();
            val.states.pop_back();
        }
        [[notiscard]] GameState::State
        GameStateSystem::getTopState(const entt::registry &registry) noexcept {
            assert_msg(
                registry.ctx().contains<GameState>(),
                "参数 `registry` 没有 `tnrw::ecs::GameState` 组件，请将本消息反馈到项目的 Issue 中");
            const auto val = registry.ctx().get<GameState>();
            return val.states.back();
        }

        [[notiscard]] bool GameStateSystem::tryPushTopState(entt::registry        &registry,
                                                            const GameState::State state) noexcept {
            if (!registry.ctx().contains<GameState>())
                return false;
            auto val = registry.ctx().get<GameState>();
            val.states.push_back(state);
            return true;
        }
        [[notiscard]] bool GameStateSystem::tryPopTopState(entt::registry &registry) noexcept {
            if (!registry.ctx().contains<GameState>())
                return false;
            auto val = registry.ctx().get<GameState>();
            val.states.pop_back();
            return true;
        }
        [[notiscard]] std::optional<GameState::State>
        GameStateSystem::tryGetTopState(const entt::registry &registry) noexcept {
            if (!registry.ctx().contains<GameState>())
                return std::nullopt;
            const auto val = registry.ctx().get<GameState>();
            return val.states.back();
        }

    } // namespace ecs

} // namespace tnrw