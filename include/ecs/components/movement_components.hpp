/**
 * @file movement_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加了移动相关的组件
 * @version 0.1.0-1
 * @date 2025-11-15
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_COMPONENTS_MOVEMENT_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_MOVEMENT_COMPONENTS_HPP

#include <SFML/System/Vector2.hpp>

namespace tnrw::ecs {

    /**
     * @brief 速度组件
     * @attention 此组件不是一个稳定的组件，可能会因需求变多而改变
     */
    struct [[nodiscard]] Velocity {
        sf::Vector2f velocity;
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_MOVEMENT_COMPONENTS_HPP