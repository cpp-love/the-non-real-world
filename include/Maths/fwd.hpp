/**
 * @file fwd.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 数学相关的前向声明头文件
 * @version 0.1.0-1
 * @date 2025-07-05
 * 
 * @copyright Copyright 2025 cpp-love
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

/**
 * @brief "The Non-real World" 游戏的命名空间
 * @details 此游戏的所有API都在此命名空间内
 */
namespace tnrw {

    /**
     * @brief 数学相关的命名空间
     * @details
     * - 为了方便游戏的血量、攻击系统的开发，先行开发作为血条值的一些相关类
     * - 同时也为了方便游戏的开发，先行定义一些数学相关的函数
     */
    namespace Maths {

        // Algebraicexpression.hpp
        class AlgebraicExpression final;

        // functions.hpp
        template <typename UnsignedIntegerType>
        UnsignedIntegerType gcd(UnsignedIntegerType a, UnsignedIntegerType b) noexcept;
        template <typename UnsignedIntegerType>
        UnsignedIntegerType lcm(UnsignedIntegerType a, UnsignedIntegerType b) noexcept;

    } // namespace Maths

    /// @brief tnrw命名空间内的自定义字面量命名空间
    namespace literals {

        // Algebraicexpression.hpp
        inline namespace AlgebraicExpression_literals {

            Maths::AlgebraicExpression
            operator""_cexpr(const unsigned long long constant) noexcept;
            Maths::AlgebraicExpression operator""_vexpr(const char variable) noexcept;

        } // namespace AlgebraicExpression_literals

    } // namespace literals

} // namespace tnrw

#endif // __MATHS_FWD_HPP__