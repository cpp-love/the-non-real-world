/**
 * @file fwd.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief math命名空间内容的前向声明头文件
 * @version 0.1.0-1
 * @date 2025-07-05
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_MATH_FWD_HPP
#define TNRW_MATH_FWD_HPP

#include <concepts>

namespace tnrw {

    /**
     * @brief 数学相关的命名空间
     * @details
     * - 为了方便游戏的血量、攻击系统的开发，先行开发作为血条值的一些相关类
     * - 同时也为了方便游戏的开发，先行定义一些数学相关的函数
     */
    namespace math {

        // expressions_base.hpp
        using ConstantType = long long;
        using VariableType = char;

        // Algebraicexpression.hpp
        class AlgebraicExpression;

        // NumericExpression.hpp
        class NumericExpression;

    } // namespace math

    inline namespace literals {

        // expressions_base.hpp
        inline namespace expressions_base_literals {

            [[nodiscard]] constexpr math::ConstantType
            operator""_c(unsigned long long constant) noexcept;
            [[nodiscard]] constexpr math::VariableType operator""_v(char variable) noexcept;

        } // namespace expressions_base_literals

        // Algebraicexpression.hpp
        inline namespace algebraic_expression_literals {

            [[nodiscard]] math::AlgebraicExpression
            operator""_cAlgeExpr(unsigned long long constant) noexcept;
            [[nodiscard]] math::AlgebraicExpression operator""_vAlgeExpr(char variable) noexcept;

        } // namespace algebraic_expression_literals

        // NumericExpression.hpp
        inline namespace numeric_expression_literals {

            [[nodiscard]] math::NumericExpression
            operator""_cNumExpr(unsigned long long constant) noexcept;

        } // namespace numeric_expression_literals

    } // namespace literals

} // namespace tnrw

#endif // TNRW_MATH_FWD_HPP