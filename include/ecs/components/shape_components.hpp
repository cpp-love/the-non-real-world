/**
 * @file shape_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加了基本的形状组件
 * @version 0.1.0-3
 * @date 2026-03-14
 *
 * @copyright cpp-love
 *
 */

#ifndef TNRW_ECS_COMPONENTS_SHAPE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_SHAPE_COMPONENTS_HPP

#include "base/assert_msg.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <utility>
#include <variant>

namespace tnrw::ecs {

    /// @brief 形状类型
    /// @details 使用 `std::variant` 来表示不同形状的集合
    struct shape {
        /// @brief 线类型
        struct line {
            sf::Vector2f start; ///< 起点
            sf::Vector2f end;   ///< 终点
        };

        /// @brief 圆类型
        struct circle {
            sf::Vector2f center;       ///< 圆心
            float        radius = 0.f; ///< 半径
        };

        /// @brief 矩形类型
        struct rectangle {
            sf::Vector2f position;                    ///< 位置，指左上角的点
            sf::Vector2f size;                        ///< 大小，x为横轴的长/宽，y为纵轴的宽/长
            sf::Angle    rotation = sf::degrees(0.f); ///< 旋转角度
        };

        using shape_type = std::variant<line, circle, rectangle>; ///< 图形类型
        shape_type shape;                                         ///< 形状
    };

    /**
     * @brief 渲染形状类型
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
             * @brief 从 @ref tnrw::ecs::shape::line 构造的构造函数
             * @param [in] non_render_line 线段
             * @param [in] thickness 粗细
             */
            explicit line(shape::line non_render_line, float thickness = 1) noexcept
                : line(non_render_line.end - non_render_line.start, thickness) {
                setPosition(non_render_line.start);
            }
            /**
             * @brief 转换成不能渲染的线段的转换函数
             * @return shape::line 转换后的线段
             */
            explicit operator shape::line() const noexcept {
                return shape::line{.start = getPosition(), .end = getPosition() + get_end()};
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

        /// @brief 圆类型
        class circle : public sf::CircleShape {
          public:
            // 继承构造函数
            using sf::CircleShape::CircleShape;
            /**
             * @brief 从 @ref tnrw::ecs::shape::circle 构造的构造函数
             * @param [in] non_render_circle 圆
             * @param [in] point_count 渲染点数
             */
            explicit circle(
                shape::circle non_render_circle,
                std::size_t   point_count = 30 /*NOLINT(cppcoreguidelines-avoid-magic-numbers)*/)
                : circle(non_render_circle.radius, point_count) {
                setPosition(non_render_circle.center
                            - sf::Vector2f{non_render_circle.radius, non_render_circle.radius});
            }
            /**
             * @brief 转换成不能渲染的圆的转换函数
             * @return shape::circle 转换后的圆
             */
            explicit operator shape::circle() const noexcept {
                float radius = getRadius();
                return shape::circle{.center = getPosition() + sf::Vector2f{radius, radius},
                                     .radius = radius};
            }
        };

        /// @brief 矩形类型
        class rectangle : public sf::RectangleShape {
          public:
            // 继承构造函数
            using sf::RectangleShape::RectangleShape;
            /**
             * @brief 从 @ref tnrw::ecs::shape::rectangle 构造的构造函数
             * @param [in] non_render_rectangle 矩形
             */
            explicit rectangle(shape::rectangle non_render_rectangle)
                : rectangle(non_render_rectangle.size) {
                setPosition(non_render_rectangle.position);
                setRotation(non_render_rectangle.rotation);
            }
            /**
             * @brief 转换成不能渲染的矩形的转换函数
             * @return shape::rectangle 转换后的矩形
             */
            explicit operator shape::rectangle() const noexcept {
                return shape::rectangle{.position = getPosition(),
                                        .size = getSize(),
                                        .rotation = getRotation()};
            }
        };

        using shape_type = std::variant<line, circle, rectangle>; ///< 图形类型
        shape_type shape;                                         ///< 形状

        /**
         * @brief 构造一个能渲染的图形
         * @param [in] variant 图形
         */
        explicit render_shape(shape_type variant) noexcept : shape(std::move(variant)) {}
    };

    /// @brief 既可以参与碰撞又可以渲染的类型
    struct collidable_shape {
        shape        collision_box; ///< 碰撞箱
        render_shape render;        ///< 渲染图形
    };

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_SHAPE_COMPONENTS_HPP
