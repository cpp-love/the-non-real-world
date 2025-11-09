/**
 * @file fwd.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief Maths命名空间内容的前向声明头文件
 * @version 0.1.0-1
 * @date 2025-07-05
 * 
 * @copyright cpp-love
 * 
 */

#ifndef __TNRW_MATHS_FWD_HPP__
#define __TNRW_MATHS_FWD_HPP__

#include <concepts>

namespace tnrw {

    /**
     * @brief 数学相关的命名空间
     * @details
     * - 为了方便游戏的血量、攻击系统的开发，先行开发作为血条值的一些相关类
     * - 同时也为了方便游戏的开发，先行定义一些数学相关的函数
     */
    namespace Maths {

        // Expressions_base.hpp
        using ConstantType = long long;
        using VariableType = char;

        // Algebraicexpression.hpp
        class AlgebraicExpression;

        // NumericExpression.hpp
        class NumericExpression;

        // functions.hpp
        template <std::unsigned_integral UnsignedIntegerType>
        [[deprecated("与标准库重复，请使用 `std::gcd` 代替")]] [[nodiscard]] UnsignedIntegerType
        gcd(UnsignedIntegerType a, UnsignedIntegerType b) noexcept;
        template <std::unsigned_integral UnsignedIntegerType>
        [[deprecated("与标准库重复，请使用 `std::lcm` 代替")]] [[nodiscard]] UnsignedIntegerType
        lcm(UnsignedIntegerType a, UnsignedIntegerType b) noexcept;

    } // namespace Maths

    inline namespace literals {

        // Expressions_base.hpp
        inline namespace Expressions_base_literals {

            [[nodiscard]] constexpr Maths::ConstantType
            operator""_c(unsigned long long constant) noexcept;
            [[nodiscard]] constexpr Maths::VariableType operator""_v(char variable) noexcept;

        } // namespace Expressions_base_literals

        // Algebraicexpression.hpp
        inline namespace AlgebraicExpression_literals {

            [[nodiscard]] Maths::AlgebraicExpression
            operator""_cAlgeExpr(unsigned long long constant) noexcept;
            [[nodiscard]] Maths::AlgebraicExpression operator""_vAlgeExpr(char variable) noexcept;

        } // namespace AlgebraicExpression_literals

        // NumericExpression.hpp
        inline namespace NumericExpression_literals {

            [[nodiscard]] Maths::NumericExpression
            operator""_cNumExpr(unsigned long long constant) noexcept;

        } // namespace NumericExpression_literals

    } // namespace literals

} // namespace tnrw

#endif // __TNRW_MATHS_FWD_HPP__