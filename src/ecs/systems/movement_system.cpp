/**
 * @file movement_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了移动系统
 * @version 0.1.0-2
 * @date 2026-02-27
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/movement_system.hpp"
#include "base/assert_msg.hpp"
#include "base/floating_point_compare.hpp"
#include "base/overload.hpp"
#include "base/sfml_formatter.hpp"
#include "ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <cmath>
#include <entt/entt.hpp>
#include <optional>

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
            sf::Vector2f line = second.end - second.start; //< 线首指向线尾的向量
            float        area_signed =
                line.cross(first.center - second.start); //< 以圆心和线构成的平行四边形的面积（有符号）
            float len = line.length();                   //< 线的长度
            float dis_signed = area_signed / len;        //< 圆到线的距离（有符号）
            if (no_nan_inf_f{std::abs(dis_signed)} >= no_nan_inf_f{first.radius}) {
                return false;
            }

            //< 检测圆到线段的距离
            sf::Vector2f line_dis = (line.normalized() * dis_signed);
            //line_dis.rotatedBy(-90_deg); <=> sf::Vector2f{line_dis.y, -line_dis.x}
            sf::Vector2f collided_pos =
                first.center
                + sf::Vector2f{line_dis.y,
                               -line_dis.x}; //< NOLINT(cppcoreguidelines-avoid-magic-numbers)
            sf::Vector2f max_position;
            sf::Vector2f min_position;
            if (no_nan_inf_f{second.start.x} < no_nan_inf_f{second.end.x}) {
                min_position = second.start;
                max_position = second.end;
            } else {
                min_position = second.end;
                max_position = second.start;
            }
            if (no_nan_inf_f{min_position.x} < no_nan_inf_f{collided_pos.x}
                && no_nan_inf_f{max_position.x} > no_nan_inf_f{collided_pos.x}) {
                return true;
            }
            if (no_nan_inf_f{max_position.x} < no_nan_inf_f{collided_pos.x}) {
                return no_nan_inf_f{(max_position - first.center).lengthSquared()}
                       < no_nan_inf_f{first.radius * first.radius};
            }
            return no_nan_inf_f{(min_position - first.center).lengthSquared()}
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
            requires requires { std::holds_alternative<SubShape>(std::declval<shape>().shape); }
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
            requires requires { std::holds_alternative<SubShape>(std::declval<shape>().shape); }
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
         * @brief 获取两线段所在直线碰撞的位置
         * @param [in] first 线段
         * @param [in] second 线段
         * @return sf::Vector2f 碰撞的位置
         * @warning 需确保两线段所在直线已经碰撞，否则结果未明确
         */
        [[nodiscard]] sf::Vector2f get_collided_position(const shape::line &first,
                                                         const shape::line &second) {
            // spdlog::trace("the first line is ({}, {}), second line is ({}, {})", first.start,
            //               first.end, second.start, second.end);
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
    } // namespace
    /// @endcond

    [[nodiscard]] sf::Vector2f &movement_system::velocity(entt::registry &registry,
                                                          entt::entity    entity) noexcept {
        return registry.get_or_emplace<struct velocity>(entity, sf::Vector2f{0.f, 0.f}).velocity;
    }

    /// @todo 添加对 `shape::circle` 和别的图形/可渲染图形的更新支持
    /// @todo 采用 CCD 算法
    void movement_system::update_with_velocity(entt::registry &registry, entt::entity entity,
                                               milliseconds_f delta_time) noexcept {
        TNRW_ASSERT_MSG(registry.all_of<render_shape>(entity),
                        "函数参数 `entity`（编号为：{}） 没有组件 `tnrw::ecs::shape`",
                        static_cast<entt::id_type>(entity));
        TNRW_ASSERT_MSG(
            std::holds_alternative<render_shape::circle>(registry.get<render_shape>(entity).shape),
            "函数参数 `entity`（编号为：{}） 的组件 `tnrw::ecs::shape` 不是圆形",
            static_cast<entt::id_type>(entity));

        auto list = registry.view<father_scenes, render_shape>(); //< 碰撞列表
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

            const auto &compare_scenes = scene_system::get_father_scenes(registry, compare_entity);
            const auto &less_scenes = (scenes.size() < compare_scenes.size()) ? scenes : compare_scenes;
            const auto &greater_scenes =
                (scenes.size() < compare_scenes.size()) ? compare_scenes : scenes;

            return std::ranges::any_of(less_scenes, [&](level_identifier_type scene) -> bool {
                return greater_scenes.contains(scene);
            });
        };

        sf::Vector2f pre_forward_direction;
        sf::Vector2f pre_pre_forward_direction;
        /**
         * @brief 物体移动函数
         * @param [in] self 自己，即定义的这个变量
         * @param [in] forward_dis 总的移动距离，方向不变
         * @param [in] forward_direction 当前移动方向及最大移动距离，每轮都不一样
         */
        auto         move =
            [&, &render = registry.get<render_shape>(entity)](this auto &&move, sf::Vector2f forward_dis,
                                                              sf::Vector2f forward_direction) -> void {
            // 不移动退出
            if (no_nan_inf_f{forward_dis.lengthSquared()} == no_nan_inf_f{0}) {
                return;
            }

            // 重复移动退出
            if (pre_pre_forward_direction == forward_direction) {
                return;
            }
            pre_pre_forward_direction = pre_forward_direction;
            pre_forward_direction = forward_direction;

            sf::Vector2f max_move_dis; //< 当前一次的最大移动距离
            if (auto projected = forward_dis.projectedOnto(forward_direction);
                no_nan_inf_f{projected.lengthSquared()}
                <= no_nan_inf_f{forward_direction.lengthSquared()}) {
                max_move_dis = projected;
            } else {
                max_move_dis = forward_direction;
            }

            // 不移动退出
            if (no_nan_inf_f{max_move_dis.lengthSquared()} == no_nan_inf_f{0}) {
                return;
            }

            auto  current = std::get<shape::circle>(static_cast<shape>(render).shape);
            auto &render_current = std::get<render_shape::circle>(render.shape);

            spdlog::trace("max_move_dis is: {}", max_move_dis);

            struct next_argument {
                sf::Vector2f move_dis;          //< 下一步移动的距离
                sf::Vector2f forward_direction; //< 下一步的 `forward_direction`
            };
            std::optional<next_argument> next_arg; //< 碰撞后的一些参数（如果有）

            shape::rectangle             move_collision_rectangle{
                            .position =
                    current.center - sf::Vector2f{0, current.radius}.rotatedBy(max_move_dis.angle()),
                            .size = {max_move_dis.length(), 2 * current.radius},
                            .rotation = max_move_dis.angle()}; //< 移动时的碰撞区域1
            shape::circle move_collision_circle{.center = current.center + max_move_dis,
                                                .radius = current.radius}; //< 移动时的碰撞区域2
            shape::line   line_to_get_collision_pos{
                  .start = current.center - max_move_dis.normalized() * current.radius,
                  .end = current.center + max_move_dis.normalized() * current.radius
                       + max_move_dis}; //< 获得碰撞位置的参数之一
            for (entt::entity other_entity : list) {
                // 不在同一场景里，跳过
                if (!is_in_same_scene(other_entity)) {
                    continue;
                }
                auto cur_shape =
                    static_cast<shape>(registry.get<render_shape>(other_entity)); //< it的形状
                if (bool rect_collided = is_collided(move_collision_rectangle, cur_shape),
                    circle_collided = is_collided(move_collision_circle, cur_shape);
                    rect_collided || circle_collided) {
                    // 碰撞了，更新移动的参数

                    // debug message
                    spdlog::trace("current shape(entity:{}) collided with a shape(entity:{}), "
                                  "rectangle collided: {}, circle collided: {}",
                                  static_cast<entt::id_type>(other_entity),
                                  static_cast<entt::id_type>(other_entity), rect_collided,
                                  circle_collided);

                    std::visit(
                        make_overloaded(
                            [&](const shape::line &line) {
                                // 获取更新后的位置
                                auto [pos,
                                      forward_direction] = [&] -> std::pair<sf::Vector2f, sf::Vector2f> {
                                    sf::Vector2f line_vec = line.end - line.start;
                                    /// @todo if line_vec == {0, 0}?
                                    TNRW_ASSERT_MSG(no_nan_inf_f{line_vec.lengthSquared()}
                                                        != no_nan_inf_f{0},
                                                    "not implemented yet");
                                    sf::Vector2f collided_position =
                                        get_collided_position(line, line_to_get_collision_pos);
                                    // 判断是否平行或相交位置超范围
                                    spdlog::trace("the line_to_get_collision_pos is ({}, {})",
                                                  line_to_get_collision_pos.start,
                                                  line_to_get_collision_pos.end);
                                    spdlog::trace("the collided position is {}", collided_position);
                                    if (no_nan_inf_f{line_vec.cross(line_to_get_collision_pos.end
                                                                    - line_to_get_collision_pos.start)}
                                            == no_nan_inf_f{0}
                                        || collided_position.x < std::min(line.start.x, line.end.x)
                                        || collided_position.x > std::max(line.start.x, line.end.x)) {
                                        spdlog::trace("entered");
                                        float area =
                                            std::abs((current.center - line.start).cross(line_vec));
                                        float        dis_sq = area * area / line_vec.lengthSquared();
                                        float        dis = std::sqrt(dis_sq);
                                        float        len_sq = (current.radius * current.radius) - dis_sq;
                                        float        len = std::sqrt(len_sq);
                                        sf::Vector2f radius = {len, dis};
                                        sf::Vector2f final_pos; //< 最终的位置
                                        if ((line.end - current.center).lengthSquared()
                                            > (line.start - current.center).lengthSquared()) {
                                            // `line.start` 更近
                                            final_pos = line.start - radius;
                                        } else {
                                            // `line.end` 更近
                                            final_pos = line.end - radius;
                                        }
                                        // `sf::Vector2f{radius.y, -radius.x}` : 将 `radius` 逆时针旋转 90°
                                        sf::Vector2f next_forward_direction =
                                            sf::Vector2f{radius.y, -radius.x};
                                        next_forward_direction =
                                            forward_dis.projectedOnto(next_forward_direction);
                                        return {final_pos, next_forward_direction};
                                    }
                                    if (line_vec.y < 0) {
                                        line_vec = -line_vec;
                                    }
                                    return {collided_position
                                                - (max_move_dis.normalized()
                                                   * (current.radius
                                                      / std::sin(wrap_to_first_quadrent(
                                                                     line_vec.angleTo(max_move_dis))
                                                                     .asRadians()))),
                                            line.end - line.start};
                                }();
                                auto move_dis = pos - current.center;

                                next_arg =
                                    next_arg
                                        .transform([&](next_argument arg) {
                                            if (no_nan_inf_f{arg.move_dis.lengthSquared()}
                                                > no_nan_inf_f{move_dis.lengthSquared()}) {
                                                arg.move_dis = move_dis;
                                                arg.forward_direction = forward_direction;
                                            }
                                            return arg;
                                        })
                                        .or_else([&]() -> std::optional<next_argument> {
                                            return next_argument{.move_dis = move_dis,
                                                                 .forward_direction = forward_direction};
                                        });
                            },
                            [&](const auto & /*unused*/) {
                                unreachable("编号为{}的实体的形状不是 `tnrw::ecs::render_shape::line` "
                                            "类型，目前移动时的碰撞处理仅支持 "
                                            "`tnrw::ecs::render_shape::line` 类型",
                                            static_cast<entt::id_type>(other_entity));
                            }),
                        cur_shape.shape);
                }
            }
            next_arg
                .transform([&](next_argument arg) -> int {
                    // 有碰撞时的移动
                    render_current.move(arg.move_dis);
                    spdlog::trace("The next position is: {}",
                                  std::get<shape::circle>(static_cast<shape>(render).shape).center);
                    sf::Vector2f forward_rest = forward_dis;
                    if (no_nan_inf_f{arg.move_dis.lengthSquared()} != no_nan_inf_f{0}) {
                        forward_rest -= (forward_dis.normalized()
                                         * (arg.move_dis.length()
                                            / std::cos(arg.move_dis.angleTo(forward_dis).asRadians())));
                    }
                    move(forward_rest, arg.forward_direction);
                    return 0;
                })
                .or_else([&]() -> std::optional<int> {
                    // 无碰撞时的移动
                    render_current.move(max_move_dis);
                    sf::Vector2f forward_rest =
                        forward_dis
                        - (forward_dis.normalized() * max_move_dis.length()
                           / std::cos(forward_dis.angleTo(max_move_dis).asRadians()));
                    move(forward_rest, forward_rest);
                    return {};
                });
        };

        auto forward_dis = delta_time.count() * velocity(registry, entity); //< 前进的距离

        if (no_nan_inf_f{forward_dis.lengthSquared()} > no_nan_inf_f{0}) {
            move(forward_dis, forward_dis);
            // debug message
            spdlog::trace(
                "current shape(entity:{})'s position is: {}, update delta time is: {}",
                static_cast<entt::id_type>(entity),
                std::get<shape::circle>(static_cast<shape>(registry.get<render_shape>(entity)).shape)
                    .center,
                delta_time);
        }
    }
    void movement_system::update_with_velocity(entt::registry &registry,
                                               milliseconds_f  delta_time) noexcept {
        auto list = registry.view<render_shape, struct velocity, father_scenes>();
        std::ranges::for_each(list, [&](entt::entity entity) -> void {
            if (std::holds_alternative<render_shape::circle>(registry.get<render_shape>(entity).shape)) {
                update_with_velocity(registry, entity, delta_time);
            }
        });
    }

} // namespace tnrw::ecs