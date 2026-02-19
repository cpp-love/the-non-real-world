/**
 * @file render_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了渲染系统及其基本功能
 * @version 0.1.0-1
 * @date 2025-10-18
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP
#define TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP

#include "ecs/components/shape_components.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include <entt/fwd.hpp>
#include <functional>

namespace tnrw::ecs {

    /// @brief 渲染系统
    class render_system {
      public: /// @publicsection
        /**
         * @brief 获取绘制的圆会有的点数
         * @return std::size_t 绘制的圆会有的点数
         */
        [[nodiscard]] static std::size_t get_circle_point_count() noexcept;
        /**
         * @brief 设置绘制的圆会有的点数
         * @param [in] new_cnt 新的绘制的圆会有的点数
         */
        static void                      set_circle_point_count(std::size_t new_cnt) noexcept;

        /**
         * @brief 绘制注册表里的内容
         * @param [in] render 需要渲染的地方
         * @param [in] registry 注册表
         * @param [in] check_if_valid 检查是否有资格渲染，不需要检查 @ref tnrw::ecs::ShouldRender 组件是否存在
         * @note 此函数会自动帮你忽略没有 @ref tnrw::ecs::ShouldRender 的组件
         * @note 此函数不会调用 `render.clear()`，需要使用者手动调用
         */
        static void                      draw(const entt::registry &registry, sf::RenderTarget &render,
                                              std::function<bool(entt::entity)> check_if_valid) noexcept;
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP