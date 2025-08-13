/**
 * @file GameStateSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/Systems/GameStateSystem.hpp"

namespace tnrw {

    namespace ecs {

        entt::entity GameStateSystem::createGameState(entt::registry &registry) noexcept {
            entt::entity entity = registry.create();
            registry.emplace<GameState>(entity);
            return entity;
        }

        void GameStateSystem::pushTopState(entt::registry        &registry,
                                           entt::entity           entity = entt::null,
                                           const GameState::State state) noexcept {
            if (entity == entt::null) {
                entity = *(registry.view<GameState>().begin());
            }
            auto val = registry.get<GameState>(entity);
            val.states.push_back(state);
        }
        void GameStateSystem::popTopState(entt::registry &registry,
                                          entt::entity    entity = entt::null) noexcept {
            if (entity == entt::null) {
                entity = *(registry.view<GameState>().begin());
            }
            auto val = registry.get<GameState>(entity);
            if (val.states.empty())
                return;
            val.states.pop_back();
        }
        GameState::State
        GameStateSystem::getTopState(entt::registry &registry,
                                     entt::entity    entity = entt::null) noexcept {
            if (entity == entt::null) {
                entity = *(registry.view<GameState>().begin());
            }
            auto val = registry.get<GameState>(entity);
            return val.states.back();
        }

        bool GameStateSystem::tryPushTopState(entt::registry        &registry,
                                              entt::entity           entity = entt::null,
                                              const GameState::State state) noexcept {
            if (entity == entt::null) {
                entity = *(registry.view<GameState>().begin());
            }
            auto val = registry.try_get<GameState>(entity);
            if (val == nullptr) {
                return false;
            }
            val->states.push_back(state);
            return true;
        }
        bool GameStateSystem::tryPopTopState(entt::registry &registry,
                                             entt::entity entity = entt::null) noexcept {
            if (entity == entt::null) {
                entity = *(registry.view<GameState>().begin());
            }
            auto val = registry.try_get<GameState>(entity);
            if (val == nullptr || val->states.empty()) {
                return false;
            }
            val->states.pop_back();
            return true;
        }
        std::optional<GameState::State>
        GameStateSystem::tryGetTopState(entt::registry &registry,
                                        entt::entity    entity = entt::null) noexcept {
            if (entity == entt::null) {
                entity = *(registry.view<GameState>().begin());
            }
            auto val = registry.try_get<GameState>(entity);
            if (val == nullptr) {
                return std::nullopt;
            }
            return val->states.back();
        }

    } // namespace ecs

} // namespace tnrw