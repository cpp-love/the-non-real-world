/**
 * @file render_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了渲染系统
 * @version 0.1.0-3
 * @date 2026-03-14
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/render_system.hpp"
#include "ecs/components/shape_components.hpp"
#include <entt/entity/registry.hpp>

namespace tnrw::ecs {

    void render_system::draw(const entt::registry                    &registry,
                             const std::function<bool(entt::entity)> &check_if_valid,
                             sf::RenderTarget &target, sf::RenderStates states) noexcept {
        auto should_render = registry.view<collidable_shape>();
        for (entt::entity entity : should_render) {
            if (check_if_valid(entity)) {
                std::visit([&target, &states](const auto &drawable) { target.draw(drawable, states); },
                           registry.get<collidable_shape>(entity).render.shape);
            }
        }
    }
} // namespace tnrw::ecs
