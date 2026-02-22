/**
 * @file scene_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏场景系统
 * @version 0.1.0-2
 * @date 2026-02-22
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 此游戏状态系统为内嵌式系统，即与 `entt::registry` 绑定
 * 
 */

#ifndef TNRW_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP
#define TNRW_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP

#include "base/config.hpp"
#include "ecs/components/global/scene_components.hpp"
#include <entt/fwd.hpp>

namespace tnrw::ecs {

    /// @brief 基本游戏场景系统
    class scene_system {
      public:
        /**
         * @brief 插入一个场景实体
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id
         * @return true 成功，场景之前未创建过
         * @return false 失败，场景之前已创建过
         */
        static bool insert_scene(entt::registry &registry, level_identifier_type scene_id) noexcept;
        /**
         * @brief 删除一个场景实体
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id
         * @return true 删除成功，场景之前存在
         * @return false 删除失败，场景之前不存在
         * @warning 此函数不会删除场景的子实体，需要手动删除
         */
        static bool erase_scene(entt::registry &registry, level_identifier_type scene_id) noexcept;
        /**
         * @brief 清除所有的场景实体
         * @param [in] registry 注册表
         * @warning 此函数不会删除场景的子实体，需要手动删除
         */
        static void clear_scenes(entt::registry &registry) noexcept;
        /**
         * @brief 获取所有场景实体的列表
         * @param [in] registry 注册表
         * @return const std::map<level_identifier_type, std::set<entt::entity>>& 所有的场景及其子实体
         */
        [[nodiscard]] static const std::map<level_identifier_type, std::set<entt::entity>>             &
        get_scenes(entt::registry &registry) noexcept;
        /**
         * @brief 将子实体加入场景中
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id
         * @param [in] child_entity 子实体
         * @return true 成功，子实体之前未加入过
         * @return false 失败，子实体之前已加入过
         */
        static bool insert_to_scene(entt::registry &registry, level_identifier_type scene_id,
                                    entt::entity child_entity) noexcept;
        /**
         * @brief 将子实体从场景中移除
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id
         * @param [in] child_entity 子实体
         * @return true 成功，子实体之前已加入过
         * @return false 失败，子实体之前未加入过
         */
        static bool erase_from_scene(entt::registry &registry, level_identifier_type scene_id,
                                     entt::entity child_entity) noexcept;
        /**
         * @brief 获取场景所有孩子的集合
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id
         * @return const std::set<entt::entity>& 场景所有孩子的集合
         */
        [[nodiscard]] static const std::set<entt::entity> &
        get_scene_children(entt::registry &registry, level_identifier_type scene_id) noexcept;
        /**
         * @brief 获取子实体所有父场景的集合
         * @param [in] registry 注册表
         * @param [in] child_entity 子实体
         * @return const std::set<level_identifier_type>& 子实体所有父场景的集合
         */
        [[nodiscard]] static const std::set<level_identifier_type> &
        get_father_scenes(entt::registry &registry, entt::entity child_entity) noexcept;
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP