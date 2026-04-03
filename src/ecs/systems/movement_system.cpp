/**
 * @file movement_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了移动系统
 * @version 0.1.0-4
 * @date 2026-03-14
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/movement_system.hpp"
#include "base/assert_msg.hpp"
#include "base/floating_point_compare.hpp"
#include "base/overload.hpp"
// #include "base/sfml_formatter.hpp"
#include "base/type_traits.hpp"
#include "ecs/components/global/game_base.hpp"
#include "ecs/components/shape_components.hpp"
#include "ecs/systems/global/scene_system.hpp"
#include "math/functions.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <entt/entt.hpp>
#include <optional>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <utility>

namespace tnrw::ecs {

    /// @cond INTERNAL
    namespace {

        /**
         * @brief 判断某一位置是否在矩形内
         * @param [in] position 位置
         * @param [in] rect 矩形
         * @return true 在其内
         * @return false 不在其内
         * @warning 此为函数 @ref tnrw::ecs::is_collided 的辅助函数，其他函数不应访问
         */
        [[nodiscard]] bool is_in_rectangle(sf::Vector2f position, shape::rectangle rect) {
            const sf::Vector2f size_x_rotated = sf::Vector2f{rect.size.x, 0.f}.rotatedBy(rect.rotation);
            const sf::Vector2f size_y_rotated = sf::Vector2f{0.f, rect.size.y}.rotatedBy(rect.rotation);
            std::array<float, 4> area_signed{
                (position - rect.position).cross(size_x_rotated),
                (position - rect.position).cross(size_y_rotated),
                (position - (rect.position + size_x_rotated)).cross(size_x_rotated),
                (position - (rect.position + size_y_rotated)).cross(size_y_rotated)};
            return no_nan_inf_f{area_signed[0] * area_signed[2]} < no_nan_inf_f{0}
                   || no_nan_inf_f{area_signed[1] * area_signed[3]} < no_nan_inf_f{0};
        }

        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 圆
         * @param [in] second 圆
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::circle &first, const shape::circle &second) {
            auto dis_sq = (second.center - first.center).lengthSquared(); //< 两圆中心的距离
            return no_nan_inf_f{dis_sq} < no_nan_inf_f{first.radius + second.radius};
        }
        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 圆
         * @param [in] second 线段
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::circle &first, const shape::line &second) {
            //< 检测圆到直线的距离
            sf::Vector2f line = second.end - second.start;                   //< 线段首指向线段尾的向量
            sf::Vector2f line_start_to_center = first.center - second.start; //< 线段首指向圆心的向量
            float        area_signed =
                line.cross(line_start_to_center); //< 以圆心和线构成的平行四边形的面积（有符号）
            float len_sq = line.lengthSquared();  //< 线的长度的平方
            float len = std::sqrt(len_sq);        //< 线的长度
            float dis_signed = area_signed / len; //< 圆到线的距离（有符号）
            if (no_nan_inf_f{std::abs(dis_signed)} >= no_nan_inf_f{first.radius}) {
                return false;
            }

            // 检测圆到线段的距离
            // 计算投影参数
            float proj_param = line.dot(line_start_to_center) / len_sq;
            if (no_nan_inf_f{0} < no_nan_inf_f{proj_param}
                && no_nan_inf_f{proj_param} < no_nan_inf_f{1}) {
                return true;
            }
            if (no_nan_inf_f{0} > no_nan_inf_f{proj_param}) {
                return no_nan_inf_f{line_start_to_center.lengthSquared()}
                       < no_nan_inf_f{first.radius * first.radius};
            }
            return no_nan_inf_f{(second.end - first.center).lengthSquared()}
                   < no_nan_inf_f{first.radius * first.radius};
        }

        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 圆
         * @param [in] second 矩形
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::circle &first, const shape::rectangle &second) {
            std::array<float, 4> area_signed{
                (first.center - second.position).cross({second.size.x, 0.f}),
                (first.center - second.position).cross({0.f, second.size.y}),
                (first.center
                 - (second.position + sf::Vector2f{0.f, second.size.y}.rotatedBy(second.rotation)))
                    .cross({second.size.x, 0.f}),
                (first.center
                 - (second.position + sf::Vector2f{second.size.x, 0.f}.rotatedBy(second.rotation)))
                    .cross({0.f, second.size.y})};
            std::array<float, 4> distances_sq_signed{
                area_signed[0] * area_signed[0] / second.size.x / second.size.x,
                area_signed[1] * area_signed[1] / second.size.y / second.size.y,
                area_signed[2] * area_signed[2] / second.size.x / second.size.x,
                area_signed[3] * area_signed[3] / second.size.y / second.size.y,
            };
            // 边检查
            for (const auto dis_sq : distances_sq_signed) {
                if (no_nan_inf_f{std::abs(dis_sq)} < no_nan_inf_f{first.radius * first.radius}) {
                    return true;
                }
            }
            // 判断是否在矩形内
            return no_nan_inf_f{area_signed[0] * area_signed[2]} < no_nan_inf_f{0}
                   || no_nan_inf_f{area_signed[1] * area_signed[3]} < no_nan_inf_f{0};
        }

        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 矩形
         * @param [in] second 圆
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::rectangle &first, const shape::circle &second) {
            return is_collided(second, first);
        }
        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 矩形
         * @param [in] second 线段
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::rectangle &first, shape::line second) {
            return is_in_rectangle(second.start, first) || is_in_rectangle(second.end, first);
        }
        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 矩形
         * @param [in] second 矩形
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::rectangle &first, const shape::rectangle &second) {
            std::array<sf::Vector2f, 4> rect{
                second.position,
                second.position + sf::Vector2f{second.size.x, 0.f}.rotatedBy(second.rotation),
                second.position + sf::Vector2f{0.f, second.size.y}.rotatedBy(second.rotation),
                second.position + second.size.rotatedBy(second.rotation)};
            return std::ranges::all_of(
                rect, [&first](sf::Vector2f position) { return is_in_rectangle(position, first); });
        }

        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 线段
         * @param [in] second 圆
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::line &first, const shape::circle &second) {
            return is_collided(second, first);
        }
        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 线段
         * @param [in] second 线段
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::line &first, const shape::line &second) {
            auto line = first.end - first.start;
            auto area1_signed = (second.start - first.start).cross(line);
            auto area2_signed = (second.end - first.start).cross(line);
            return no_nan_inf_f{area1_signed * area2_signed} < no_nan_inf_f{0};
        }
        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 线段
         * @param [in] second 矩形
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape::line &first, const shape::rectangle &second) {
            return is_collided(second, first);
        }

        /**
         * @brief 判断两物体是否碰撞
         * @tparam SubShape 子图形类型（如 `tnrw::ecs::shape::circle` , `tnrw::ecs::shape::line` ）
         * @param [in] first 子图形
         * @param [in] second 图形
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        template <typename SubShape>
            requires is_variant_member_v<SubShape, shape::shape_type>
        [[nodiscard]] bool is_collided(const SubShape &first, const shape &second) {
            return std::visit([&first] [[nodiscard]] (
                                  const auto &second) -> bool { return is_collided(first, second); },
                              second.shape);
        }
        /**
         * @brief 判断两物体是否碰撞
         * @tparam SubShape 子图形类型（如 `tnrw::ecs::shape::circle` , `tnrw::ecs::shape::line` ）
         * @param [in] first 图形
         * @param [in] second 子图形
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        template <typename SubShape>
            requires is_variant_member_v<SubShape, shape::shape_type>
        [[nodiscard]] bool is_collided(const shape &first, const SubShape &second) {
            return std::visit([&second] [[nodiscard]] (
                                  const auto &first) -> bool { return is_collided(first, second); },
                              first.shape);
        }

        /**
         * @brief 判断两物体是否碰撞
         * @param [in] first 图形
         * @param [in] second 图形
         * @return true 两物体碰撞
         * @return false 两物体不碰撞
         */
        [[nodiscard]] bool is_collided(const shape &first, const shape &second) {
            return std::visit(
                [] [[nodiscard]] (const auto &first, const auto &second) -> bool {
                    return is_collided(first, second);
                },
                first.shape, second.shape);
        }

        /**
         * @brief 计算两物体的最小距离的平方
         * @param [in] first 点
         * @param [in] second 线段
         * @return float 点与线段的最小距离的平方
         */
        [[nodiscard]] float get_distance_squared(const sf::Vector2f &first, const shape::line &second) {
            //< 计算点到直线的距离
            sf::Vector2f line = second.end - second.start;           //< 线段首指向线段尾的向量
            sf::Vector2f line_start_to_point = first - second.start; //< 线段首指向点的向量
            float area = std::abs(line.cross(line_start_to_point));  //< 以点和线构成的平行四边形的面积
            float len_sq = line.lengthSquared();                     //< 线的长度的平方
            float dis_sq = area * area / len_sq;                     //< 点到线的距离的平方

            // 检测点到线段的距离
            // 计算投影参数
            float proj_param = line.dot(line_start_to_point) / len_sq;
            if (no_nan_inf_f{0} < no_nan_inf_f{proj_param}
                && no_nan_inf_f{proj_param} < no_nan_inf_f{1}) {
                return dis_sq;
            }
            if (no_nan_inf_f{0} > no_nan_inf_f{proj_param}) {
                return line_start_to_point.lengthSquared();
            }
            return (second.end - first).lengthSquared();
        }

// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
        /**
         * @brief 获取两线段所在直线碰撞的位置
         * @param [in] first 线段
         * @param [in] second 线段
         * @return sf::Vector2f 碰撞的位置
         * @warning 需确保两线段所在直线已经碰撞，否则结果未明确
         */
        [[nodiscard]] sf::Vector2f get_collided_position(const shape::line &first,
                                                         const shape::line &second) {
            auto line = first.end - first.start;
            auto area1_signed = (second.start - first.start).cross(line);
            auto area2_signed = (second.end - first.start).cross(line);
            return (second.end - second.start) * (area1_signed / (area1_signed - area2_signed))
                   + second.start;
        }

        /**
         * @brief 将角度转到坐标轴的第一象限，即 [0°, 90°]
         * @param [in] angle 角度
         * @return sf::Angle 转换后的角度
         */
        [[nodiscard]] sf::Angle wrap_to_first_quadrent(sf::Angle angle) {
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
            using namespace sf::Literals;
            angle = angle.wrapSigned();
            if (sf::Angle::Zero > angle) {
                angle = -angle;
            }
            if (90_deg < angle) {
                angle = 180_deg - angle;
            }
            return angle;
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
        }
#endif

    } // namespace
    /// @endcond

    [[nodiscard]] sf::Vector2f &movement_system::velocity(entt::registry &registry,
                                                          entt::entity    entity) noexcept {
        return registry.get_or_emplace<struct velocity>(entity, sf::Vector2f{0.f, 0.f}).velocity;
    }

    /// @todo 添加对 `shape::circle` 和别的图形/可渲染图形的更新支持
    /// @details 采用 CCD 算法
    void movement_system::update_with_velocity(entt::registry &registry, entt::entity entity,
                                               milliseconds_f delta_time) noexcept {
        TNRW_ASSERT_MSG(registry.all_of<collidable_shape>(entity),
                        "函数参数 `entity`（编号为：{}） 没有组件 `tnrw::ecs::collidable_shape`",
                        static_cast<entt::id_type>(entity));
        TNRW_ASSERT_MSG(std::holds_alternative<shape::circle>(
                            registry.get<collidable_shape>(entity).collision_box.shape),
                        "函数参数 `entity`（编号为：{}） 的组件 `tnrw::ecs::shape` 不是圆形",
                        static_cast<entt::id_type>(entity));

        auto list = registry.view<father_scenes, collidable_shape>(); //< 碰撞列表
        /**
         * @brief 判断当前实体与传入的比较实体是否在同一场景里
         * @param [in] compare_entity 比较实体
         * @return true 在同一场景
         * @return false 不在同一场景
         */
        auto is_in_same_scene = [&, &scenes = scene_system::get_father_scenes(registry, entity)](
                                    entt::entity compare_entity) -> bool {
            // 防止碰撞到自己
            if (entity == compare_entity) {
                return false;
            }

            /// @todo 性能优化
            const auto &compare_scenes = scene_system::get_father_scenes(registry, compare_entity);
            const auto &less_scenes = (scenes.size() < compare_scenes.size()) ? scenes : compare_scenes;
            const auto &greater_scenes =
                (scenes.size() < compare_scenes.size()) ? compare_scenes : scenes;

            return std::ranges::any_of(less_scenes, [&](level_identifier_type scene) -> bool {
                return greater_scenes.contains(scene);
            });
        };

        auto         &collidable = registry.get<collidable_shape>(entity); //< 当前形状
        auto         &current = std::get<shape::circle>(collidable.collision_box.shape);
        float         radius_sq = current.radius * current.radius;
        auto         &render_current = collidable.render; //< 当前渲染形状
        auto         &cur_velocity = velocity(registry, entity);

        constexpr int max_iterations = 15; //< 最大迭代次数
        for (int i = 0; true; ++i) {
            // 超过迭代次数，退出
            if (i == max_iterations) {
                spdlog::warn("current shape(entity: {})'s update time is greater than max iterations",
                             std::to_underlying(entity));
                break;
            }

            // 时间太少，退出
            if (no_nan_inf_f{delta_time.count()} == no_nan_inf_f{0}) {
                break;
            }

            sf::Vector2f forward_vec = delta_time.count() * cur_velocity;
            float        forward_vec_dis_sq = forward_vec.lengthSquared();
            // 不移动，退出
            if (no_nan_inf_f{forward_vec_dis_sq} == no_nan_inf_f{0}) {
                break;
            }

            // spdlog::trace("velocity is: {}", velocity);
            // spdlog::trace("forward_vec is: {}", forward_vec);

            bool                        should_exit = false;  //< 当起点就碰撞时设置为 `true`，表示退出
            float                       time_of_impact = 1.f; //< 最早碰撞时间(TOI)
            std::optional<sf::Vector2f> normal_line;          //< 用于更新的法线

            shape::rectangle            move_collision_rectangle{
                           .position =
                    current.center - sf::Vector2f{0, current.radius}.rotatedBy(forward_vec.angle()),
                           .size = {forward_vec.length(), 2 * current.radius},
                           .rotation = forward_vec.angle()}; //< 移动时的碰撞区域1
            shape::circle move_collision_circle{.center = current.center + forward_vec,
                                                .radius = current.radius}; //< 移动时的碰撞区域2
            for (entt::entity other_entity : list) {
                // 不在同一场景里，跳过
                if (!is_in_same_scene(other_entity)) {
                    continue;
                }
                auto cur_shape = registry.get<collidable_shape>(other_entity).collision_box; //< it的形状
                if (is_collided(move_collision_rectangle, cur_shape)
                    || is_collided(move_collision_circle, cur_shape)) {
                    // 碰撞了，更新移动的参数

                    // debug message
                    // spdlog::trace("current shape(entity:{}) collided with a shape(entity:{})",
                    //               std::to_underlying(entity), std::to_underlying(other_entity));

                    std::visit(
                        make_overloaded(
                            [&](const shape::line &line) {
                                sf::Vector2f line_vec = line.end - line.start; //< 线段首指向线段尾的向量
                                float        line_len_sq = line_vec.lengthSquared();
                                sf::Vector2f line_start_to_center =
                                    current.center - line.start; //< 线段首指向圆心的向量
                                sf::Vector2f line_end_to_center =
                                    current.center - line.end; //< 线段尾指向圆心的向量
                                auto get_proj_param = [&](float time) {
                                    return (line_start_to_center + forward_vec * time).dot(line_vec)
                                           / line_len_sq;
                                };

                                // 特殊：当 `t=0` 时碰撞
                                if (is_collided(current, cur_shape)) {
                                    float line_center_dis_sq = get_distance_squared(
                                        current.center, line); //< 圆到线的距离的平方

                                    sf::Vector2f normal; //< 法线
                                    // 计算法线
                                    if (line_center_dis_sq == line_start_to_center.lengthSquared()) {
                                        normal = line_start_to_center;
                                    } else if (line_center_dis_sq
                                               == line_end_to_center.lengthSquared()) {
                                        normal = line_end_to_center;
                                    } else {
                                        // `sf::Vector2f{vec.y, -vec.x}` : 将 `vec` 顺时针旋转 90°
                                        // `sf::Vector2f{-vec.y, vec.x}` : 将 `vec` 逆时针旋转 90°
                                        if (line_vec.cross(line_start_to_center) > 0) {
                                            // line_vec 逆时针旋转可以到 line_start_to_center
                                            normal = {-line_vec.y, line_vec.x};
                                        } else {
                                            // line_vec 顺时针旋转可以到 line_start_to_center
                                            normal = {line_vec.y, -line_vec.x};
                                        }
                                    }

                                    // 如果穿透，修正
                                    if (no_nan_inf_f{line_center_dis_sq} < no_nan_inf_f{radius_sq}) {
                                        sf::Vector2f move_off =
                                            (current.radius - std::sqrt(line_center_dis_sq)
                                             - no_nan_inf_f::epsilon)
                                            * normal.normalized();
                                        current.center += move_off;
                                        // render_current.move(move_off);
                                        std::visit(
                                            [&](auto &transformable) { transformable.move(move_off); },
                                            render_current.shape);
                                    }

                                    // 更新速度
                                    // `sf::Vector2f{-vec.y, vec.x}` : 将 `vec` 逆时针旋转 90°
                                    cur_velocity = cur_velocity.projectedOnto({-normal.y, normal.x});

                                    should_exit = true;
                                    return;
                                }

                                // 1. 与 `.start` 接触
                                {
                                    // 解二元一次方程组
                                    auto res = math::solve_quadratic_equation(
                                        forward_vec_dis_sq, 2 * line_start_to_center.dot(forward_vec),
                                        line_start_to_center.lengthSquared() - radius_sq);
                                    for (auto opt : res) {
                                        opt.transform([&](float time) {
                                            if (no_nan_inf_f{0} <= no_nan_inf_f{time}
                                                && no_nan_inf_f{time} <= no_nan_inf_f{1}
                                                && no_nan_inf_f{get_proj_param(time)}
                                                       <= no_nan_inf_f{0}) {
                                                if (no_nan_inf_f{time} < no_nan_inf_f{time_of_impact}) {
                                                    time_of_impact = time;
                                                    normal_line = line_start_to_center;
                                                } else if (no_nan_inf_f{time}
                                                           == no_nan_inf_f{time_of_impact}) {
                                                    normal_line =
                                                        normal_line
                                                            .transform([&](sf::Vector2f normal) {
                                                                return normal + line_start_to_center;
                                                            })
                                                            .or_else([&] -> std::optional<sf::Vector2f> {
                                                                return line_start_to_center;
                                                            });
                                                }
                                            }
                                            return 0;
                                        });
                                    }
                                }

                                // 2. 与 `.end` 接触
                                {
                                    // 解二元一次方程组
                                    auto res = math::solve_quadratic_equation(
                                        forward_vec_dis_sq, 2 * line_end_to_center.dot(forward_vec),
                                        line_end_to_center.lengthSquared() - radius_sq);
                                    for (auto opt : res) {
                                        opt.transform([&](float time) {
                                            if (no_nan_inf_f{0} <= no_nan_inf_f{time}
                                                && no_nan_inf_f{time} <= no_nan_inf_f{1}
                                                && no_nan_inf_f{get_proj_param(time)}
                                                       <= no_nan_inf_f{0}) {
                                                if (no_nan_inf_f{time} < no_nan_inf_f{time_of_impact}) {
                                                    time_of_impact = time;
                                                    normal_line = line_end_to_center;
                                                } else if (no_nan_inf_f{time}
                                                           == no_nan_inf_f{time_of_impact}) {
                                                    normal_line =
                                                        normal_line
                                                            .transform([&](sf::Vector2f normal) {
                                                                return normal + line_end_to_center;
                                                            })
                                                            .or_else([&] -> std::optional<sf::Vector2f> {
                                                                return line_end_to_center;
                                                            });
                                                }
                                            }
                                            return 0;
                                        });
                                    }
                                }

                                // 3. 在线段内
                                {
                                    float line_cross_forward = line_vec.cross(forward_vec);
                                    float line_cross_start_center = line_vec.cross(line_start_to_center);
                                    // 解二元一次方程组
                                    auto  res = math::solve_quadratic_equation(
                                        line_cross_forward * line_cross_forward,
                                        2 * line_cross_forward * line_cross_start_center,
                                        (line_cross_start_center * line_cross_start_center)
                                            - (line_len_sq * radius_sq));
                                    for (auto opt : res) {
                                        opt.transform([&](float time) {
                                            float proj_param = get_proj_param(time);
                                            if (no_nan_inf_f{0} <= no_nan_inf_f{time}
                                                && no_nan_inf_f{time} <= no_nan_inf_f{1}
                                                && no_nan_inf_f{0} <= no_nan_inf_f{proj_param}
                                                && no_nan_inf_f{proj_param} <= no_nan_inf_f{1}) {
                                                if (no_nan_inf_f{time} < no_nan_inf_f{time_of_impact}) {
                                                    time_of_impact = time;
                                                    // `sf::Vector2f{-vec.y, vec.x}` : 将 `vec` 逆时针旋转 90°
                                                    normal_line = {-line_vec.y, line_vec.x};
                                                } else if (no_nan_inf_f{time}
                                                           == no_nan_inf_f{time_of_impact}) {
                                                    normal_line =
                                                        normal_line
                                                            .transform([&](sf::Vector2f normal) {
                                                                return normal
                                                                       + sf::Vector2f{-line_vec.y,
                                                                                      line_vec.x};
                                                            })
                                                            .or_else([&] -> std::optional<sf::Vector2f> {
                                                                return {{-line_vec.y, line_vec.x}};
                                                            });
                                                }
                                            }
                                            return 0;
                                        });
                                    }
                                }
                            },
                            [&](const auto & /*unused*/) {
                                unreachable("编号为{}的实体的形状不是 `tnrw::ecs::render_shape::line` "
                                            "类型，目前移动时的碰撞处理仅支持 "
                                            "`tnrw::ecs::render_shape::line` 类型",
                                            static_cast<entt::id_type>(other_entity));
                            }),
                        cur_shape.shape);
                    if (should_exit) {
                        break;
                    }
                }
            }

            if (should_exit) {
                continue;
            }

            if (!normal_line.has_value()) {
                // 无碰撞时的移动
                current.center += forward_vec;
                // render_current.move(forward_vec);
                std::visit([&](auto &transformable) { transformable.move(forward_vec); },
                           render_current.shape);
                break;
            }
            // 有碰撞时的移动
            TNRW_ASSERT_MSG(no_nan_inf_f{0} <= no_nan_inf_f{time_of_impact}
                                && no_nan_inf_f{time_of_impact} < no_nan_inf_f{1.f},
                            "`time_of_impace` 应在 [0.f, 1.f) 区间内");

            sf::Vector2f move_off =
                forward_vec * time_of_impact - forward_vec.normalized() * no_nan_inf_f::epsilon;
            current.center += move_off;
            // render_current.move(move_off);
            std::visit([&](auto &transformable) { transformable.move(move_off); }, render_current.shape);

            // spdlog::trace("The next position is: {}",
            //               std::get<shape::circle>(static_cast<shape>(render).shape).center);

            // `sf::Vector2f{-vec.y, vec.x}` : 将 `vec` 逆时针旋转 90°
            cur_velocity = cur_velocity.projectedOnto({-normal_line->y, normal_line->x});
            delta_time *= (1.f - time_of_impact);
        }

        // if (velocity(registry, entity).lengthSquared() != 0) {
        // debug message
        //     spdlog::trace(
        //         "current shape(entity:{})'s position is: {}, update delta time is: {}",
        //         static_cast<entt::id_type>(entity),
        //         std::get<shape::circle>(registry.get<collidable_shape>(entity).collision_box.shape)
        //             .center,
        //         delta_time);
        // }
    }
    void movement_system::update_with_velocity(entt::registry &registry,
                                               milliseconds_f  delta_time) noexcept {
        auto list = registry.view<collidable_shape, struct velocity, father_scenes>();
        std::ranges::for_each(list, [&](entt::entity entity) -> void {
            if (std::holds_alternative<shape::circle>(
                    registry.get<collidable_shape>(entity).collision_box.shape)) {
                update_with_velocity(registry, entity, delta_time);
            }
        });
    }

} // namespace tnrw::ecs