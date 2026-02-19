/**
 * @file test_scene_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::scene_system` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-09-20
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#include "base/assert_msg.hpp"
#include "ecs/systems/global/scene_system.hpp"
#include <entt/entt.hpp>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    using tnrw::ecs::scene_system;
    entt::registry              registry;
    tnrw::level_identifier_type scene1_id = 1;
    std::cout << "创建场景1\n";
    auto scene1_res = scene_system::create_scene(registry, scene1_id);
    assert_msg(scene1_res.has_value(), "错误：不应创建失败");
    auto scene1_e = scene1_res.value();
    auto tmp = scene_system::create_scene(registry, scene1_id);
    assert_msg(!tmp.has_value(), "错误：不应创建二次创建成功");
    assert_msg(scene1_e == scene_system::get_scene_entity(registry, scene1_id),
               "错误：场景1不应随获取方式而变化");
    assert_msg(scene1_e == scene_system::try_get_scene_entity(registry, scene1_id),
               "错误：场景1不应随获取方式而变化");
    auto child1 = registry.create();
    std::cout << "将孩子1添加到场景1中\n";
    scene_system::add_to_scene(registry, scene1_id, child1);
    bool succeeded = scene_system::try_add_to_scene(registry, scene1_e, child1);
    assert_msg(!succeeded, "错误：不应二次添加成功");
    std::cout << "现在孩子1的父亲有：\n";
    for (auto father : scene_system::get_father_scenes(registry, child1)) {
        std::cout << static_cast<entt::id_type>(father) << ' ';
    }
    std::cout << '\n';
    auto child2 = registry.create();
    std::cout << "将孩子2添加到场景1中\n";
    scene_system::add_to_scene(registry, scene1_id, child2);
    succeeded = scene_system::try_add_to_scene(registry, scene1_e, child2);
    assert_msg(!succeeded, "错误：不应二次添加成功");
    std::cout << "现在场景1的子实体有：\n";
    for (auto child : scene_system::get_scene_children(registry, scene1_id)) {
        std::cout << static_cast<entt::id_type>(child) << ' ';
    }
    std::cout << '\n';
    std::cout << "将孩子1从场景1中删除\n";
    scene_system::remove_from_scene(registry, scene1_id, child1);
    succeeded = scene_system::try_remove_from_scene(registry, scene1_e, child1);
    assert_msg(!succeeded, "错误：不应二次删除成功");
    std::cout << "现在孩子1的父亲有：\n";
    for (auto father : scene_system::get_father_scenes(registry, child1)) {
        std::cout << static_cast<entt::id_type>(father) << ' ';
    }

    assert_msg(&scene_system::get_scene_children(registry, scene1_id)
                   == scene_system::try_get_scene_children(registry, scene1_e),
               "错误：场景孩子列表不应随获取方式而变化");
    tnrw::level_identifier_type scene2_id = 2;
    std::cout << "创建场景2\n";
    auto scene2_res = scene_system::create_scene(registry, scene2_id);
    assert_msg(scene2_res.has_value(), "错误：不应创建失败");
    auto scene2_e = scene2_res.value();
    std::cout << "现在registry拥有场景：\n";
    for (auto [identity, entity] : scene_system::get_scenes(registry)) {
        std::cout << "id: " << identity << "entity: " << static_cast<entt::id_type>(entity) << '\n';
    }
    std::cout << "销毁场景2\n";
    scene_system::erase_scene(registry, scene2_id);
    assert_msg(!registry.valid(scene2_e), "错误：场景2不应仍然存在");
    succeeded = scene_system::try_erase_scene(registry, scene2_id);
    assert_msg(!succeeded, "错误：不应二次删除场景2");
    std::cout << "现在registry拥有场景：\n";
    for (auto [identity, entity] : scene_system::get_scenes(registry)) {
        std::cout << "id: " << identity << "entity: " << static_cast<entt::id_type>(entity) << '\n';
    }
    std::cout << "删除所有场景\n";
    scene_system::clear_scenes(registry);
    std::cout << "现在registry拥有场景：\n";
    for (auto [identity, entity] : scene_system::get_scenes(registry)) {
        std::cout << "id: " << identity << " entity: " << static_cast<entt::id_type>(entity) << '\n';
    }
    std::cout << std::flush;

    return 0;
}