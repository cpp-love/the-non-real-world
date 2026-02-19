/**
 * @file test_game_state_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::game_state_system` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-09-20
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#include "base/assert_msg.hpp"
#include "ecs/systems/global/game_state_system.hpp"
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

    // using 声明
    using tnrw::ecs::game_state;
    using tnrw::ecs::game_state_system;

    entt::registry        registry; //< 注册表
    [[maybe_unused]] bool notcreated = game_state_system::create_game_state(registry);
    assert_msg(notcreated, "错误：未创建游戏状态");
    notcreated = game_state_system::create_game_state(registry);
    assert_msg(!notcreated, "错误：重复创建游戏状态");
    std::cout << "加入Mainpage状态（代码为1）\n";
    game_state_system::push_top_state(registry, game_state::state::mainpage);
    std::cout << "当前顶状态代码为：" << static_cast<int>(game_state_system::get_top_state(registry))
              << '\n';
    std::cout << "当前所有状态代码为：\n";
    for (const auto &state : game_state_system::get_states(registry)) {
        std::cout << static_cast<int>(state) << ' ';
    }
    std::cout << '\n';
    std::cout << "删除Mainpage状态\n";
    game_state_system::pop_top_state(registry);

    std::cout << "加入Mainpage状态（代码为1）\n";
    [[maybe_unused]] bool succeeded =
        game_state_system::try_push_top_state(registry, game_state::state::mainpage);
    assert_msg(succeeded, "错误：加入状态失败");
    std::cout << "加入Game状态（代码为2）\n";
    succeeded = game_state_system::try_push_top_state(registry, game_state::state::game);
    assert_msg(succeeded, "错误：加入状态失败");
    std::optional<game_state::state> state = game_state_system::try_get_top_state(registry);
    assert_msg(state.has_value(), "错误：获取状态失败");
    std::cout << "当前顶状态代码为：" << static_cast<int>(state.value()) << '\n';
    const std::vector<game_state::state> *states = game_state_system::try_get_states(registry);
    assert_msg(states != nullptr, "错误：获取状态列表失败");
    std::cout << "当前所有状态代码为：\n";
    for (const game_state::state &state : *states) { std::cout << static_cast<int>(state) << ' '; }
    std::cout << '\n';
    std::cout << "删除Game状态\n";
    succeeded = game_state_system::try_pop_top_state(registry);
    assert_msg(succeeded, "错误：删除状态失败");
    std::cout << "删除Mainpage状态\n";
    succeeded = game_state_system::try_pop_top_state(registry);
    assert_msg(succeeded, "错误：删除状态失败");

    return 0;
}