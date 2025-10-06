/**
 * @file fwd.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief ecs命名空间内容的系统部分的前向声明头文件
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#ifndef __ECS_SYSTEMS_FWD_HPP__
#define __ECS_SYSTEMS_FWD_HPP__

#include "base/config.hpp"
#include "global/SceneSystem.hpp"
#include <algorithm>
#include <entt/fwd.hpp>
#include <memory>
#include <utility>

namespace tnrw {

    namespace ecs {

        // global/GameStateSystem.hpp
        class GameStateSystem final;

    } // namespace ecs

} // namespace tnrw

#endif // __ECS_SYSTEMS_FWD_HPP__