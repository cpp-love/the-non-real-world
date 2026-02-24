/**
 * @file movement_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了移动系统
 * @version 0.1.0-1
 * @date 2025-11-15
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/systems/movement_system.hpp"
#include "base/assert_msg.hpp"
#include "base/floating_point_compare.hpp"
#include "base/sfml_formatter.hpp"
#include "ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <entt/entt.hpp>
#include <optional>
#include <type_traits>

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
        [[nodiscard]] bool tnrw_is_in_rectangle(sf::Vector2f position, shape::rectangle rect) {
            const sf::Vector2f size_x_rotated = sf::Vector2f{rect.size.x, 0.f}.rotatedBy(rect.rotation);
            const sf::Vector2f size_y_rotated = sf::Vector2f{0.f, rect.size.y}.rotatedBy(rect.rotation);
            std::array<float, 4> area_signed{
                (position - rect.position).cross(size_x_rotated),
                (position - rect.position).cross(size_y_rotated),
                (position - (rect.position + size_x_rotated)).cross(size_x_rotated),
                (position - (rect.position + size_y_rotated)).cross(size_y_rotated)};
            return fast_floatf{area_signed[0] * area_signed[2]} < fast_floatf{0}
                   || fast_floatf{area_signed[1] * area_signed[3]} < fast_floatf{0};
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
            return fast_floatf{dis_sq} < fast_floatf{first.radius + second.radius};
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
            sf::Vector2f line = second.end.position - second.start.position; //< 线首指向线尾的向量
            float        area_signed = line.cross(
                first.center - second.start.position); //< 以圆心和线构成的平行四边形的面积（有符号）
            float len = line.length();                        //< 线的长度
            float dis_signed = area_signed / len;             //< 圆到线的距离（有符号）
            if (fast_floatf{std::abs(dis_signed)} >= fast_floatf{first.radius}) {
                return false;
            }

            //< 检测圆到线段的距离
            using namespace sf::Literals;
            sf::Vector2f collided_pos =
                first.center
                + (line.normalized() * dis_signed)
                      .rotatedBy(-90_deg); //< NOLINT(cppcoreguidelines-avoid-magic-numbers)
            sf::Vector2f max_position;
            sf::Vector2f min_position;
            if (fast_floatf{second.start.position.x} < fast_floatf{second.end.position.x}) {
                min_position = second.start.position;
                max_position = second.end.position;
            } else {
                min_position = second.end.position;
                max_position = second.start.position;
            }
            if (fast_floatf{min_position.x} < fast_floatf{collided_pos.x}
                && fast_floatf{max_position.x} > fast_floatf{collided_pos.x}) {
                return true;
            }
            if (fast_floatf{max_position.x} < fast_floatf{collided_pos.x}) {
                return fast_floatf{(max_position - first.center).lengthSquared()}
                       < fast_floatf{first.radius * first.radius};
            }
            return fast_floatf{(min_position - first.center).lengthSquared()}
                   < fast_floatf{first.radius * first.radius};
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
                if (fast_floatf{std::abs(dis_sq)} < fast_floatf{first.radius * first.radius}) {
                    return true;
                }
            }
            // 判断是否在矩形内
            return fast_floatf{area_signed[0] * area_signed[2]} < fast_floatf{0}
                   || fast_floatf{area_signed[1] * area_signed[3]} < fast_floatf{0};
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
            return tnrw_is_in_rectangle(second.start.position, first)
                   || tnrw_is_in_rectangle(second.end.position, first);
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
                rect, [&first](sf::Vector2f position) { return tnrw_is_in_rectangle(position, first); });
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
            auto line = first.end.position - first.start.position;
            auto area1_signed = (second.start.position - first.start.position).cross(line);
            auto area2_signed = (second.end.position - first.start.position).cross(line);
            return fast_floatf{area1_signed * area2_signed} < fast_floatf{0};
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
            // spdlog::trace("the first line is ({}, {}), second line is ({}, {})", first.start.position,
            //               first.end.position, second.start.position, second.end.position);
            auto line = first.end.position - first.start.position;
            auto area1_signed = (second.start.position - first.start.position).cross(line);
            auto area2_signed = (second.end.position - first.start.position).cross(line);
            return (second.end.position - second.start.position)
                       * (area1_signed / (area1_signed - area2_signed))
                   + second.start.position;
        }

        /**
         * @brief 将角度转到坐标轴的第一象限，即 [0°, 90°]
         * @param [in] angle 角度
         * @return sf::Angle 转换后的角度
         */
        [[nodiscard]] sf::Angle wrap_to_first(sf::Angle angle) {
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
    void movement_system::update_with_velocity(entt::registry &registry, entt::entity entity,
                                               std::chrono::milliseconds delta_time) noexcept {
        ASSERT_MSG(registry.all_of<shape>(entity),
                   "函数参数 `entity`（编号为：{}） 没有组件 `tnrw::ecs::shape`",
                   static_cast<entt::id_type>(entity));
        ASSERT_MSG(std::holds_alternative<shape::circle>(registry.get<shape>(entity).shape),
                   "函数参数 `entity`（编号为：{}） 的组件 `tnrw::ecs::shape` 不是圆形",
                   static_cast<entt::id_type>(entity));

        auto list = registry.view<should_collide, father_scenes, shape>(); //< 碰撞列表
        using iterator = decltype(list.begin());                           //< 迭代器类型
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
        /**
         * @brief 迭代器自加（忽略不在同一场景内的实体）
         * @param [in, out] cur 当前迭代器的引用
         * @return Iterator& 修改后的当前迭代器
         */
        auto to_next = [&](iterator &cur) -> iterator & {
            while (true) {
                ++cur;
                if (cur == list.end()) {
                    break;
                }
                if (is_in_same_scene(*cur)) {
                    break;
                }
            }
            return cur;
        };

        sf::Vector2f pre_forward_direction;
        sf::Vector2f pre_pre_forward_direction;
        /**
         * @brief 物体移动函数
         * @param [in] self 自己，即定义的这个变量
         * @param [in] forward_dis 总的移动距离，方向不变
         * @param [in] forward_direction 当前移动方向及最大移动距离，每轮都不一样
         */
        auto         move = [&, &current = std::get<shape::circle>(registry.get<shape>(entity).shape)](
                        auto self, sf::Vector2f forward_dis, sf::Vector2f forward_direction) -> void {
            // 不移动就退出
            if (fast_floatf{forward_dis.lengthSquared()} == fast_floatf{0}) {
                return;
            }
            // 重复移动也退出
            if (pre_pre_forward_direction == forward_direction) {
                return;
            }
            pre_pre_forward_direction = pre_forward_direction;
            pre_forward_direction = forward_direction;

            sf::Vector2f max_move_dis; //< 当前一次的最大移动距离
            if (auto projected = forward_dis.projectedOnto(forward_direction);
                fast_floatf{projected.lengthSquared()}
                <= fast_floatf{forward_direction.lengthSquared()}) {
                max_move_dis = projected;
            } else {
                max_move_dis = forward_direction;
            }

            spdlog::trace("max_move_dis is: {}", max_move_dis);

            std::optional<sf::Vector2f> next_move_dis; //< 碰撞后下一步移动的距离（如果有）
            std::optional<sf::Vector2f>
                             next_forward_direction; //< 碰撞后下一步的 `forward_direction` （如果有）

            shape::rectangle move_collision_rectangle{
                .position =
                    current.center - sf::Vector2f{0, current.radius}.rotatedBy(max_move_dis.angle()),
                .size = {max_move_dis.length(), 2 * current.radius},
                .rotation = max_move_dis.angle()}; //< 移动时的碰撞区域1
            shape::circle move_collision_circle{.center = current.center + max_move_dis,
                                                .radius = current.radius}; //< 移动时的碰撞区域2
            shape::line   line_to_get_collision_pos{
                  .start = {.position = current.center - max_move_dis.normalized() * current.radius},
                  .end = {.position = current.center
                                      + max_move_dis.normalized()
                                            * (1.f + current.radius)}}; //< 获得碰撞位置的参数之一
            for (auto it = [&] -> iterator {
                     auto tmp = list.begin();
                     return (tmp != list.end() && !is_in_same_scene(*tmp)) ? to_next(tmp) : tmp;
                 }();
                 it != list.end(); to_next(it)) {
                auto cur_shape = registry.get<shape>(*it); //< it的形状
                if (bool rect_collided = is_collided(move_collision_rectangle, cur_shape),
                    circle_collided = is_collided(move_collision_circle, cur_shape);
                    rect_collided || circle_collided) {
                    // 碰撞了，本轮移动到碰撞前

                    // debug message
                    spdlog::trace("current shape(entity:{}) collided with a shape(entity:{}), "
                                  "rectangle collided: {}, circle collided: {}",
                                  static_cast<entt::id_type>(entity), static_cast<entt::id_type>(*it),
                                  rect_collided, circle_collided);

                    std::visit(
                        [&](const auto &line) {
                            using type = std::decay_t<decltype(line)>;
                            if constexpr (std::is_same_v<type, shape::line>) {
                                // 获取更新后的位置
                                sf::Vector2f pos = [&] {
                                    using namespace sf::Literals;
                                    sf::Vector2f collided_position =
                                        get_collided_position(line, line_to_get_collision_pos);
                                    sf::Vector2f line_vec = line.end.position - line.start.position;
                                    if (line_vec.y < 0) {
                                        line_vec = -line_vec;
                                    }
                                    return collided_position
                                           - (max_move_dis.normalized()
                                              * (current.radius
                                                 / std::sin(wrap_to_first(line_vec.angleTo(max_move_dis))
                                                                .asRadians())));
                                }();
                                auto move_dis = pos - current.center;
                                if (!next_move_dis.has_value()
                                    || fast_floatf{next_move_dis->lengthSquared()}
                                           > fast_floatf{move_dis.lengthSquared()}) {
                                    next_move_dis = move_dis;
                                    next_forward_direction = line.end.position - line.start.position;
                                }
                            } else {
                                ASSERT_MSG(
                                    false,
                                    "编号为{}的实体的形状不是 `tnrw::ecs::shape::line` "
                                    "类型，目前移动时的碰撞处理仅支持 `tnrw::ecs::shape::line` 类型",
                                    static_cast<entt::id_type>(*it));
                            }
                        },
                        cur_shape.shape);
                }
            }
            if (next_move_dis.has_value()) {
                // 有碰撞时的移动
                auto forward_rest = forward_dis
                                    - (forward_dis.normalized()
                                       * (next_move_dis->length()
                                          / std::cos(next_move_dis->angleTo(forward_dis).asRadians())));
                current.center += *next_move_dis;
                spdlog::trace("The next position is: {}", current.center);
                self(self, forward_rest, *next_forward_direction);
                return;
            }
            // 无碰撞时的移动
            current.center += max_move_dis;
            sf::Vector2f forward_rest = forward_dis
                                        - (forward_dis.normalized() * max_move_dis.length()
                                           / std::cos(forward_dis.angleTo(max_move_dis).asRadians()));
            self(self, forward_rest, forward_rest);
        };

        auto forward_dis =
            static_cast<float>(delta_time.count()) * velocity(registry, entity); //< 前进的距离

        if (fast_floatf{forward_dis.lengthSquared()} > fast_floatf{0}) {
            move(move, forward_dis, forward_dis);
            // debug message
            spdlog::trace("current shape(entity:{})'s position is: {}, update delta time is: {}",
                          static_cast<entt::id_type>(entity),
                          std::get<shape::circle>(registry.get<shape>(entity).shape).center, delta_time);
        }
    }
    void movement_system::update_with_velocity(entt::registry           &registry,
                                               std::chrono::milliseconds delta_time) noexcept {
        auto list = registry.view<shape, struct velocity, should_collide, father_scenes>();
        std::ranges::for_each(list, [&](entt::entity entity) -> void {
            if (std::holds_alternative<shape::circle>(registry.get<shape>(entity).shape)) {
                update_with_velocity(registry, entity, delta_time);
            }
        });
    }

} // namespace tnrw::ecs