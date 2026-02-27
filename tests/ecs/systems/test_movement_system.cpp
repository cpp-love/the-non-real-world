/**
 * @file test_movement_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::movement_system` 的测试用例和使用示例
 * @version 0.1.0-2
 * @date 2027-02-27
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
#ifdef _WIN32

#include <windows.h>
#endif // _WIN32

using tnrw::ecs::movement_system;
using tnrw::ecs::render_system;
using tnrw::ecs::scene_system;

entt::entity create_shape(entt::registry &registry, tnrw::level_identifier_type level_id,
                          tnrw::ecs::render_shape shape) {
    const auto entity = registry.create();
    spdlog::info("create shape entity: {}", static_cast<entt::id_type>(entity));
    registry.emplace<tnrw::ecs::render_shape>(entity, shape);
    tnrw::ecs::scene_system::insert_scene(registry, level_id);
    tnrw::ecs::scene_system::insert_to_scene(registry, level_id, entity);
    return entity;
}

constexpr sf::Vector2u                window_size = {800, 600};    ///< 窗口的大小
constexpr tnrw::level_identifier_type first_level = 1;             ///< 第一个关卡
constexpr tnrw::level_identifier_type second_level = 2;            ///< 第二个关卡
constexpr float                       velocity_no_direction = 1.f; ///< 没有方向的速度

int                                   main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_st>("log/log.log");
    file_logger->set_level(spdlog::level::trace);
    auto stdout_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
    stdout_logger->set_level(spdlog::level::info);
    auto multi_sink_logger = std::make_shared<spdlog::logger>(
        "multi_sink_logger", spdlog::sinks_init_list{file_logger, stdout_logger});
    multi_sink_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(multi_sink_logger);
    spdlog::flush_on(spdlog::level::info);

    std::vector<entt::entity> line_vec;
    std::vector<entt::entity> circle_vec;
    sf::RenderWindow          window(sf::VideoMode(window_size), "testMovementSystem");
    entt::registry            registry;

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    circle_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::circle circle(30.f);
        circle.setPosition({0.f, 70.f});
        circle.setFillColor(sf::Color::Blue);
        return tnrw::ecs::render_shape{std::move(circle)};
    }()));
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({30.f, 30.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({20.f, 20.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }()));
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({50.f, -10.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({50.f, 50.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }()));
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({100.f, 60.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({100.f, 40.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }()));
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({300.f, 200.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({200.f, 100.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }()));
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({100.f, 300.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({500.f, 300.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }()));
    line_vec.push_back(create_shape(registry, first_level, [] {
        tnrw::ecs::render_shape::line line({50.f, -200.f});
        line.setFillColor(sf::Color::Red);
        line.setPosition({600.f, 600.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }()));
    line_vec.push_back(create_shape(registry, second_level, [] {
        tnrw::ecs::render_shape::line line({-100.f, 10.f});
        line.setFillColor(sf::Color::Green);
        line.setPosition({200.f, 90.f});
        return tnrw::ecs::render_shape{std::move(line)};
    }())); //< 这个组件不会参与碰撞
    circle_vec.push_back(create_shape(registry, second_level, [] {
        tnrw::ecs::render_shape::circle circle(20.f);
        circle.setPosition({300.f, 300.f});
        circle.setFillColor(sf::Color::Cyan);
        return tnrw::ecs::render_shape{std::move(circle)};
    }())); //< 这个组件也不会参与碰撞
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    auto curlevel = first_level;
    bool is_running = true;
    auto prev = tnrw::ecs::clock::now();
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

        auto cur = tnrw::ecs::clock::now();
        movement_system::update_with_velocity(registry, circle_vec[0], cur - prev);
        prev = cur;

        window.clear();
        render_system::draw(
            registry,
            [&registry, curlevel](entt::entity entity) -> bool {
                const auto &father_scenes = scene_system::get_father_scenes(registry, entity);
                return father_scenes.contains(curlevel);
            },
            window);
        window.display();
    }

    return 0;
}