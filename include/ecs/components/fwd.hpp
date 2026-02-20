/**
 * @file fwd.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief ecs命名空间内容的组件部分的前向声明头文件
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_COMPONENTS_FWD_HPP
#define TNRW_ECS_COMPONENTS_FWD_HPP

#include "ecs/components/global/game_base.hpp"
#include "ecs/components/global/scene_components.hpp"
#include <SFML/System/Vector2.hpp>

namespace tnrw::ecs {

    // global/game_state_components.hpp
    class game_state;

    // shape_components.hpp
    struct shape;
    struct should_render;
    struct should_collide;

    // movement_components.hpp
    struct velocity;

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_FWD_HPP