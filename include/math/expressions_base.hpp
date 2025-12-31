/**
 * @file expressions_base.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了一些数学的基本元素
 * @version 0.1.0-1
 * @date 2025-07-23
 * 
 * @copyright cpp-love
 * 
 * @details 声明了 `ConstantType` 和 `VariableType` 来表示常量与变量类型
 * 
 */

#ifndef TNRW_MATH_EXPRESSIONS_BASE_HPP
#define TNRW_MATH_EXPRESSIONS_BASE_HPP

namespace tnrw {

    namespace math {

        // using别名
        using ConstantType = long long; ///< 常量值类型
        using VariableType = char;      ///< 变量字符类型

    } // namespace math

    inline namespace literals {

        /// @brief Expressions类集( `AlgebraicExpression` 和 `NumericExpression` 类)的内联自定义字面量命名空间
        inline namespace expressions_base_literals {

            /**
             * @brief 创建常量
             * @param [in] constant 常量值字面量
             * @return math::ConstantType 创建的常量类型
             */
            [[nodiscard]] constexpr math::ConstantType
            operator""_c(unsigned long long constant) noexcept;
            /**
             * @brief 创建变量
             * @param [in] variable 变量字符字面量
             * @return math::VariableType 创建的变量类型
             */
            [[nodiscard]] constexpr math::VariableType operator""_v(char variable) noexcept;

        } // namespace expressions_base_literals

    } // namespace literals

} // namespace tnrw

#endif // TNRW_MATH_EXPRESSIONS_BASE_HPP