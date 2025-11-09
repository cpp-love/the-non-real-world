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

#ifndef __TNRW_ECS_COMPONENTS_FWD_HPP__
#define __TNRW_ECS_COMPONENTS_FWD_HPP__

#include "ecs/Components/global/SceneComponents.hpp"
#include <SFML/System/Vector2.hpp>

namespace tnrw::ecs {

    // global/GameState.hpp
    class GameState;

    // ShapeComponents.hpp
    struct Line;
    struct Circle;
    struct Rectangle;
    struct ShouldRender;
    struct ShouldCollide;

} // namespace tnrw::ecs

#endif // __TNRW_ECS_COMPONENTS_FWD_HPP__