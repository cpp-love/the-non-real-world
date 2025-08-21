/**
 * @file SceneSystem.hpp
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

#ifndef __ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP__
#define __ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP__

#include "base/config.hpp"
#include "ecs/Components/global/ScenesComponents.hpp"
#include <algorithm>
#include <entt/entt.hpp>
#include <functional>

namespace tnrw {

    namespace ecs {

        /**
         * @brief 基本游戏场景系统
         * @tparam Key 键类型
         * @tparam Compare 可选的比较类型
         * @tparam std::allocator<std ::pair<const Key, entt::entity>> 可选的分配器类型
         */
        template <typename Key, typename Compare = std::less<Key>,
                  typename Allocator = std::allocator<std ::pair<const Key, entt::entity>>>
        class BasicSceneSystem {
          public: /// @publicsection
            // using 声明
            using key_type = Key;             //< 键类型
            using key_compare = Compare;      //< 键比较类型
            using allocator_type = Allocator; //< 分配器类型

            /**
             * @brief 创建一个场景实体
             * @param [in] registry 注册表
             * @param [in] name 场景名字
             * @return entt::entity 场景的实体
             */
            static entt::entity createScene(entt::registry &registry, const Key &name) noexcept;
            /**
             * @brief 删除一个场景实体
             * @param [in] registry 注册表
             * @param [in] name 场景名字
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryEraseScene(entt::registry &registry, const Key &name)
             */
            static void eraseScene(entt::registry &registry, const Key &name) noexcept;
            /**
             * @brief 删除一个场景实体（不删除子实体）
             * @param [in] registry 注册表
             * @param [in] name 场景名字
             * @return true 删除成功
             * @return false 删除失败
             * @note 删除失败不影响原来的组件
             */
            static bool tryEraseScene(entt::registry &registry, const Key &name) noexcept;
            /**
             * @brief 清除所有的场景实体（不删除子实体）
             * @param [in] registry 注册表
             */
            static void clearScenes(entt::registry &registry) noexcept;
            /**
             * @brief 获取所有场景实体的列表
             * @param [in] registry 注册表
             * @return const std::vector<entt::entity>& 所有的场景实体
             */
            [[nodiscard]] static const std::vector<entt::entity> &
            getScenes(const entt::registry &registry) noexcept;
            /**
             * @brief 获取场景名字所对应的实体
             * @param [in] registry 注册表
             * @param [in] name 场景名字
             * @return entt::entity 场景名字所对应的实体
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryGetSceneEntity(entt::registry &registry, const Key &name)
             */
            [[nodiscard]] static entt::entity getSceneEntity(const entt::registry &registry,
                                                             const Key            &name) noexcept;
            /**
             * @brief 获取场景名字所对应的实体
             * @param [in] registry 注册表
             * @param [in] name 注册表
             * @return entt::entity 场景名字所对应的实体，
             * @retval entt::null 如果没有对应的实体时返回此值
             * @note 获取失败不影响原来的组件
             */
            [[nodiscard]] static entt::entity tryGetSceneEntity(const entt::registry &registry,
                                                                const Key            &name) noexcept;
            /**
             * @brief 将子实体加入场景中
             * @param [in] registry 注册表
             * @param [in] scene_entity 场景实体
             * @param [in] child_entity 子实体
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryAddToScene(entt::registry &registry, const entt::entity scene_entity, const entt::entity child_entity)
             */
            static void addToScene(entt::registry &registry, const entt::entity scene_entity,
                                   const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体加入场景中
             * @param [in] registry 注册表
             * @param [in] scene_name 场景名字
             * @param [in] child_entity 子实体
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryAddToScene(entt::registry &registry, const entt::entity scene_entity, const entt::entity child_entity)
             */
            static void addToScene(entt::registry &registry, const Key &scene_name,
                                   const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体加入场景中
             * @param [in] registry 注册表
             * @param [in] scene_entity 场景实体
             * @param [in] child_entity 子实体
             * @return true 加入成功
             * @return false 加入失败
             * @note 加入失败不影响原来的组件
             */
            static bool tryAddToScene(entt::registry &registry, const entt::entity scene_entity,
                                      const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体加入场景中
             * @param [in] registry 注册表
             * @param [in] scene_name 场景名字
             * @param [in] child_entity 子实体
             * @return true 加入成功
             * @return false 加入失败
             * @note 加入失败不影响原来的组件
             */
            static bool tryAddToScene(entt::registry &registry, const Key &scene_name,
                                      const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体从场景中移除
             * @param [in] registry 注册表
             * @param [in] scene_entity 场景实体
             * @param [in] child_entity 子实体
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryRemoveFromScene(entt::registry &registry, const entt::entity scene_entity, const entt::entity child_entity)
             */
            static void removeFromScene(entt::registry &registry, const entt::entity scene_entity,
                                        const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体从场景中移除
             * @param [in] registry 注册表
             * @param [in] scene_name 场景名字
             * @param [in] child_entity 子实体
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryRemoveFromScene(entt::registry &registry, const entt::entity scene_entity, const entt::entity child_entity)
             */
            static void removeFromScene(entt::registry &registry, const Key &scene_name,
                                        const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体从场景中移除
             * @param [in] registry 注册表
             * @param [in] scene_entity 场景实体
             * @param [in] child_entity 子实体
             * @return true 移除成功
             * @return false 移除失败
             * @note 移除失败不影响原来的组件
             */
            static bool tryRemoveFromScene(entt::registry &registry, const entt::entity scene_entity,
                                           const entt::entity child_entity) noexcept;
            /**
             * @brief 将子实体从场景中移除
             * @param [in] registry 注册表
             * @param [in] scene_name 场景名字
             * @param [in] child_entity 子实体
             * @return true 移除成功
             * @return false 移除失败
             * @note 移除失败不影响原来的组件
             */
            static bool tryRemoveFromScene(entt::registry &registry, const Key &scene_name,
                                           const entt::entity child_entity) noexcept;
            /**
             * @brief 获取场景所有孩子的集合
             * @param [in] registry 注册表
             * @param [in] entity 场景实体
             * @return const std::set<entt::entity>& 场景所有孩子的集合
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryGetSceneChildren(entt::registry &registry, const entt::entity entity)
             */
            [[nodiscard]] static const std::set<entt::entity> &
            getSceneChildren(const entt::registry &registry, const entt::entity entity) noexcept;
            /**
             * @brief 获取场景所有孩子的集合
             * @param [in] registry 注册表
             * @param [in] entity 场景实体
             * @return const std::set<entt::entity>* 指向场景所有孩子的集合的指针
             * @retval nullptr 获取失败时的返回值
             * @note 获取失败不影响原来的组件
             */
            [[nodiscard]] static const std::set<entt::entity> *
            tryGetSceneChildren(const entt::registry &registry, const entt::entity entity) noexcept;
            /**
             * @brief 获取场景所有孩子的集合
             * @param [in] registry 注册表
             * @param [in] name 场景名字
             * @return const std::set<entt::entity>& 场景所有孩子的集合
             * @warning 此函数不安全，如果注册表没有组件，可能会断言错误
             * @note 安全版本见 @ref tnrw::ecs::BasicSceneSystem::tryGetSceneChildren(entt::registry &registry, const entt::entity entity)
             */
            [[nodiscard]] static const std::set<entt::entity> &
            getSceneChildren(const entt::registry &registry, const Key &name) noexcept;
            /**
             * @brief 获取场景所有孩子的集合
             * @param [in] registry 注册表
             * @param [in] name 场景实体
             * @return const std::set<entt::entity>* 指向场景所有孩子的集合的指针
             * @retval nullptr 获取失败时的返回值
             * @note 获取失败不影响原来的组件
             */
            [[nodiscard]] static const std::set<entt::entity> *
            tryGetSceneChildren(const entt::registry &registry, const Key &name) noexcept;
            /**
             * @brief 获取实体所有父亲的集合
             * @param [in] registry 注册表
             * @param [in] entity 实体
             * @return const std::set<entt::entity>& 实体所有父亲的集合
             */
            [[nodiscard]] static const std::set<entt::entity> &
            getFatherScenes(const entt::registry &registry, const entt::entity entity) noexcept;
            /// @cond INTERNAL
          private: /// @privatesection
            /**
              * @brief 创建游戏全局获取场景的组件（即 `GlobalScenes` ）
              * @param [in] registry 注册表
              */
            static void createGlobalScenes(entt::registry &registry) noexcept;
            /**
             * @brief 创建 `FatherScenes` 组件
             * @param [in] registry 注册表
             * @param [in] entity 实体
             */
            static void createFatherScenes(entt::registry &registry, const entt::entity entity) noexcept;
            /// @endcond
        };

        // using 声明
        using SceneSystem = BasicSceneSystem<TNRW_GAME_LEVEL_TYPE>;

    } // namespace ecs

} // namespace tnrw

#endif // __ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP__