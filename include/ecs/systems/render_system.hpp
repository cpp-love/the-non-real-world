/**
 * @file render_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了渲染系统及其基本功能
 * @version 0.1.0-3
 * @date 2026-03-14
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP
#define TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP

#include "ecs/components/shape_components.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include <entt/fwd.hpp>
#include <functional>

namespace tnrw::ecs {

    /// @brief 渲染系统
    class render_system {
      public:
        /**
         * @brief 绘制注册表里的内容
         * @param [in] registry 注册表
         * @param [in] check_if_valid 检查是否有资格渲染，
         *                            参数为 `entt::entity`，即有 @ref tnrw::ecs::collidable_shape 组件的实体
         *                            返回实体是否有资格渲染
         * @param [in] target 需要渲染的地方
         * @param [in] states 渲染状态
         * @details 此函数会在注册表中寻找 @ref tnrw::ecs::collidable_shape 组件，
         * 然后调用 `check_if_valid`，如果返回 `true` 就渲染
         * @note 此函数不会调用 `render.clear()`，需要使用者手动调用
         */
        static void draw(const entt::registry                    &registry,
                         const std::function<bool(entt::entity)> &check_if_valid,
                         sf::RenderTarget                        &target,
                         sf::RenderStates states = sf::RenderStates::Default) noexcept;
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP