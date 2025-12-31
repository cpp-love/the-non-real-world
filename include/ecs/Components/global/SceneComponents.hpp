/**
 * @file SceneComponents.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了全局登记场景的组件、场景组件和获取父场景实体的组件
 * @version 0.1.0-1
 * @date 2025-08-14
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 使用 `std::map<Key, entt::entity, Compare, Alloc>` 来存储全局场景的登记，其中 `Key` ,  `Compare` ,  `Alloc` 为模板参数
 * - 使用 `std:vector<entt::entity>` 来存储场景的子实体的登记
 * - 使用 `std:vector<entt::entity>` 来存储父场景实体
 * 
 */

#ifndef TNRW_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP

#include <entt/fwd.hpp>
#include <functional>
#include <map>
#include <set>

namespace tnrw::ecs {

    template <typename Key, typename Compare, typename Allocator>
    class BasicSceneSystem;

    /// @brief 全局获取场景的组件
    template <typename Key, typename Compare = std::less<Key>,
              typename Allocator = std::allocator<std ::pair<const Key, entt::entity>>>
    class [[nodiscard]] GlobalScenes {
      public: /// @publicsection
        // using 声明
        using KeyType = Key;             //< 键类型
        using KeyCompare = Compare;      //< 键比较类型
        using AllocatorType = Allocator; //< 分配器类型
        // 友元声明
        friend class BasicSceneSystem<Key, Compare, Allocator>;

        /// @cond INTERNAL
      private: /// @privatesection
        // 成员
        std::map<KeyType, entt::entity, KeyCompare, AllocatorType> m_scenes; //< 场景实体列表
        /// @endcond
    };

    /// @brief 场景组件
    class [[nodiscard]] Scene {
      public: /// @publicsection
        // 友元声明
        template <typename Key, typename Compare, typename Allocator>
        friend class BasicSceneSystem;
        /// @cond INTERNAL
      private: /// @privatesection
        // 成员
        std::set<entt::entity> m_children; //< 子实体列表
        /// @endcond
    };

    /// @brief 获取父场景实体的组件
    class [[nodiscard]] FatherScenes {
      public: /// @publicsection
        // 友元声明
        template <typename Key, typename Compare, typename Allocator>
        friend class BasicSceneSystem;
        /// @cond INTERNAL
      private: /// @privatesection
        // 成员
        std::set<entt::entity> m_fathers; //< 父亲实体列表
        /// @endcond
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP