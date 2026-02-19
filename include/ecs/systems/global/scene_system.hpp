/**
 * @file scene_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏场景系统
 * @version 0.1.0-1
 * @date 2025-08-14
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 此游戏状态系统为内嵌式系统，即与 `entt::registry` 绑定
 * - 为了高性能，分为了不安全（即不判断是否有组件）与安全的两类操作，
 *   安全操作会返回 `bool` 或 `std::optional` 判断操作是否成功
 * 
 */

#ifndef TNRW_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP
#define TNRW_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP

#include "base/assert_msg.hpp"
#include "base/config.hpp"
#include "ecs/components/global/scene_components.hpp"
#include <entt/entt.hpp>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <utility>

namespace tnrw::ecs {

    /**
     * @brief 基本游戏场景系统
     * @tparam Key 键类型
     * @tparam Compare 可选的比较类型
     * @tparam std::allocator<std::pair<const Key, entt::entity>> 可选的分配器类型
     */
    template <typename Key, typename Compare = std::less<Key>,
              typename Allocator = std::allocator<std::pair<const Key, entt::entity>>>
    class basic_scene_system {
      public: /// @publicsection
        // using 声明
        using key_type = Key;             //< 键类型
        using key_compare = Compare;      //< 键比较类型
        using allocator_type = Allocator; //< 分配器类型

