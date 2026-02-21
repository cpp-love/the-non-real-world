/**
 * @file shape_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加了基本的形状、可渲染、可碰撞组件
 * @version 0.1.0-1
 * @date 2025-10-18
 *
 * @copyright cpp-love
 *
 */

#ifndef TNRW_ECS_COMPONENTS_SHAPE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_SHAPE_COMPONENTS_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Angle.hpp>
#include <variant>

namespace tnrw::ecs {

    /// @brief 形状类型
    /// @details 使用 `std::variant` 来表示不同形状的集合
    struct shape {
        /// @brief 线类型
        struct line {
            sf::Vertex start; ///< 起点
            sf::Vertex end;   ///< 终点
        };

        /// @brief 圆类型
        struct circle {
            sf::Vector2f center;                                 ///< 圆心
            float        radius = 0.f;                           ///< 半径
            sf::Color    fill_color = sf::Color::Transparent;    ///< 填充颜色
            sf::Color    outline_color = sf::Color::Transparent; ///< 外边颜色
            float        outline_thickness = 0.f;                ///< 外边长度
        };

        /// @brief 矩形类型
        struct rectangle {
            sf::Vector2f position;                            ///< 位置，指左上角的点
            sf::Vector2f size;                                ///< 大小，x为横轴的长/宽，y为纵轴的宽/长
            sf::Color    fill_color = sf::Color::Transparent; ///< 填充颜色
            sf::Angle    rotation = sf::degrees(0.f);         ///< 旋转角度
            sf::Color    outline_color = sf::Color::Transparent; ///< 外边颜色
            float        outline_thickness = 0.f;                ///< 外边厚度
        };

        std::variant<line, circle, rectangle> shape; ///< 形状
    };

    /// @brief 空类型，用于表示实体应该渲染
    struct should_render {};

    /// @brief 空类型，用于表示实体应该碰撞
    struct should_collide {};

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_SHAPE_COMPONENTS_HPP
