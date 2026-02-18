/**
 * @file render_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了渲染系统
 * @version 0.1.0-1
 * @date 2025-10-18
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/render_system.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <entt/entt.hpp>

namespace tnrw::ecs {

    namespace {
        void drawer(sf::RenderTarget &render, const Shape::Line &line) noexcept {
            std::array<sf::Vertex, 2> draw_line{line.start, line.end};
            render.draw(draw_line.data(), draw_line.size(), sf::PrimitiveType::Lines);
        }
        std::size_t &getPointCountRef() noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
            static std::size_t point_count = 30;
            return point_count;
        }
        void drawer(sf::RenderTarget &render, const Shape::Circle &circle) noexcept {
            sf::CircleShape draw_circle(circle.radius, getPointCountRef());
            draw_circle.setPosition(circle.center - sf::Vector2f{circle.radius, circle.radius});
            draw_circle.setFillColor(circle.fill_color);
            draw_circle.setOutlineThickness(circle.outline_thickness);
            draw_circle.setOutlineColor(circle.outline_color);
            render.draw(draw_circle);
        }
        void drawer(sf::RenderTarget &render, const Shape::Rectangle &rect) noexcept {
            sf::RectangleShape draw_rect(rect.size);
            draw_rect.setPosition(rect.position);
            draw_rect.setFillColor(rect.fill_color);
            draw_rect.setOutlineThickness(rect.outline_thickness);
            draw_rect.setOutlineColor(rect.outline_color);
            draw_rect.setRotation(rect.rotation);
            render.draw(draw_rect);
        }
        // struct PointCount {
        //     std::size_t point_count;
        // };
        // std::size_t &getPointCountRef(entt::registry &registry) noexcept {
        //     if (!registry.ctx().contains<PointCount>()) {
        //         // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        //         registry.ctx().emplace<PointCount>(30); //< 默认个数
        //     }
        //     return registry.ctx().get<PointCount>().point_count;
        // }
    } // namespace

    [[nodiscard]] std::size_t RenderSystem::getCirclePointCount() noexcept { return getPointCountRef(); }
    void                      RenderSystem::setCirclePointCount(std::size_t new_cnt) noexcept {
        getPointCountRef() = new_cnt;
    }
    void RenderSystem::draw(const entt::registry &registry, sf::RenderTarget &render,
                            std::function<bool(entt::entity)>
                                check_if_valid) noexcept { // NOLINT(performance-unnecessary-value-param)
        auto should_render = registry.view<ShouldRender>();
        for (const auto &entity : should_render) {
            if (check_if_valid(entity)) {
                const auto *result = registry.try_get<Shape>(entity);
                if (result != nullptr) {
                    std::visit([&render](auto drawable) { drawer(render, drawable); }, result->shape);
                }
            }
        }
    }
} // namespace tnrw::ecs
