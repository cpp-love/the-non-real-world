/**
 * @file test_game_state_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::GameStateSystem` 的测试用例和使用示例
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
    using tnrw::ecs::GameState;
    using tnrw::ecs::GameStateSystem;

    entt::registry        registry; //< 注册表
    [[maybe_unused]] bool notcreated = GameStateSystem::createGameState(registry);
    assert_msg(notcreated, "错误：未创建游戏状态");
    notcreated = GameStateSystem::createGameState(registry);
    assert_msg(!notcreated, "错误：重复创建游戏状态");
    std::cout << "加入Mainpage状态（代码为1）\n";
    GameStateSystem::pushTopState(registry, GameState::State::Mainpage);
    std::cout << "当前顶状态代码为：" << static_cast<int>(GameStateSystem::getTopState(registry))
              << '\n';
    std::cout << "当前所有状态代码为：\n";
    for (const auto &state : GameStateSystem::getStates(registry)) {
        std::cout << static_cast<int>(state) << ' ';
    }
    std::cout << '\n';
    std::cout << "删除Mainpage状态\n";
    GameStateSystem::popTopState(registry);

    std::cout << "加入Mainpage状态（代码为1）\n";
    [[maybe_unused]] bool succeeded =
        GameStateSystem::tryPushTopState(registry, GameState::State::Mainpage);
    assert_msg(succeeded, "错误：加入状态失败");
    std::cout << "加入Game状态（代码为2）\n";
    succeeded = GameStateSystem::tryPushTopState(registry, GameState::State::Game);
    assert_msg(succeeded, "错误：加入状态失败");
    std::optional<GameState::State> state = GameStateSystem::tryGetTopState(registry);
    assert_msg(state.has_value(), "错误：获取状态失败");
    std::cout << "当前顶状态代码为：" << static_cast<int>(state.value()) << '\n';
    const std::vector<GameState::State> *states = GameStateSystem::tryGetStates(registry);
    assert_msg(states != nullptr, "错误：获取状态列表失败");
    std::cout << "当前所有状态代码为：\n";
    for (const GameState::State &state : *states) { std::cout << static_cast<int>(state) << ' '; }
    std::cout << '\n';
    std::cout << "删除Game状态\n";
    succeeded = GameStateSystem::tryPopTopState(registry);
    assert_msg(succeeded, "错误：删除状态失败");
    std::cout << "删除Mainpage状态\n";
    succeeded = GameStateSystem::tryPopTopState(registry);
    assert_msg(succeeded, "错误：删除状态失败");

    return 0;
}