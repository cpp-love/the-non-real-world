/**
 * @file movement_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了移动系统及其功能
 * @version 0.1.0-3
 * @date 2026-03-14
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_SYSTEMS_MOVEMENT_SYSTEM_HPP
#define TNRW_ECS_SYSTEMS_MOVEMENT_SYSTEM_HPP

#include "ecs/components/global/game_base.hpp"
#include "ecs/components/movement_components.hpp"
#include "ecs/components/shape_components.hpp"
#include <entt/entity/fwd.hpp>

namespace tnrw::ecs {

    class movement_system {
      public:
        /**
         * @brief 获取实体的速度引用
         * @param [in] registry 注册表
         * @param [in] entity 实体
         * @return sf::Vector2f 速度引用，以 `像素/ms` 为单位
         * @attention 虽然可以使用 `registry.get<tnrw::ecs::Velocity>(entity, velocity).velocity` 获取速度，
         * 但是由于 `tnrw::ecs::Velocity` 是一个不稳定的组件，可能会变更，所以更推荐使用此函数 
         */
        [[nodiscard]] static sf::Vector2f &velocity(entt::registry &registry,
                                                    entt::entity    entity) noexcept;
        /**
         * @brief 更新一个实体
         * @param [in] registry 注册表
         * @param [in] entity 实体
         * @param [in] delta_time 更新时间
         * @warning 此函数目前只支持圆形 (碰撞箱为 `tnrw::ecs::shape::circle`) 的移动，其他形状的实体可能会断言失败
         * @warning 此函数的碰撞只关注在同一场景里的内容，其他场景会被忽略
         * @warning 若实体没有组件 @ref tnrw::ecs::collidable_shape，可能会断言失败
         */
        static void update_with_velocity(entt::registry &registry, entt::entity entity,
                                         milliseconds_f delta_time) noexcept;
        /**
         * @brief 更新所有实体
         * @param [in] registry 注册表
         * @param [in] delta_time 更新时间
         * @details 此函数会在注册表中寻找 @ref tnrw::ecs::collidable_shape 组件，
         * 然后调用 `update_with_velocity` 的单实体版本
         * @warning 此函数目前只支持圆形 (碰撞箱为 `tnrw::ecs::shape::circle`) 的移动，其他形状的实体会直接跳过
         * @warning 此函数的碰撞只关注在同一场景里的内容，其他场景会被忽略
         */
        static void update_with_velocity(entt::registry &registry, milliseconds_f delta_time) noexcept;
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_SYSTEMS_MOVEMENT_SYSTEM_HPP