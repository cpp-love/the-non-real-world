/**
 * @file SceneSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏场景系统
 * @version 0.1.0-1
 * @date 2025-08-16
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/Systems/global/SceneSystem.hpp"
#include "base/assert_msg.hpp"
#include <algorithm>

namespace tnrw {

    namespace ecs {

        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::createFatherScenes(
            entt::registry &registry, const entt::entity entity) noexcept {
            if (registry.all_of<FatherScenes>(entity))
                return;
            registry.emplace<FatherScenes>(entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::createGlobalScenes(
            entt::registry &registry) noexcept {
            if (registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return;
            registry.ctx().emplace<GlobalScenes<key_type, key_compare, allocator_type>>();
        }
        template <typename Key, typename Compare, typename Allocator>
        entt::entity BasicSceneSystem<Key, Compare, Allocator>::createScene(entt::registry &registry,
                                                                            const Key &name) noexcept {
            createGlobalScenes(registry); //< 确保全局状态已被创建
            auto newscene = registry.create();
            registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes[name] =
                newscene;
            return newscene;
        }
        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::eraseScene(entt::registry &registry,
                                                                   const Key      &name) noexcept {
            assert_msg(
                registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>(),
                "参数 `registry` 没有 `tnrw::ecs::GlobalScenes` 组件，请将本消息反馈到项目的 Issue 中");
            assert_msg(registry.all_of<Scene>(getScenes(name)),
                       "参数 `name` 没有 `tnrw::ecs::Scene` 组件，请将本消息反馈到项目的 Issue 中");
            auto &scenes =
                registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes;
            auto scene = scenes[name];
            scenes.erase(scene);
            registry.destroy(scene);
        }
        template <typename Key, typename Compare, typename Allocator>
        bool BasicSceneSystem<Key, Compare, Allocator>::tryEraseScene(entt::registry &registry,
                                                                      const Key      &name) noexcept {
            if (!registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return false;
            if (!registry.all_of<Scene>(getSceneEntity(name)))
                return false;
            auto &scenes =
                registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes;
            auto scene = scenes[name];
            scenes.erase(scene);
            registry.destroy(scene);
            return true;
        }
        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::clearScenes(entt::registry &registry) noexcept {
            createGlobalScenes(registry); //< 确保全局状态已被创建
            auto &scenes =
                registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes;
            for (auto entity : scenes) { registry.destroy(entity); }
            scenes.clear();
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] const std::vector<entt::entity> &
        BasicSceneSystem<Key, Compare, Allocator>::getScenes(const entt::registry &registry) noexcept {
            createGlobalScenes(registry); //< 确保全局状态已被创建
            return registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes;
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] entt::entity
        BasicSceneSystem<Key, Compare, Allocator>::getSceneEntity(const entt::registry &registry,
                                                                  const Key            &name) noexcept {
            assert_msg(
                registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>(),
                "参数 `registry` 没有 `tnrw::ecs::GlobalScenes` 组件，请将本消息反馈到项目的 Issue 中");
            assert_msg(
                registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes.find(
                    name)
                    != registry.ctx()
                           .get<GlobalScenes<key_type, key_compare, allocator_type>>()
                           .scenes.end(),
                "参数 `name` 没有 `tnrw::ecs::Scene` 组件，请将本消息反馈到项目的 Issue 中");
            return registry.ctx()
                .get<GlobalScenes<key_type, key_compare, allocator_type>>()
                .scenes[name];
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] entt::entity
        BasicSceneSystem<Key, Compare, Allocator>::tryGetSceneEntity(const entt::registry &registry,
                                                                     const Key &name) noexcept {
            if (!registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return entt::null;
            if (!registry.ctx().get<GlobalScenes<key_type, key_compare, allocator_type>>().scenes.find(
                    name)
                != registry.ctx()
                       .get<GlobalScenes<key_type, key_compare, allocator_type>>()
                       .scenes.end())
                return entt::null;
            return registry.ctx()
                .get<GlobalScenes<key_type, key_compare, allocator_type>>()
                .scenes[name];
        }
        template <typename Key, typename Compare, typename Allocator>
        void
        BasicSceneSystem<Key, Compare, Allocator>::addToScene(entt::registry    &registry,
                                                              const entt::entity scene_entity,
                                                              const entt::entity child_entity) noexcept {
            assert_msg(
                registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>(),
                "参数 `registry` 没有 `tnrw::ecs::GlobalScenes` 组件，请将本消息反馈到项目的 Issue 中");
            assert_msg(registry.all_of<Scene>(scene_entity),
                       "参数 `name` 没有 `tnrw::ecs::Scene` 组件，请将本消息反馈到项目的 Issue 中");

            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            registry.get<Scene>(scene_entity).children.insert(child_entity);
            registry.get<FatherScenes>(child_entity).fathers.insert(scene_entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::addToScene(
            entt::registry &registry, const Key &scene_name, const entt::entity child_entity) noexcept {
            auto scene_entity = getSceneEntity(registry, scene_name);
            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            registry.get<Scene>(scene_entity).children.insert(child_entity);
            registry.get<FatherScenes>(child_entity).fathers.insert(scene_entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        bool BasicSceneSystem<Key, Compare, Allocator>::tryAddToScene(
            entt::registry &registry, const entt::entity scene_entity,
            const entt::entity child_entity) noexcept {
            if (!registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return false;
            if (!registry.all_of<Scene>(scene_entity))
                return false;

            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            registry.get<Scene>(scene_entity).children.insert(child_entity);
            registry.get<FatherScenes>(child_entity).fathers.insert(scene_entity);
            return true;
        }
        template <typename Key, typename Compare, typename Allocator>
        bool BasicSceneSystem<Key, Compare, Allocator>::tryAddToScene(
            entt::registry &registry, const Key &scene_name, const entt::entity child_entity) noexcept {
            auto scene_entity = tryGetSceneEntity(registry, scene_name);
            if (scene_entity == entt::null)
                return false;
            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            registry.get<Scene>(scene_entity).children.insert(child_entity);
            registry.get<FatherScenes>(child_entity).fathers.insert(scene_entity);
            return true;
        }
        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::removeFromScene(
            entt::registry &registry, const entt::entity scene_entity,
            const entt::entity child_entity) noexcept {
            assert_msg(
                registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>(),
                "参数 `registry` 没有 `tnrw::ecs::GlobalScenes` 组件，请将本消息反馈到项目的 Issue 中");
            assert_msg(registry.all_of<Scene>(scene_entity),
                       "参数 `name` 没有 `tnrw::ecs::Scene` 组件，请将本消息反馈到项目的 Issue 中");
            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            auto &children = registry.get<Scene>(scene_entity).children;
            auto &fathers = registry.get<FatherScenes>(child_entity).fathers;
            children.erase(child_entity);
            fathers.erase(scene_entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        void BasicSceneSystem<Key, Compare, Allocator>::removeFromScene(
            entt::registry &registry, const Key &scene_name, const entt::entity child_entity) noexcept {
            assert_msg(
                registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>(),
                "参数 `registry` 没有 `tnrw::ecs::GlobalScenes` 组件，请将本消息反馈到项目的 Issue 中");
            auto scene_entity = getSceneEntity(scene_name);
            assert_msg(registry.all_of<Scene>(scene_entity),
                       "参数 `name` 没有 `tnrw::ecs::Scene` 组件，请将本消息反馈到项目的 Issue 中");
            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            auto &children = registry.get<Scene>(scene_entity).children;
            auto &fathers = registry.get<FatherScenes>(child_entity).fathers;
            children.erase(child_entity);
            fathers.erase(scene_entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        bool BasicSceneSystem<Key, Compare, Allocator>::tryRemoveFromScene(
            entt::registry &registry, const entt::entity scene_entity,
            const entt::entity child_entity) noexcept {
            if (!registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return false;
            if (!registry.all_of<Scene>(scene_entity))
                return false;
            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            auto &children = registry.get<Scene>(scene_entity).children;
            auto &fathers = registry.get<FatherScenes>(child_entity).fathers;
            children.erase(child_entity);
            fathers.erase(scene_entity);
            return true;
        }
        template <typename Key, typename Compare, typename Allocator>
        bool BasicSceneSystem<Key, Compare, Allocator>::tryRemoveFromScene(
            entt::registry &registry, const Key &scene_name, const entt::entity child_entity) noexcept {
            if (!registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return false;
            auto scene_entity = getSceneEntity(scene_name);
            if (!registry.all_of<Scene>(scene_entity))
                return false;
            createFatherScenes(registry, child_entity); //< 确保组件已被创建
            auto &children = registry.get<Scene>(scene_entity).children;
            auto &fathers = registry.get<FatherScenes>(child_entity).fathers;
            children.erase(child_entity);
            fathers.erase(scene_entity);
            return true;
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] const std::set<entt::entity> &
        BasicSceneSystem<Key, Compare, Allocator>::getSceneChildren(const entt::registry &registry,
                                                                    const entt::entity entity) noexcept {
            assert_msg(
                registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>(),
                "参数 `registry` 没有 `tnrw::ecs::GlobalScenes` 组件，请将本消息反馈到项目的 Issue 中");
            assert_msg(registry.all_of<Scene>(entity),
                       "参数 `name` 没有 `tnrw::ecs::Scene` 组件，请将本消息反馈到项目的 Issue 中");
            return registry.get<Scene>(entity).children;
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] const std::set<entt::entity> &
        BasicSceneSystem<Key, Compare, Allocator>::getSceneChildren(const entt::registry &registry,
                                                                    const Key &name) noexcept {
            entt::entity entity = getSceneEntity(registry, name);
            return getSceneChildren(registry, entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] const std::set<entt::entity> *
        BasicSceneSystem<Key, Compare, Allocator>::tryGetSceneChildren(
            const entt::registry &registry, const entt::entity entity) noexcept {
            if (!registry.ctx().contains<GlobalScenes<key_type, key_compare, allocator_type>>())
                return nullptr;
            if (!registry.all_of<Scene>(entity))
                return nullptr;
            return &registry.get<Scene>(entity).children;
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] const std::set<entt::entity> *
        BasicSceneSystem<Key, Compare, Allocator>::tryGetSceneChildren(const entt::registry &registry,
                                                                       const Key &name) noexcept {
            entt::entity entity = tryGetSceneEntity(registry, name);
            if (entity == entt::null)
                return nullptr;
            return &getSceneChildren(registry, entity);
        }
        template <typename Key, typename Compare, typename Allocator>
        [[nodiscard]] const std::set<entt::entity> &
        BasicSceneSystem<Key, Compare, Allocator>::getFatherScenes(const entt::registry &registry,
                                                                   const entt::entity entity) noexcept {
            createFatherScenes(registry, entity); //< 确保组件已被创建
            return registry.get<FatherScenes>(entity).fathers;
        }

        template class BasicSceneSystem<TNRW_GAME_LEVEL_TYPE>;

    } // namespace ecs

} // namespace tnrw