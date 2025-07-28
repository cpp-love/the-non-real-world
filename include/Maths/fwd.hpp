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

#ifndef __MATHS_FWD_HPP__
#define __MATHS_FWD_HPP__

/**@defgroup Maths description
 * @brief 
 * @details 
 * @{
*/

/** @} Maths*/

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
        class AlgebraicExpression final;

        // NumericExpression.hpp
        class NumericExpression final;

        // functions.hpp
        template <typename UnsignedIntegerType>
        UnsignedIntegerType gcd(UnsignedIntegerType a, UnsignedIntegerType b) noexcept;
        template <typename UnsignedIntegerType>
        UnsignedIntegerType lcm(UnsignedIntegerType a, UnsignedIntegerType b) noexcept;

    } // namespace Maths

    namespace literals {

        // Expressions_base.hpp
        inline namespace Expressions_base_literals {

            Maths::ConstantType operator""_c(const unsigned long long constant) noexcept;
            Maths::VariableType operator""_v(const char variable) noexcept;

        } // namespace Expressions_base_literals

        // Algebraicexpression.hpp
        inline namespace AlgebraicExpression_literals {

            Maths::AlgebraicExpression
            operator""_cAlgeExpr(const unsigned long long constant) noexcept;
            Maths::AlgebraicExpression operator""_vAlgeExpr(const char variable) noexcept;

        } // namespace AlgebraicExpression_literals

        // NumericExpression.hpp
        inline namespace NumericExpression_literals {

            Maths::NumericExpression
            operator""_cNumExpr(const unsigned long long constant) noexcept;

        } // namespace NumericExpression_literals

    } // namespace literals

} // namespace tnrw

#endif // __MATHS_FWD_HPP__