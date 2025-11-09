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

#ifndef __TNRW_ECS_SYSTEMS_GLOBAL_GAME_STATE_SYSTEM_HPP__
#define __TNRW_ECS_SYSTEMS_GLOBAL_GAME_STATE_SYSTEM_HPP__

#include "ecs/Components/global/GameState.hpp"
#include <entt/fwd.hpp>
#include <optional>

namespace tnrw::ecs {

    /// @brief 游戏状态系统
    class GameStateSystem final {
      public: /// @publicsection
        /**
          * @brief 创建游戏状态
          * @param [in] registry 注册表
          * @return true 状态创建成功
          * @return false 状态已创建
          */
        static bool createGameState(entt::registry &registry) noexcept;

        /**
         * @brief 加入状态到顶层
         * @param [in] registry 注册表
         * @param [in] state 注册表
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::GameStateSystem::tryPushTopState(entt::registry &registry, const tnrw::ecs::GameState::State state)
         */
        static void pushTopState(entt::registry &registry, GameState::State state) noexcept;
        /**
         * @brief 删除顶层状态
         * @param [in] registry 注册表
         * @warning 如果组件没有状态，行为未定义
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::GameStateSystem::tryPopState(entt::registry &registry)
         */
        static void popTopState(entt::registry &registry) noexcept;
        /**
         * @brief 获取顶层状态
         * @param [in] registry 注册表
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::GameStateSystem::tryGetTopState(const entt::registry &registry)
         */
        [[nodiscard]] static GameState::State getTopState(const entt::registry &registry) noexcept;
        /**
         * @brief 获取状态列表
         * @param [in] registry 注册表
         * @return const std::vector<GameState::State> & 状态列表
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::GameStateSystem::tryGetStates(const entt::registry &registry)
         */
        [[nodiscard]] static const std::vector<GameState::State>                           &
        getStates(const entt::registry &registry) noexcept;

        /**
         * @brief 尝试加入状态到顶层
         * @param [in] registry 注册表
         * @param [in] state 状态
         * @return true 加入成功
         * @return false 加入失败
         * @note 失败不会影响原来的组件状态
         */
        [[nodiscard]] static bool tryPushTopState(entt::registry  &registry,
                                                  GameState::State state) noexcept;

        /**
         * @brief 尝试删除顶层状态
         * @param [in] registry 注册表
         * @return true 删除成功
         * @return false 删除失败
         * @note 如果组件没有状态，行为未定义
         * @note 失败不会影响原来的组件状态
         */
        [[nodiscard]] static bool tryPopTopState(entt::registry &registry) noexcept;
        /**
         * @brief 尝试获取顶层状态
         * @param [in] registry 注册表
         * @return std::optional<GameState::State> 如果获取成功返回状态，失败返回 `std::nullopt`
         * @note 失败不会影响原来的组件状态
         */
        [[nodiscard]] static std::optional<GameState::State>
        tryGetTopState(const entt::registry &registry) noexcept;

        /**
         * @brief 尝试获取状态列表
         * @param [in] registry 注册表
         * @return const std::vector<GameState::State> * 如果成功返回指向状态列表指针，失败返回 `nullptr`
         * @note 失败不会影响原来的组件状态
         */
        [[nodiscard]] static const std::vector<GameState::State> *
        tryGetStates(const entt::registry &registry) noexcept;
    };

} // namespace tnrw::ecs

#endif // __TNRW_ECS_SYSTEMS_GLOBAL_GAME_STATE_SYSTEM_HPP__