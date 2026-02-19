/**
 * @file expressions_base.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了一些数学的基本元素
 * @version 0.1.0-2
 * @date 2026-01-17
 * 
 * @copyright cpp-love
 * 
 * @details 声明了 `integer_constant_type` 和 `variable_type` 来表示常量与变量类型
 * 
 */

#ifndef TNRW_MATH_EXPRESSIONS_BASE_HPP
#define TNRW_MATH_EXPRESSIONS_BASE_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

namespace tnrw {

    namespace math {

        namespace details {
            /**
             * @brief  `Pimpl` 惯用法实现 `algebraic_expression` 和 `numeric_expression` 类的私有封装（前向声明）
             * @details
             * - 所有私有成员都隐藏在此结构体的完整定义中，
             *   具体定义见源文件 @ref `Expressions.cpp`
             * - 此结构体也是代数式树的节点定义
             * @warning 该结构体是私有的，用户不应直接访问
             */
            struct node;
            using node_ptr = std::unique_ptr<node>; ///< 节点的智能指针的别名
        } // namespace details

        // using别名
        using integer_constant_type = long long; ///< 常量值类型
        using variable_type = std::string;       ///< 变量字符串类型
        using variable_view = std::string_view;  ///< 变量字符串视图类型

    } // namespace math

    inline namespace literals {

        /// @brief `algebraic_expression` 和 `numeric_expression` 类的内联自定义字面量命名空间
        inline namespace expressions_base_literals {

            /**
             * @brief 创建常量
             * @param [in] constant 常量值字面量
             * @return math::integer_constant_type 创建的常量类型
             */
            [[nodiscard]] constexpr math::integer_constant_type
                                              operator""_c(unsigned long long constant) noexcept;
            /**
             * @brief 创建变量
             * @param [in] variable 变量字符串字面量
             * @param [in] len 变量字符串字面量的长度
             * @return math::variable_type 创建的变量类型
             */
            [[nodiscard]] math::variable_type operator""_v(const char *variable,
                                                           std::size_t len) noexcept;
            /**
             * @brief 创建变量视图
             * @param [in] variable 变量字符串字面量
             * @param [in] len 变量字符串字面量的长度
             * @return math::variable_view 创建的变量视图
             */
            [[nodiscard]] math::variable_view operator""_vv(const char *variable,
                                                            std::size_t len) noexcept;

        } // namespace expressions_base_literals

    } // namespace literals

} // namespace tnrw

#endif // TNRW_MATH_EXPRESSIONS_BASE_HPP