/**
 * @file test_render_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::render_system` 的测试用例和使用示例
 * @version 0.1.0-3
 * @date 2026-03-14
 * 
 * @copyright cpp-love
 * 
 * @details 按下 `1` 或 `2` 键以启用不同的场景
 * 
 */

#include "ecs/components/shape_components.hpp"
#include "ecs/systems/global/scene_system.hpp"
#include "ecs/systems/render_system.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <vector>
#ifdef _WIN32

#include <windows.h>
#endif // _WIN32

entt::entity create_shape(entt::registry &registry, tnrw::level_identifier_type level_id,
                          tnrw::ecs::collidable_shape shape) {
    const auto entity = registry.create();
    registry.emplace<tnrw::ecs::collidable_shape>(entity, std::move(shape));
    tnrw::ecs::scene_system::insert_scene(registry, level_id);
    tnrw::ecs::scene_system::insert_to_scene(registry, level_id, entity);
    return entity;
}

constexpr sf::Vector2u                window_size = {800, 800}; ///< 窗口的大小
constexpr tnrw::level_identifier_type first_level = 1;          ///< 第一个关卡
constexpr tnrw::level_identifier_type second_level = 2;         ///< 第二个关卡

int                                   main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    std::vector<entt::entity> line_vec;
    std::vector<entt::entity> circle_vec;
    std::vector<entt::entity> rect_vec;
    sf::RenderWindow          window(sf::VideoMode(window_size), "testRenderSystem");
    entt::registry            registry;

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({30.f, 30.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({20.f, 20.f});
        return tnrw::ecs::collidable_shape{.collision_box = {static_cast<tnrw::ecs::shape::line>(line)},
                                           .render = tnrw::ecs::render_shape{std::move(line)}};
    }()));
    circle_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::circle circle(30.f);
        circle.setPosition({110.f, 30.f});
        circle.setFillColor(sf::Color::Blue);
        return tnrw::ecs::collidable_shape{
            .collision_box = {static_cast<tnrw::ecs::shape::circle>(circle)},
            .render = tnrw::ecs::render_shape{std::move(circle)}};
    }()));
    rect_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::rectangle rect({50.f, 60.f});
        rect.setPosition({400.f, 400.f});
        rect.setFillColor(sf::Color::Yellow);
        return tnrw::ecs::collidable_shape{
            .collision_box = {static_cast<tnrw::ecs::shape::rectangle>(rect)},
            .render = tnrw::ecs::render_shape{std::move(rect)}};
    }()));
    line_vec.push_back(create_shape(registry, second_level, [] {
        tnrw::ecs::render_shape::line line({-100.f, 10.f});
        line.setFillColor(sf::Color::Green);
        line.setPosition({200.f, 90.f});
        return tnrw::ecs::collidable_shape{.collision_box = {static_cast<tnrw::ecs::shape::line>(line)},
                                           .render = tnrw::ecs::render_shape{std::move(line)}};
    }()));
    line_vec.push_back(create_shape(registry, second_level, [] {
        tnrw::ecs::render_shape::line line({-150.f, -500.f});
        line.setFillColor(sf::Color::Blue);
        line.setPosition({550.f, 700.f});
        return tnrw::ecs::collidable_shape{.collision_box = {static_cast<tnrw::ecs::shape::line>(line)},
                                           .render = tnrw::ecs::render_shape{std::move(line)}};
    }()));
    circle_vec.push_back(create_shape(registry, second_level, [] {
        tnrw::ecs::render_shape::circle circle(20.f);
        circle.setPosition({280.f, 280.f});
        circle.setFillColor(sf::Color::Cyan);
        return tnrw::ecs::collidable_shape{
            .collision_box = {static_cast<tnrw::ecs::shape::circle>(circle)},
            .render = tnrw::ecs::render_shape{std::move(circle)}};
    }()));
    rect_vec.push_back(create_shape(registry, second_level, [] {
        tnrw::ecs::render_shape::rectangle rect({50.f, 60.f});
        rect.setPosition({400.f, 400.f});
        rect.setFillColor(sf::Color::Cyan);
        return tnrw::ecs::collidable_shape{
            .collision_box = {static_cast<tnrw::ecs::shape::rectangle>(rect)},
            .render = tnrw::ecs::render_shape{std::move(rect)}};
    }()));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    auto curlevel = first_level;
    while (window.isOpen()) {

        if (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto *keypressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keypressed->code == sf::Keyboard::Key::Num1
                    || keypressed->code == sf::Keyboard::Key::Numpad1) {
                    curlevel = first_level;
                } else if (keypressed->code == sf::Keyboard::Key::Num2
                           || keypressed->code == sf::Keyboard::Key::Numpad2) {
                    curlevel = second_level;
                }
            }
        }

        window.clear();
        tnrw::ecs::render_system::draw(
            registry,
            [&registry, curlevel](entt::entity entity) -> bool {
                const auto &father_scenes = tnrw::ecs::scene_system::get_father_scenes(registry, entity);
                return father_scenes.contains(curlevel);
            },
            window);
        window.display();
    }

    return 0;
}