/**
 * @file scene_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏场景系统
 * @version 0.1.0-1
 * @date 2026-02-22
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/global/scene_system.hpp"
#include "base/assert_msg.hpp"
#include "ecs/components/global/scene_components.hpp"
#include <entt/entity/registry.hpp>
#include <utility>

namespace tnrw::ecs {

    namespace {

        /**
         * @brief 获取 @ref game_scenes 对象，没有就创建
         * @param [in] registry 注册表
         * @return std::map<level_identifier_type, std::set<entt::entity>>& 在 `registry.ctx()` 上创建的对象
         */
        std::map<level_identifier_type, std::set<entt::entity>> &
        get_or_create_scenes(entt::registry &registry) {
            if (!registry.ctx().contains<game_scenes>()) {
                registry.ctx().emplace<game_scenes>();
            }
            return registry.ctx().get<game_scenes>().m_scenes_childrens;
        }

        /**
         * @brief 获取场景的子实体列表，没有就创建
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id
         * @return std::pair<std::set<entt::entity> &, bool> 子实体列表和是否已经创建过
         */
        std::pair<std::set<entt::entity> &, bool>
        get_or_create_scene_children(entt::registry &registry, level_identifier_type scene_id) {
            auto &scenes_childrens = get_or_create_scenes(registry);
            auto [iter, not_inserted] = scenes_childrens.try_emplace(scene_id);
            if (not_inserted) {
                spdlog::info("添加场景：标识符为{}", scene_id);
            }
            return {iter->second, not_inserted};
        }

    } // namespace

    bool scene_system::insert_scene(entt::registry &registry, level_identifier_type scene_id) noexcept {
        return get_or_create_scene_children(registry, scene_id).second;
    }
    bool scene_system::erase_scene(entt::registry &registry, level_identifier_type scene_id) noexcept {
        auto &scenes_childrens = get_or_create_scenes(registry);
        auto  scene_it = scenes_childrens.find(scene_id);
        if (scene_it == scenes_childrens.end()) {
            return false;
        }
        spdlog::info("删除场景：标识符为{}", scene_id);
        // 删除子实体对场景的引用
        for (entt::entity child : scene_it->second) {
            auto                 &fathers = registry.get<father_scenes>(child).m_fathers;
            [[maybe_unused]] auto cnt = fathers.erase(scene_id);
            ASSERT_MSG(cnt == 1, "子实体错误地未删除父场景");
        }
        // 删除场景
        scenes_childrens.erase(scene_it);
        return true;
    }
    void scene_system::clear_scenes(entt::registry &registry) noexcept {
        if (!registry.ctx().contains<game_scenes>()) {
            return;
        }
        auto &scenes_childrens = registry.ctx().get<game_scenes>().m_scenes_childrens;
        for (auto it = scenes_childrens.begin(); it != scenes_childrens.end();) {
            spdlog::info("删除场景：标识符为{}", it->first);
            it = scenes_childrens.erase(it);
        }
        registry.clear<father_scenes>();
    }
    [[nodiscard]] const std::map<level_identifier_type, std::set<entt::entity>> &
    scene_system::get_scenes(entt::registry &registry) noexcept {
        return get_or_create_scenes(registry);
    }
    bool scene_system::insert_to_scene(entt::registry &registry, level_identifier_type scene_id,
                                       entt::entity child_entity) noexcept {
        auto                 &children = get_or_create_scene_children(registry, scene_id).first;
        bool                  is_inserted = children.insert(child_entity).second;
        [[maybe_unused]] bool is_inserted2 =
            registry.get_or_emplace<father_scenes>(child_entity).m_fathers.insert(scene_id).second;
        ASSERT_MSG(is_inserted == is_inserted2, "子实体和父场景的两处登记处的信息错误地不一致");
        return is_inserted;
    }
    bool scene_system::erase_from_scene(entt::registry &registry, level_identifier_type scene_id,
                                        entt::entity child_entity) noexcept {
        auto                 &children = get_or_create_scene_children(registry, scene_id).first;
        bool                  is_inserted = static_cast<bool>(children.erase(child_entity));
        [[maybe_unused]] bool is_inserted2 = static_cast<bool>(
            registry.get_or_emplace<father_scenes>(child_entity).m_fathers.erase(scene_id));
        ASSERT_MSG(is_inserted == is_inserted2, "子实体和父场景的两处登记处的信息错误地不一致");
        return is_inserted;
    }
    [[nodiscard]] const std::set<entt::entity> &
    scene_system::get_scene_children(entt::registry &registry, level_identifier_type scene_id) noexcept {
        return get_or_create_scene_children(registry, scene_id).first;
    }

    [[nodiscard]] const std::set<level_identifier_type> &
    scene_system::get_father_scenes(entt::registry &registry, entt::entity child_entity) noexcept {
        return registry.get_or_emplace<father_scenes>(child_entity).m_fathers;
    }

} // namespace tnrw::ecs