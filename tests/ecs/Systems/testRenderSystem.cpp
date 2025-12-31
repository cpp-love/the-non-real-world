/**
 * @file testRenderSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::ecs::RenderSystem` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-10-19
 * 
 * @copyright cpp-love
 * 
 * @details 按下 `1` 或 `2` 键以启用不同的场景
 * 
 */

#include "ecs/Systems/RenderSystem.hpp"
#include "ecs/Systems/global/SceneSystem.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <vector>

using tnrw::ecs::RenderSystem;
using tnrw::ecs::SceneSystem;

entt::entity createShape(entt::registry &registry, tnrw::LevelIdentifierType level_id,
                         tnrw::ecs::Shape shape) {
    const auto entity = registry.create();
    registry.emplace<tnrw::ecs::Shape>(entity, shape);
    registry.emplace<tnrw::ecs::ShouldRender>(entity);
    SceneSystem::createScene(registry, level_id);
    SceneSystem::addToScene(registry, level_id, entity);
    return entity;
}

constexpr sf::Vector2u              window_size = {800, 600}; ///< 窗口的大小
constexpr tnrw::LevelIdentifierType first_level = 1;          ///< 第一个关卡
constexpr tnrw::LevelIdentifierType second_level = 2;         ///< 第二个关卡

int                                 main() {

    std::vector<entt::entity> line_vec;
    std::vector<entt::entity> circle_vec;
    std::vector<entt::entity> rect_vec;
    sf::RenderWindow          window(sf::VideoMode(window_size), "testRenderSystem");
    entt::registry            registry;

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    line_vec.push_back(createShape(
        registry, first_level,
        {tnrw::ecs::Shape::Line{.start = {.position = {20.f, 20.f}, .color = sf::Color::Red},
                                                                .end = {.position = {50.f, 50.f}, .color = sf::Color::Red}}}));
    circle_vec.push_back(createShape(registry, first_level,
                                                                     {tnrw::ecs::Shape::Circle{.center = {140.f, 60.f},
                                                                                               .radius = 30.f,
                                                                                               .fill_color = sf::Color::Blue}}));
    rect_vec.push_back(createShape(registry, first_level,
                                                                   {tnrw::ecs::Shape::Rectangle{.position = {400.f, 400.f},
                                                                                                .size = {50.f, 60.f},
                                                                                                .fill_color = sf::Color::Yellow}}));
    line_vec.push_back(createShape(
        registry, second_level,
        {tnrw::ecs::Shape::Line{.start = {.position = {200.f, 90.f}, .color = sf::Color::Green},
                                                                .end = {.position = {100.f, 100.f}, .color = sf::Color::Blue}}}));
    line_vec.push_back(createShape(
        registry, second_level,
        {tnrw::ecs::Shape::Line{.start = {.position = {550.f, 700.f}, .color = sf::Color::Red},
                                                                .end = {.position = {400.f, 200.f}, .color = sf::Color::Blue}}}));
    circle_vec.push_back(createShape(registry, second_level,
                                                                     {tnrw::ecs::Shape::Circle{.center = {300.f, 300.f},
                                                                                               .radius = 20.f,
                                                                                               .fill_color = sf::Color::Cyan}}));
    rect_vec.push_back(createShape(registry, second_level,
                                                                   {tnrw::ecs::Shape::Rectangle{.position = {400.f, 400.f},
                                                                                                .size = {50.f, 60.f},
                                                                                                .fill_color = sf::Color::Cyan}}));
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
        RenderSystem::draw(registry, window, [&registry, curlevel](entt::entity entity) -> bool {
            auto father_scenes = SceneSystem::getFatherScenes(registry, entity);
            return father_scenes.contains(SceneSystem::getSceneEntity(registry, curlevel));
        });
        window.display();
    }

    return 0;
}