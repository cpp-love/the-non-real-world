/**
 * @file test_movement_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::movement_system` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-11-16
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/global/scene_system.hpp"
#include "ecs/systems/movement_system.hpp"
#include "ecs/systems/render_system.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <entt/entt.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

using tnrw::ecs::movement_system;
using tnrw::ecs::render_system;
using tnrw::ecs::scene_system;

entt::entity create_shape(entt::registry &registry, tnrw::level_identifier_type level_id,
                          tnrw::ecs::shape shape) {
    const auto entity = registry.create();
    spdlog::info("create shape entity: {}", static_cast<entt::id_type>(entity));
    registry.emplace<tnrw::ecs::shape>(entity, shape);
    registry.emplace<tnrw::ecs::should_render>(entity);
    registry.emplace<tnrw::ecs::should_collide>(entity);
    scene_system::create_scene(registry, level_id);
    scene_system::add_to_scene(registry, level_id, entity);
    return entity;
}

constexpr sf::Vector2u                window_size = {800, 600};    ///< 窗口的大小
constexpr tnrw::level_identifier_type first_level = 1;             ///< 第一个关卡
constexpr tnrw::level_identifier_type second_level = 2;            ///< 第二个关卡
constexpr float                       velocity_no_direction = 3.f; ///< 没有方向的速度

int                                   main() {

    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_st>("log/log.log");
    file_logger->set_level(spdlog::level::trace);
    auto stdout_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
    stdout_logger->set_level(spdlog::level::info);
    auto multi_sink_logger = std::make_shared<spdlog::logger>(
        "multi_sink_logger", spdlog::sinks_init_list{file_logger, stdout_logger});
    multi_sink_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(multi_sink_logger);
    spdlog::flush_on(spdlog::level::info);
    sf::Clock                 clock;
    std::vector<entt::entity> line_vec;
    std::vector<entt::entity> circle_vec;
    sf::RenderWindow          window(sf::VideoMode(window_size), "testMovementSystem");
    entt::registry            registry;

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    circle_vec.push_back(create_shape(registry, first_level,
                                                                        {tnrw::ecs::shape::circle{.center = {30.f, 100.f},
                                                                                                  .radius = 30.f,
                                                                                                  .fill_color = sf::Color::Blue}}));
    line_vec.push_back(create_shape(
        registry, first_level,
        {tnrw::ecs::shape::line{.start = {.position = {20.f, 20.f}, .color = sf::Color::Red},
                                                                  .end = {.position = {50.f, 50.f}, .color = sf::Color::Red}}}));
    line_vec.push_back(create_shape(
        registry, first_level,
        {tnrw::ecs::shape::line{.start = {.position = {50.f, 50.f}, .color = sf::Color::Red},
                                                                  .end = {.position = {100.f, 40.f}, .color = sf::Color::Blue}}}));
    line_vec.push_back(create_shape(
        registry, first_level,
        {tnrw::ecs::shape::line{.start = {.position = {100.f, 40.f}, .color = sf::Color::Blue},
                                                                  .end = {.position = {200.f, 100.f}, .color = sf::Color::Green}}}));
    line_vec.push_back(create_shape(
        registry, first_level,
        {tnrw::ecs::shape::line{.start = {.position = {200.f, 100.f}, .color = sf::Color::Green},
                                                                  .end = {.position = {500.f, 300.f}, .color = sf::Color::Green}}}));
    line_vec.push_back(create_shape(
        registry, first_level,
        {tnrw::ecs::shape::line{.start = {.position = {500.f, 300.f}, .color = sf::Color::Green},
                                                                  .end = {.position = {600.f, 600.f}, .color = sf::Color::Green}}}));
    line_vec.push_back(create_shape(
        registry, first_level,
        {tnrw::ecs::shape::line{.start = {.position = {600.f, 600.f}, .color = sf::Color::Green},
                                                                  .end = {.position = {650.f, 400.f}, .color = sf::Color::Green}}}));
    line_vec.push_back(create_shape(
        registry, second_level,
        {tnrw::ecs::shape::line{
                                              .start = {.position = {200.f, 90.f}, .color = sf::Color::Green},
                                              .end = {.position = {100.f, 100.f}, .color = sf::Color::Blue}}})); //< 这个组件不会参与碰撞
    circle_vec.push_back(create_shape(
        registry, second_level,
        {tnrw::ecs::shape::circle{.center = {300.f, 300.f},
                                                                    .radius = 20.f,
                                                                    .fill_color = sf::Color::Cyan}})); //< 这个组件也不会参与碰撞
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    auto curlevel = first_level;
    bool is_running = true;
    while (window.isOpen()) {
        if (std::optional event = window.pollEvent()) {
            // 基本事件处理
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (event->is<sf::Event::FocusGained>()) {
                is_running = true;
            } else if (event->is<sf::Event::FocusLost>()) {
                is_running = false;
            }
        }

        if (is_running) {
            // 移动处理
            movement_system::velocity(registry, circle_vec[0]) = {0.f, 0.f};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                movement_system::velocity(registry, circle_vec[0]) += {0.f, -velocity_no_direction};
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                movement_system::velocity(registry, circle_vec[0]) += {-velocity_no_direction, 0.f};
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                movement_system::velocity(registry, circle_vec[0]) += {0.f, velocity_no_direction};
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                movement_system::velocity(registry, circle_vec[0]) += {velocity_no_direction, 0.f};
            }
        }

        auto delta_time = clock.restart();
        movement_system::update_with_velocity(
            registry, circle_vec[0],
            std::chrono::duration_cast<std::chrono::milliseconds>(delta_time.toDuration()));

        window.clear();
        render_system::draw(registry, window, [&registry, curlevel](entt::entity entity) -> bool {
            auto father_scenes = scene_system::get_father_scenes(registry, entity);
            return father_scenes.contains(scene_system::get_scene_entity(registry, curlevel));
        });
        window.display();
    }

    return 0;
}