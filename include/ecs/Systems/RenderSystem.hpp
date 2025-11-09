/**
 * @file RenderSystem.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了渲染系统及其基本功能
 * @version 0.1.0-1
 * @date 2025-10-18
 * 
 * @copyright cpp-love
 * 
 */

#ifndef __TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP__
#define __TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP__

#include "ecs/Components/ShapeComponents.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <entt/fwd.hpp>
#include <functional>

namespace tnrw::ecs {

    /// @brief 渲染系统
    class RenderSystem final {
      private:
        static std::size_t circle_point_count; ///< 绘制的圆会有的点数

        /**
         * @brief 绘制单个图形
         * @param [in] render 需要绘制的地方
         * @param [in] line 线图形
         */
        static void        drawer(sf::RenderTarget &render, const Shape::Line &line);
        /**
         * @brief 绘制单个图形
         * @param [in] render 需要绘制的地方
         * @param [in] circle 圆图形
         */
        static void        drawer(sf::RenderTarget &render, const Shape::Circle &circle);
        /**
         * @brief 绘制单个图形
         * @param [in] render 需要绘制的地方
         * @param [in] rect 矩形图形
         */
        static void        drawer(sf::RenderTarget &render, const Shape::Rectangle &rect);

      public:
        /**
         * @brief 获取绘制的圆会有的点数
         * @return std::size_t 绘制的圆会有的点数
         */
        [[nodiscard]] static std::size_t getCirclePointCount();
        /**
         * @brief 设置绘制的圆会有的点数
         * @param [in] new_cnt 新的绘制的圆会有的点数
         */
        static void                      setCirclePointCount(std::size_t new_cnt);

        /**
         * @brief 绘制注册表里的内容
         * @param [in] render 需要渲染的地方
         * @param [in] registry 注册表
         * @param [in] checkIfVaild 检查是否有资格渲染，不需要检查 @ref tnrw::ecs::ShouldRender 组件是否存在
         * @note 此函数会自动帮你忽略没有 @ref tnrw::ecs::ShouldRender 的组件
         * @note 此函数不会调用 `render.clear()`，需要使用者手动调用
         */
        static void                      draw(const entt::registry &registry, sf::RenderTarget &render,
                                              std::function<bool(entt::entity)> checkIfVaild);
    };

} // namespace tnrw::ecs

#endif // __TNRW_ECS_SYSTEM_RENDER_SYSTEM_HPP__