        /**
         * @brief 创建一个场景实体
         * @param [in] registry 注册表
         * @param [in] name 场景名字
         * @return std::optional<entt::entity> 场景的实体和是否已创建过
         */
        static std::optional<entt::entity> create_scene(entt::registry &registry,
                                                        const Key      &name) noexcept {
            create_global_scenes(registry); //< 确保全局状态已被创建
            auto newscene = registry.create();
            registry.emplace<scene>(newscene);
            auto res = registry.ctx()
                           .get<global_scenes<key_type, key_compare, allocator_type>>()
                           .m_scenes.try_emplace(name, newscene);
            if (!res.second) {
                registry.destroy(newscene);
                return std::nullopt;
            }
            spdlog::info("添加场景：（实体编号：{}，名称：{}）", static_cast<entt::id_type>(newscene),
                         name);
            return newscene;
        }
        /**
         * @brief 删除一个场景实体
         * @param [in] registry 注册表
         * @param [in] name 场景名字
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @warning 此函数不会删除子实体，需要手动删除
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryEraseScene(entt::registry &registry, const key_type &name)
         */
        static void erase_scene(entt::registry &registry, const key_type &name) noexcept {
            assert_msg((registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()),
                       "参数 `registry` 没有 `tnrw::ecs::global_scenes` 组件");
            auto &scenes =
                registry.ctx().get<global_scenes<key_type, key_compare, allocator_type>>().m_scenes;
            auto scene_it = scenes.find(name);
            assert_msg((scene_it != scenes.end() && registry.all_of<scene>(scene_it->second)),
                       "参数 `name`（为{}） 没有对应的实体或对应的实体没有 `tnrw::ecs::scene` "
                       "组件",
                       name);
            spdlog::info("删除场景：（实体编号：{}，名称：{}）",
                         static_cast<entt::id_type>(scene_it->second), name);
            scenes.erase(scene_it);
            registry.destroy(scene_it->second);
        }
        /**
         * @brief 删除一个场景实体（不删除子实体）
         * @param [in] registry 注册表
         * @param [in] name 场景名字
         * @return true 删除成功
         * @return false 删除失败
         * @warning 此函数不会删除子实体，需要手动删除
         * @note 删除失败不影响原来的组件
         */
        [[nodiscard]] static bool try_erase_scene(entt::registry &registry,
                                                  const key_type &name) noexcept {
            if (!registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()) {
                return false;
            }
            auto &scenes =
                registry.ctx().get<global_scenes<key_type, key_compare, allocator_type>>().m_scenes;
            auto scene_it = scenes.find(name);
            if (scene_it == scenes.end() || !registry.all_of<scene>(scene_it->second)) {
                return false;
            }
            spdlog::info("删除场景：（实体编号：{}，名称：{}）",
                         static_cast<entt::id_type>(scene_it->second), name);
            scenes.erase(scene_it);
            registry.destroy(scene_it->second);
            return true;
        }
        /**
         * @brief 清除所有的场景实体（不删除子实体）
         * @param [in] registry 注册表
         */
        static void clear_scenes(entt::registry &registry) noexcept {
            create_global_scenes(registry); //< 确保全局状态已被创建
            auto &scenes =
                registry.ctx().get<global_scenes<key_type, key_compare, allocator_type>>().m_scenes;
            for (auto scene : scenes) {
                spdlog::info("删除场景：（实体编号：{}，名称：{}）",
                             static_cast<entt::id_type>(scene.second), scene.first);
                registry.destroy(scene.second);
            }
            scenes.clear();
        }
        /**
         * @brief 获取所有场景实体的列表
         * @param [in] registry 注册表
         * @return const std::map<key_type, entt::entity>& 所有的场景实体
         */
        [[nodiscard]] static const std::map<key_type, entt::entity> &
        get_scenes(entt::registry &registry) noexcept {
            create_global_scenes(registry); //< 确保全局状态已被创建
            return registry.ctx().get<global_scenes<key_type, key_compare, allocator_type>>().m_scenes;
        }
        /**
         * @brief 获取场景名字所对应的实体
         * @param [in] registry 注册表
         * @param [in] name 场景名字
         * @return entt::entity 场景名字所对应的实体
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryGetSceneEntity(entt::registry &registry, const key_type &name)
         */
        [[nodiscard]] static entt::entity get_scene_entity(const entt::registry &registry,
                                                           const key_type       &name) noexcept {
            assert_msg((registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()),
                       "参数 `registry` 没有 `tnrw::ecs::global_scenes` 组件");
            const auto &scenes =
                registry.ctx().get<global_scenes<key_type, key_compare, allocator_type>>().m_scenes;
            const auto scene_it = scenes.find(name);
            assert_msg(scene_it != scenes.end(), "参数 `name`（为：{}） 没有对应的实体", name);
            return scene_it->second;
        }
        /**
         * @brief 获取场景名字所对应的实体
         * @param [in] registry 注册表
         * @param [in] name 注册表
         * @return entt::entity 场景名字所对应的实体
         * @retval entt::null 如果没有对应的实体时返回此值
         * @note 获取失败不影响原来的组件
         */
        [[nodiscard]] static entt::entity try_get_scene_entity(const entt::registry &registry,
                                                               const key_type       &name) noexcept {
            if (!registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()) {
                return entt::null;
            }
            const auto &scenes =
                registry.ctx().get<global_scenes<key_type, key_compare, allocator_type>>().m_scenes;
            const auto scene_it = scenes.find(name);
            if (scene_it == scenes.end()) {
                return entt::null;
            }
            return scene_it->second;
        }
        /**
         * @brief 将子实体加入场景中
         * @param [in] registry 注册表
         * @param [in] scene_entity 场景实体
         * @param [in] child_entity 子实体
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryAddToScene(entt::registry &registry, const entt::entity scene_entity, const entt::entity child_entity)
         */
        static void add_to_scene(entt::registry &registry, const entt::entity scene_entity,
                                 const entt::entity child_entity) noexcept {
            assert_msg((registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()),
                       "参数 `registry` 没有 `tnrw::ecs::global_scenes` 组件");
            assert_msg(registry.all_of<scene>(scene_entity),
                       "参数 `scene_entity`（编号为：{}） 没有 `tnrw::ecs::scene` "
                       "组件",
                       static_cast<entt::id_type>(scene_entity));

            create_father_scenes(registry, child_entity); //< 确保组件已被创建
            registry.get<scene>(scene_entity).m_children.insert(child_entity);
            registry.get<father_scenes>(child_entity).m_fathers.insert(scene_entity);
        }
        /**
         * @brief 将子实体加入场景中
         * @param [in] registry 注册表
         * @param [in] scene_name 场景名字
         * @param [in] child_entity 子实体
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryAddToScene(entt::registry &registry, const key_type &scene_name, const entt::entity child_entity)
         */
        static void add_to_scene(entt::registry &registry, const key_type &scene_name,
                                 const entt::entity child_entity) noexcept {
            add_to_scene(registry, get_scene_entity(registry, scene_name), child_entity);
        }
        /**
         * @brief 将子实体加入场景中
         * @param [in] registry 注册表
         * @param [in] scene_entity 场景实体
         * @param [in] child_entity 子实体
         * @return true 加入成功
         * @return false 加入失败
         * @note 加入失败不影响原来的组件
         */
        [[nodiscard]] static bool try_add_to_scene(entt::registry    &registry,
                                                   const entt::entity scene_entity,
                                                   const entt::entity child_entity) noexcept {
            if (!registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()) {
                return false;
            }
            if (!registry.all_of<scene>(scene_entity)) {
                return false;
            }

            create_father_scenes(registry, child_entity); //< 确保组件已被创建
            auto tmp = registry.get<scene>(scene_entity).m_children.insert(child_entity).second;
            auto tmp2 = registry.get<father_scenes>(child_entity).m_fathers.insert(scene_entity).second;
            return tmp && tmp2;
        }
        /**
         * @brief 将子实体加入场景中
         * @param [in] registry 注册表
         * @param [in] scene_name 场景名字
         * @param [in] child_entity 子实体
         * @return true 加入成功
         * @return false 加入失败
         * @note 加入失败不影响原来的组件
         */
        [[nodiscard]] static bool try_add_to_scene(entt::registry &registry, const key_type &scene_name,
                                                   const entt::entity child_entity) noexcept {
            auto scene_entity = try_get_scene_entity(registry, scene_name);
            if (scene_entity == entt::null) {
                return false;
            }
            return try_add_to_scene(registry, scene_entity, child_entity);
        }
        /**
         * @brief 将子实体从场景中移除
         * @param [in] registry 注册表
         * @param [in] scene_entity 场景实体
         * @param [in] child_entity 子实体
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryRemoveFromScene(entt::registry &registry, const entt::entity scene_entity, const entt::entity child_entity)
         */
        static void remove_from_scene(entt::registry &registry, const entt::entity scene_entity,
                                      const entt::entity child_entity) noexcept {
            assert_msg((registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()),
                       "参数 `registry` 没有 `tnrw::ecs::global_scenes` 组件");
            assert_msg(registry.all_of<scene>(scene_entity),
                       "参数 `scene_entity`（编号为：{}） 没有 `tnrw::ecs::scene` "
                       "组件",
                       static_cast<entt::id_type>(scene_entity));
            auto                 &children = registry.get<scene>(scene_entity).m_children;
            auto                 &fathers = registry.get<father_scenes>(child_entity).m_fathers;
            [[maybe_unused]] auto cnt = children.erase(child_entity);
            cnt += fathers.erase(scene_entity);
            assert_msg(cnt == 2,
                       "参数 `scene_entity`（编号为：{}） 没有 `child_entity` "
                       "子实体",
                       static_cast<entt::id_type>(scene_entity));
        }
        /**
         * @brief 将子实体从场景中移除
         * @param [in] registry 注册表
         * @param [in] scene_name 场景名字
         * @param [in] child_entity 子实体
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryRemoveFromScene(entt::registry &registry, const key_type &scene_name, const entt::entity child_entity)
             */
        static void remove_from_scene(entt::registry &registry, const key_type &scene_name,
                                      const entt::entity child_entity) noexcept {
            remove_from_scene(registry, get_scene_entity(registry, scene_name), child_entity);
        }
        /**
         * @brief 将子实体从场景中移除
         * @param [in] registry 注册表
         * @param [in] scene_entity 场景实体
         * @param [in] child_entity 子实体
         * @return true 移除成功
         * @return false 移除失败
         * @note 移除失败不影响原来的组件
         */
        [[nodiscard]] static bool try_remove_from_scene(entt::registry    &registry,
                                                        const entt::entity scene_entity,
                                                        const entt::entity child_entity) noexcept {
            if (!registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()) {
                return false;
            }
            if (!registry.all_of<scene>(scene_entity)) {
                return false;
            }
            create_father_scenes(registry, child_entity); //< 确保组件已被创建
            auto &children = registry.get<scene>(scene_entity).m_children;
            auto &fathers = registry.get<father_scenes>(child_entity).m_fathers;
            auto  cnt = children.erase(child_entity);
            cnt += fathers.erase(scene_entity);
            return cnt == 2;
        }
        /**
         * @brief 将子实体从场景中移除
         * @param [in] registry 注册表
         * @param [in] scene_name 场景名字
         * @param [in] child_entity 子实体
         * @return true 移除成功
         * @return false 移除失败
         * @note 移除失败不影响原来的组件
         */
        [[nodiscard]] static bool try_remove_from_scene(entt::registry    &registry,
                                                        const key_type    &scene_name,
                                                        const entt::entity child_entity) noexcept {

            auto scene_entity = try_get_scene_entity(scene_name);
            if (scene_entity == entt::null) {
                return false;
            }
            return try_remove_from_scene(registry, scene_entity, child_entity);
        }
        /**
         * @brief 获取场景所有孩子的集合
         * @param [in] registry 注册表
         * @param [in] entity 场景实体
         * @return const std::set<entt::entity>& 场景所有孩子的集合
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryGetSceneChildren(entt::registry &registry, const entt::entity entity)
         */
        [[nodiscard]] static const std::set<entt::entity> &
        get_scene_children(const entt::registry &registry, const entt::entity entity) noexcept {
            assert_msg((registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()),
                       "参数 `registry` 没有 `tnrw::ecs::global_scenes` 组件");
            assert_msg(registry.all_of<scene>(entity),
                       "参数 `entity`（编号为：{}） 没有 `tnrw::ecs::scene` "
                       "组件",
                       static_cast<entt::id_type>(entity));
            return registry.get<scene>(entity).m_children;
        }
        /**
         * @brief 获取场景所有孩子的集合
         * @param [in] registry 注册表
         * @param [in] entity 场景实体
         * @return const std::set<entt::entity>* 指向场景所有孩子的集合的指针
         * @retval nullptr 获取失败时的返回值
         * @note 获取失败不影响原来的组件
         */
        [[nodiscard]] static const std::set<entt::entity> *
        try_get_scene_children(const entt::registry &registry, const entt::entity entity) noexcept {
            if (!registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()) {
                return nullptr;
            }
            if (!registry.all_of<scene>(entity)) {
                return nullptr;
            }
            return &registry.get<scene>(entity).m_children;
        }
        /**
         * @brief 获取场景所有孩子的集合
         * @param [in] registry 注册表
         * @param [in] name 场景名字
         * @return const std::set<entt::entity>& 场景所有孩子的集合
         * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
         * @note 安全版本见 @ref tnrw::ecs::basic_scene_system::tryGetSceneChildren(entt::registry &registry, const key_type &name)
         */
        [[nodiscard]] static const std::set<entt::entity> &
        get_scene_children(const entt::registry &registry, const key_type &name) noexcept {
            entt::entity entity = get_scene_entity(registry, name);
            return get_scene_children(registry, entity);
        }
        /**
         * @brief 获取场景所有孩子的集合
         * @param [in] registry 注册表
         * @param [in] name 场景实体
         * @return const std::set<entt::entity>* 指向场景所有孩子的集合的指针
         * @retval nullptr 获取失败时的返回值
         * @note 获取失败不影响原来的组件
         */
        [[nodiscard]] static const std::set<entt::entity> *
        try_get_scene_children(const entt::registry &registry, const key_type &name) noexcept {
            entt::entity entity = try_get_scene_entity(registry, name);
            if (entity == entt::null) {
                return nullptr;
            }
            return try_get_scene_children(registry, entity);
        }
        /**
         * @brief 获取实体所有父亲的集合
         * @param [in] registry 注册表
         * @param [in] entity 实体
         * @return const std::set<entt::entity>& 实体所有父亲的集合
         */
        [[nodiscard]] static const std::set<entt::entity> &
        get_father_scenes(entt::registry &registry, const entt::entity entity) noexcept {
            create_father_scenes(registry, entity); //< 确保组件已被创建
            return registry.get<father_scenes>(entity).m_fathers;
        }
        /// @cond INTERNAL
      private: /// @privatesection
        /**
          * @brief 创建游戏全局获取场景的组件（即 `global_scenes` ）
          * @param [in] registry 注册表
          */
        static void create_global_scenes(entt::registry &registry) noexcept {
            if (registry.ctx().contains<global_scenes<key_type, key_compare, allocator_type>>()) {
                return;
            }
            registry.ctx().emplace<global_scenes<key_type, key_compare, allocator_type>>();
        }
        /**
         * @brief 创建 `father_scenes` 组件
         * @param [in] registry 注册表
         * @param [in] entity 实体
         */
        static void create_father_scenes(entt::registry &registry, const entt::entity entity) noexcept {
            if (registry.all_of<father_scenes>(entity)) {
                return;
            }
            registry.emplace<father_scenes>(entity);
        }
        /// @endcond
    };

    // using 声明
    using scene_system = basic_scene_system<level_identifier_type>;

} // namespace tnrw::ecs

#endif // TNRW_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP