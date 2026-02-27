/**
 * @file render_shape_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加了基本的可渲染的形状组件
 * @version 0.1.0-1
 * @date 2026-02-23
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#ifndef TNRW_ECS_COMPONENTS_RENDER_SHAPE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_RENDER_SHAPE_COMPONENTS_HPP

#include "base/assert_msg.hpp"
#include "base/overload.hpp"
#include "ecs/components/shape_components.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <utility>
#include <variant>

namespace tnrw::ecs {

    /**
     * @brief 形状类型
     * @details 使用 `std::variant` 来表示不同形状的集合
     */
    struct render_shape {
        /// @brief 线类型
        class line : public sf::Shape {
          private:
            sf::Vector2f m_end;       ///< 终点
            float        m_thickness; ///< 粗细
          public:
            /**
             * @brief 从终点构造的构造函数
             * @param [in] end 终点
             * @param [in] thickness 粗细
             */
            explicit line(sf::Vector2f end = {0, 0}, float thickness = 1) noexcept
                : m_end(end), m_thickness(thickness) {
                update();
            }

            /**
             * @brief 获取终点
             * @return sf::Vector2f 终点
             */
            [[nodiscard]] sf::Vector2f get_end() const noexcept { return m_end; }
            /**
             * @brief 设置终点
             * @param [in] end 终点
             */
            void                       set_end(sf::Vector2f end) noexcept {
                m_end = end;
                update();
            }
            /**
             * @brief 获取粗细
             * @return sf::Vector2f 粗细
             */
            [[nodiscard]] float get_thickness() const noexcept { return m_thickness; }
            /**
             * @brief 设置粗细
             * @param [in] thickness 粗细
             */
            void                set_thickness(float thickness) noexcept {
                m_thickness = thickness;
                update();
            }

            /**
             * @brief 获取线总计的点数
             * @return std::size_t 线总计的点数，为 4
             */
            [[nodiscard]] std::size_t  getPointCount() const noexcept override { return 4; }
            /**
             * @brief 获取构成图形的指定的点
             * @param [in] index 索引（只允许为 0~3）
             * @return sf::Vector2f 指定的点
             */
            [[nodiscard]] sf::Vector2f getPoint(std::size_t index) const noexcept override {
                // `sf::Vector2f{-m_end.y, m_end.x}` : 将 `m_end` 逆时针旋转 90°
                sf::Vector2f offset = sf::Vector2f{-m_end.y, m_end.x}.normalized() * (m_thickness / 2);
                switch (index) {
                    case 0: return offset;
                    case 1: return m_end + offset;
                    case 2: return m_end - offset;
                    case 3: return -offset;
                    default: tnrw::unreachable("index错误地不在0~4之间（实际为{}）", index);
                }
            }
        };
        using circle = sf::CircleShape;       ///< 圆类型
        using rectangle = sf::RectangleShape; ///< 矩形类型

        using shape_type = std::variant<line, circle, rectangle>; ///< 图形类型
        shape_type shape;                                         ///< 形状

        /**
         * @brief 构造一个能渲染的图形
         * @param [in] variant 图形
         */
        explicit render_shape(shape_type variant) noexcept : shape(std::move(variant)) {}
        /**
         * @brief 构造一个能渲染的图形
         * @param [in] non_render_shape 不能渲染的图形
         * @note 对于渲染的图形的其他参数，使用的都是默认的参数
         */
        explicit render_shape(const struct shape &non_render_shape) noexcept
            : shape(std::visit(make_overloaded(
                                   [](const shape::line &line) -> shape_type {
                                       render_shape::line render_line(line.end - line.start);
                                       render_line.setPosition(line.start);
                                       return render_line;
                                   },
                                   [](const shape::circle &circle) -> shape_type {
                                       render_shape::circle render_circle(circle.radius);
                                       render_circle.setPosition(
                                           circle.center - sf::Vector2f{circle.radius, circle.radius});
                                       return render_circle;
                                   },
                                   [](const shape::rectangle &rect) -> shape_type {
                                       render_shape::rectangle render_rect(rect.size);
                                       render_rect.setPosition(rect.position);
                                       render_rect.setRotation(rect.rotation);
                                       return render_rect;
                                   }),
                               non_render_shape.shape)) {
            static_assert(std::variant_size_v<shape::shape_type> == 3,
                          "此函数未完备所有 `shape` 的子类型的处理");
        }

        /**
         * @brief 转换成不能渲染的图形的转换构造函数
         * @return struct shape 转换成的不能渲染的图形
         */
        explicit operator struct shape() const noexcept {
            static_assert(std::variant_size_v<shape_type> == 3,
                          "此函数未完备所有 `shape` 的子类型的处理");
            return {std::visit(make_overloaded(
                                   [](const line &render_line) -> shape::shape_type {
                                       return shape::line{.start = render_line.getPosition(),
                                                          .end = render_line.getPosition()
                                                                 + render_line.get_end()};
                                   },
                                   [](const circle &render_circle) -> shape::shape_type {
                                       float radius = render_circle.getRadius();
                                       return shape::circle{.center = render_circle.getPosition()
                                                                      + sf::Vector2f{radius, radius},
                                                            .radius = radius};
                                   },
                                   [](const rectangle &render_rect) -> shape::shape_type {
                                       return shape::rectangle{.position = render_rect.getPosition(),
                                                               .size = render_rect.getSize(),
                                                               .rotation = render_rect.getRotation()};
                                   }),
                               shape)};
        }
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_RENDER_SHAPE_COMPONENTS_HPP
