/**
 * @file test_scene_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::scene_system` 的测试用例和使用示例
 * @version 0.1.0-2
 * @date 2026-02-22
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#include "base/assert_msg.hpp"
#include "base/config.hpp"
#include "ecs/systems/global/scene_system.hpp"
#include <entt/entity/registry.hpp>
#include <print>
#include <ranges>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    entt::registry                        registry;
    constexpr tnrw::level_identifier_type scene1_id = 1;

    std::println("创建场景1（id：{}）", scene1_id);
    bool succeeded = tnrw::ecs::scene_system::insert_scene(registry, scene1_id);
    ASSERT_MSG(succeeded, "错误：不应创建失败");
    succeeded = tnrw::ecs::scene_system::insert_scene(registry, scene1_id);
    ASSERT_MSG(!succeeded, "错误：不应创建二次创建成功");
    entt::entity child1 = registry.create();
    std::println("将孩子1（标识符：{}）添加到场景1中", std::to_underlying(child1));
    succeeded = tnrw::ecs::scene_system::insert_to_scene(registry, scene1_id, child1);
    ASSERT_MSG(succeeded, "错误：不应添加失败");
    succeeded = tnrw::ecs::scene_system::insert_to_scene(registry, scene1_id, child1);
    ASSERT_MSG(!succeeded, "错误：不应二次添加成功");
    std::println("现在孩子1的父亲有：{}", tnrw::ecs::scene_system::get_father_scenes(registry, child1));
    entt::entity child2 = registry.create();
    std::println("将孩子2（标识符：{}）添加到场景1中", std::to_underlying(child2));
    succeeded = tnrw::ecs::scene_system::insert_to_scene(registry, scene1_id, child2);
    ASSERT_MSG(succeeded, "错误：不应添加失败");
    succeeded = tnrw::ecs::scene_system::insert_to_scene(registry, scene1_id, child2);
    ASSERT_MSG(!succeeded, "错误：不应二次添加成功");
    std::println(
        "现在场景1的子实体有：{}",
        tnrw::ecs::scene_system::get_scene_children(registry, scene1_id)
            | std::views::transform([](entt::entity entity) { return std::to_underlying(entity); }));
    std::println("将孩子1从场景1中删除");
    succeeded = tnrw::ecs::scene_system::erase_from_scene(registry, scene1_id, child1);
    ASSERT_MSG(succeeded, "错误：不应删除失败");
    succeeded = tnrw::ecs::scene_system::erase_from_scene(registry, scene1_id, child1);
    ASSERT_MSG(!succeeded, "错误：不应二次删除成功");
    std::println("现在孩子1的父亲有：{}", tnrw::ecs::scene_system::get_father_scenes(registry, child1));

    constexpr tnrw::level_identifier_type scene2_id = 2;
    std::println("创建场景2（id：{}）", scene2_id);
    succeeded = tnrw::ecs::scene_system::insert_scene(registry, scene2_id);
    ASSERT_MSG(succeeded, "错误：不应创建失败");
    std::println("现在registry拥有场景：{}",
                 tnrw::ecs::scene_system::get_scenes(registry)
                     | std::views::transform(
                         [](const std::pair<tnrw::level_identifier_type, std::set<entt::entity>> &pair) {
                             return pair.first;
                         }));
    std::println("销毁场景2");
    succeeded = tnrw::ecs::scene_system::erase_scene(registry, scene2_id);
    ASSERT_MSG(succeeded, "错误：不应删除场景2失败");
    succeeded = tnrw::ecs::scene_system::erase_scene(registry, scene2_id);
    ASSERT_MSG(!succeeded, "错误：不应二次删除场景2成功");
    std::println("现在registry拥有场景：{}",
                 tnrw::ecs::scene_system::get_scenes(registry)
                     | std::views::transform(
                         [](const std::pair<tnrw::level_identifier_type, std::set<entt::entity>> &pair) {
                             return pair.first;
                         }));
    std::println("删除所有场景");
    tnrw::ecs::scene_system::clear_scenes(registry);
    std::println("现在registry拥有场景：{}",
                 tnrw::ecs::scene_system::get_scenes(registry)
                     | std::views::transform(
                         [](const std::pair<tnrw::level_identifier_type, std::set<entt::entity>> &pair) {
                             return pair.first;
                         }));

    return 0;
}