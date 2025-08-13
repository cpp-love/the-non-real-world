/**
 * @file GameStateSystem.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏状态系统
 * @version 0.1.0-1
 * @date 2025-07-26
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 此游戏状态系统为内嵌式系统，即与 `entt::registry` 绑定
 * - 状态采用 `tnrw::ecs::GameState` （内部为 `std::vector` ）存储，
 *   支持类似栈的操作，即支持压入、取出、查询顶部元素的操作
 * - 为了高性能，分为了不安全（即不判断是否有组件）与安全的两类操作，
 *   安全操作会返回 `bool` 或 `std::optional` 判断操作是否成功
 * 
 */

#ifndef __ECS_SYSTEMS_GAME_STATE_SYSTEM_HPP__
#define __ECS_SYSTEMS_GAME_STATE_SYSTEM_HPP__

#include "ecs/Components/GameState.hpp"
#include <entt/entt.hpp>
#include <optional>

namespace tnrw {

    namespace ecs {

        /// @brief 游戏状态系统
        class GameStateSystem final {
          public: /// @publicsection
            /**
             * @brief 创建游戏状态
             * @param [in] registry 注册表
             * @return entt::entity 拥有游戏状态的实体
             */
            static entt::entity createGameState(entt::registry &registry) noexcept;

            /**
             * @brief 加入状态到顶层
             * @param [in] registry 注册表
             * @param [in] entity 拥有游戏状态的实体（可选）
             * @param [in] state 状态
             * @note 如果实体没有组件，行为未定义
             */
            static void pushTopState(entt::registry        &registry,
                                     entt::entity           entity = entt::null,
                                     const GameState::State state) noexcept;
            /**
             * @brief 删除顶层状态
             * @param [in] registry 注册表
             * @param [in] entity 拥有游戏状态的实体（可选）
             * @note 如果实体没有组件，行为未定义
             */
            static void popTopState(entt::registry &registry,
                                    entt::entity    entity = entt::null) noexcept;
            /**
             * @brief 获取顶层状态
             * @param [in] registry 注册表
             * @param [in] entity 拥有游戏状态的实体（可选）
             * @note 如果实体没有组件，行为未定义
             */
            static GameState::State
            getTopState(entt::registry &registry,
                        entt::entity    entity = entt::null) noexcept;

            /**
             * @brief 尝试加入状态到顶层
             * @param [in] registry 注册表
             * @param [in] entity 拥有游戏状态的实体（可选）
             * @param [in] state 状态
             * @return true 加入成功
             * @return false 加入失败
             * @note 失败不会影响原来的组件状态
             */
            static bool tryPushTopState(entt::registry        &registry,
                                        entt::entity           entity = entt::null,
                                        const GameState::State state) noexcept;

            /**
             * @brief 尝试删除顶层状态
             * @param [in] registry 注册表
             * @param [in] entity 拥有游戏状态的实体（可选）
             * @return true 删除成功
             * @return false 删除失败
             * @note 失败不会影响原来的组件状态
             */
            static bool tryPopTopState(entt::registry &registry,
                                       entt::entity    entity = entt::null) noexcept;
            /**
             * @brief 尝试获取顶层状态
             * @param [in] registry 注册表
             * @param [in] entity 拥有游戏状态的实体（可选）
             * @return std::optional<GameState::State> 如果获取成功返回状态，失败返回 `std::nullopt`
             * @note 失败不会影响原来的组件状态
             */
            static std::optional<GameState::State>
            tryGetTopState(entt::registry &registry,
                           entt::entity    entity = entt::null) noexcept;
        };

    } // namespace ecs

} // namespace tnrw

#endif // __ECS_SYSTEMS_GAME_STATE_SYSTEM_HPP__