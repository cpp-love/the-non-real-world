/**
 * @file game_state_manager.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏状态管理系统
 * @version 0.1.0-2
 * @date 2026-02-20
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_SYSTEMS_GLOBAL_GAME_STATE_MANAGER_HPP
#define TNRW_ECS_SYSTEMS_GLOBAL_GAME_STATE_MANAGER_HPP

#include "ecs/components/global/game_state_components.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <memory>
#include <vector>

namespace tnrw::ecs {

    /// @brief 游戏状态管理系统
    class game_state_manager {
      private:
        /**
         * @brief 状态栈
         * @details
         * 我们称 `m_states[i]` (`i` 为索引) 为一个块
         * 互相不阻隔的状态在同一个块中，
         * 互相阻隔的状态在不同的块中。
         * 
         * 就像这样：
         * 
         * > | A B C | D A B C | D A | D |
         * > 栈底 --------------------- 栈顶
         * > 其中 D 会阻隔绘制、更新、事件往下传递
         */
        std::vector<std::vector<std::unique_ptr<game_state_base>>> m_states;
        entt::dispatcher                                           m_dispatcher; ///< 分配器

      public:
        /**
         * @brief 默认构造函数
         * @details 构造一个含有 `mainpage` 状态的对象
         */
        game_state_manager() noexcept;
        /**
         * @brief 压入状态
         * @param [in] state 状态
         */
        void push_state(game_state_enum state) noexcept;
        /// @brief 弹出状态
        void pop_state() noexcept;
        /// @brief 重启对象
        void reset() noexcept;
        /**
         * @brief 处理事件
         * @param [in] event 事件
         * @return true 事件已处理
         * @return false 事件未处理
         */
        bool handle_event(const sf::Event &event) noexcept;
        /**
         * @brief 更新游戏状态
         * @param [in] delta_time 时间间隔
         */
        void update(milliseconds_f delta_time) noexcept;
        /**
         * @brief 绘制当前状态
         * @param [in] render 需要渲染的地方
         */
        void draw(sf::RenderTarget &render) noexcept;

      private:
        /**
         * @brief 响应压入状态的函数
         * @param [in] event 压入状态
         */
        void on_push_state(game_state_push_event event) noexcept;
        /**
         * @brief 响应弹出状态的函数
         * @param [in] event 弹出状态
         */
        void on_pop_state([[maybe_unused]] game_state_pop_event event) noexcept;
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_SYSTEMS_GLOBAL_GAME_STATE_MANAGER_HPP