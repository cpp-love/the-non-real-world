/**
 * @file game_state_components.hpp
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

#ifndef TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP

#include "base/assert_msg.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace tnrw::ecs {

    /// @brief 游戏状态的组件
    class [[nodiscard]] game_state {
      public: /// @publicsection
        /// @brief 游戏的状态枚举
        enum class state : std::uint8_t {
            settings = 0, ///< 设置
            mainpage = 1, ///< 主页
            game = 2      ///< 游戏界面
        };
        // 友元声明
        friend class game_state_system; ///< 仅其对应系统才可访问其成员，防止篡改

        static constexpr std::string_view to_string(state state) {
            switch (state) {
                case state::settings: return "Settings";
                case state::game: return "Game";
                case state::mainpage: return "Mainpage";
                default:
                    assert_msg(false, "参数 `state` （整数形式为：{}）有无法转换为字符串的状态",
                               static_cast<std::uint8_t>(state));
            }
            return {};
        }

        /// @cond INTERNAL
      private: /// @privatesection
        // 成员
        std::vector<state> m_states; ///< 游戏状态集
        /// @endcond
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP