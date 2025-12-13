/**
 * @file GameStateComponents.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了游戏状态的组件
 * @version 0.1.0-1
 * @date 2025-07-26
 * 
 * @copyright cpp-love
 * 
 * @details 状态采用 `std::vector` 存储，支持不断加入状态
 * 
 */

#ifndef __TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP__
#define __TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP__

#include "base/assert_msg.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace tnrw::ecs {

    /// @brief 游戏状态的组件
    class [[nodiscard]] GameState final {
      public: /// @publicsection
        /// @brief 游戏的状态枚举
        enum class State : std::uint8_t {
            Settings = 0, ///< 设置
            Mainpage = 1, ///< 主页
            Game = 2      ///< 游戏界面
        };
        // 友元声明
        friend class GameStateSystem; ///< 仅其对应系统才可访问其成员，防止篡改

        static constexpr std::string_view toString(State state) {
            switch (state) {
                case State::Settings: return "Settings";
                case State::Game: return "Game";
                case State::Mainpage: return "Mainpage";
                default:
                    assert_msg(false, "参数 `state` （整数形式为：{}）有无法转换为字符串的状态",
                               static_cast<std::uint8_t>(state));
            }
            return {};
        }

        /// @cond INTERNAL
      private: /// @privatesection
        // 成员
        std::vector<State> states; ///< 游戏状态集
        /// @endcond
    };

} // namespace tnrw::ecs

#endif // __TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP__