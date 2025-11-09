/**
 * @file RenderSystem.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了渲染系统
 * @version 0.1.0-1
 * @date 2025-10-18
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/Systems/RenderSystem.hpp"
#include <entt/entt.hpp>

namespace tnrw::ecs {

    std::size_t RenderSystem::circle_point_count =
        30; // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    void RenderSystem::drawer(sf::RenderTarget &render, const Shape::Line &line) {
        std::array<sf::Vertex, 2> draw_line{line.start, line.end};
        render.draw(draw_line.data(), draw_line.size(), sf::PrimitiveType::Lines);
    }
    void RenderSystem::drawer(sf::RenderTarget &render, const Shape::Circle &circle) {
        sf::CircleShape draw_circle(circle.radius, circle_point_count);
        draw_circle.setPosition(circle.center + sf::Vector2f(circle.radius, circle.radius));
        draw_circle.setFillColor(circle.fill_color);
        draw_circle.setOutlineThickness(circle.outline_thickness);
        draw_circle.setOutlineColor(circle.outline_color);
        render.draw(draw_circle);
    }
    void RenderSystem::drawer(sf::RenderTarget &render, const Shape::Rectangle &rect) {
        sf::RectangleShape draw_rect(rect.size);
        draw_rect.setPosition(rect.position);
        draw_rect.setFillColor(rect.fill_color);
        draw_rect.setOutlineThickness(rect.outline_thickness);
        draw_rect.setOutlineColor(rect.outline_color);
        draw_rect.rotate(rect.rotation);
        render.draw(draw_rect);
    }
    [[nodiscard]] std::size_t RenderSystem::getCirclePointCount() { return circle_point_count; }
    void RenderSystem::setCirclePointCount(std::size_t new_cnt) { circle_point_count = new_cnt; }
    void RenderSystem::draw(
        const entt::registry &registry, sf::RenderTarget &render,
        std::function<bool(entt::entity)> checkIfVaild) { // NOLINT(performance-unnecessary-value-param)
        auto should_render = registry.view<ShouldRender>();
        for (const auto &entity : should_render) {
            if (checkIfVaild(entity)) {
                const auto *result = registry.try_get<Shape>(entity);
                if (result != nullptr) {
                    std::visit([&render](auto drawable) { drawer(render, drawable); }, result->shape);
                }
            }
        }
    }
} // namespace tnrw::ecs
