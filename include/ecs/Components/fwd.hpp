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

#ifndef __ECS_COMPONENTS_FWD_HPP__
#define __ECS_COMPONENTS_FWD_HPP__

namespace tnrw {

    namespace ecs {

        // global/GameState.hpp
        class GameState final;

        // global/ScenesComponents.hpp
        class GlobalScenes final;
        class Scene        final;
        class FatherScenes final;

    } // namespace ecs

} // namespace tnrw

#endif