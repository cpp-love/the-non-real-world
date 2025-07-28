/**
 * @file functions.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 数学功能函数的声明
 * @version 0.1.0-1
 * @date 2025-07-18
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 声明了最大公约数和最小公倍数的函数模板，适用于非负整数类型
 * （支持自定义类需满足：
 *   1. 支持 `std::swap` 交换且行为正常
 *   2. 有对其类对象和内置整数类型（或内置整数类型可隐式转换的类型）的 `operator!=` 重载且行为正常
 *   3. 其类对象可复制构造且行为正常
 *   4. 有对其类对象和其类另一对象的 `operator%` 重载且行为正常）
 * 
 */

#ifndef __MATHS_FUNCTIONS_HPP__
#define __MATHS_FUNCTIONS_HPP__

namespace tnrw {

    namespace Maths {

        /**
         * @brief 求最大公约数的函数模板
         * @tparam UnsignedIntegerType 非负整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return UnsignedIntegerType 求得的最大公约数
         * @note 当 `a == 0 || b == 0` 时，函数返回 `0`
         */
        template <typename UnsignedIntegerType>
        UnsignedIntegerType gcd(UnsignedIntegerType a, UnsignedIntegerType b) noexcept {
            // 确保始终 `a > b` ，加速循环
            if (a < b)
                std::swap(a, b);
            // 处理特殊情况
            if (b == 0)
                return 0;
            do {
                UnsignedIntegerType t = b;
                b = a % b;
                a = t;
            } while (b != 0);
            return a;
        }

        /**
         * @brief 求最小公倍数的函数模板
         * @tparam UnsignedIntegerType 非负整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return UnsignedIntegerType 求得的最大公约数
         * @note 当 `a == 0 || b == 0` 时，函数返回 `0`
         */
        template <typename UnsignedIntegerType>
        UnsignedIntegerType lcm(UnsignedIntegerType a, UnsignedIntegerType b) noexcept {
            // 确保始终 `a > b` ，加速循环
            if (a < b)
                std::swap(a, b);
            // 处理特殊情况
            if (b == 0)
                return 0;
            return a / gcd(a, b) * b;
        }

    } // namespace Maths

} // namespace tnrw

#endif // __MATHS_FUNCTIONS_HPP__