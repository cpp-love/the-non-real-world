/**
 * @file testSceneSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::SceneSystem` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-09-20
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#include "base/assert_msg.hpp"
#include "ecs/Systems/global/SceneSystem.hpp"
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

    using tnrw::ecs::SceneSystem;
    entt::registry              registry;
    tnrw::level_identifier_type scene1_id = 1;
    std::cout << "创建场景1\n";
    auto [scene1_e, succeeded] = SceneSystem::createScene(registry, scene1_id);
    assert_msg(succeeded, "错误：不应创建失败");
    auto tmp = SceneSystem::createScene(registry, scene1_id);
    assert_msg(!tmp.second, "错误：不应创建二次创建成功");
    assert_msg(scene1_e == SceneSystem::getSceneEntity(registry, scene1_id),
               "错误：场景1不应随获取方式而变化");
    assert_msg(scene1_e == SceneSystem::tryGetSceneEntity(registry, scene1_id),
               "错误：场景1不应随获取方式而变化");
    auto child1 = registry.create();
    std::cout << "将孩子1添加到场景1中\n";
    SceneSystem::addToScene(registry, scene1_id, child1);
    succeeded = SceneSystem::tryAddToScene(registry, scene1_e, child1);
    assert_msg(!succeeded, "错误：不应二次添加成功");
    std::cout << "现在孩子1的父亲有：\n";
    for (auto father : SceneSystem::getFatherScenes(registry, child1)) {
        std::cout << static_cast<entt::id_type>(father) << ' ';
    }
    std::cout << '\n';
    auto child2 = registry.create();
    std::cout << "将孩子2添加到场景1中\n";
    SceneSystem::addToScene(registry, scene1_id, child2);
    succeeded = SceneSystem::tryAddToScene(registry, scene1_e, child2);
    assert_msg(!succeeded, "错误：不应二次添加成功");
    std::cout << "现在场景1的子实体有：\n";
    for (auto child : SceneSystem::getSceneChildren(registry, scene1_id)) {
        std::cout << static_cast<entt::id_type>(child) << ' ';
    }
    std::cout << '\n';
    std::cout << "将孩子1从场景1中删除\n";
    SceneSystem::removeFromScene(registry, scene1_id, child1);
    succeeded = SceneSystem::tryRemoveFromScene(registry, scene1_e, child1);
    assert_msg(!succeeded, "错误：不应二次删除成功");
    std::cout << "现在孩子1的父亲有：\n";
    for (auto father : SceneSystem::getFatherScenes(registry, child1)) {
        std::cout << static_cast<entt::id_type>(father) << ' ';
    }

    assert_msg(&SceneSystem::getSceneChildren(registry, scene1_id)
                   == SceneSystem::tryGetSceneChildren(registry, scene1_e),
               "错误：场景孩子列表不应随获取方式而变化");
    tnrw::level_identifier_type scene2_id = 2;
    std::cout << "创建场景2\n";
    auto [scene2_e, succeeded2] = SceneSystem::createScene(registry, scene2_id);
    assert_msg(succeeded2, "错误：不应创建失败");
    std::cout << "现在registry拥有场景：\n";
    for (auto [id, entity] : SceneSystem::getScenes(registry)) {
        std::cout << "id: " << id << "entity: " << static_cast<entt::id_type>(entity) << '\n';
    }
    std::cout << "销毁场景2\n";
    SceneSystem::eraseScene(registry, scene2_id);
    assert_msg(!registry.valid(scene2_e), "错误：场景2不应仍然存在");
    succeeded = SceneSystem::tryEraseScene(registry, scene2_id);
    assert_msg(!succeeded, "错误：不应二次删除场景2");
    std::cout << "现在registry拥有场景：\n";
    for (auto [id, entity] : SceneSystem::getScenes(registry)) {
        std::cout << "id: " << id << "entity: " << static_cast<entt::id_type>(entity) << '\n';
    }
    std::cout << "删除所有场景\n";
    SceneSystem::clearScenes(registry);
    std::cout << "现在registry拥有场景：\n";
    for (auto [id, entity] : SceneSystem::getScenes(registry)) {
        std::cout << "id: " << id << " entity: " << static_cast<entt::id_type>(entity) << '\n';
    }
    std::cout << std::flush;

    return 0;
}