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
 * - 声明了最大公约数和最小公倍数的函数模板，适用于整数类型
 * （支持自定义类需满足：
 *   1. 支持 `std::swap` 交换且行为正常
 *   2. 有对其类对象和内置整数类型（或内置整数类型可隐式转换的类型）的 `operator!=` 重载且行为正常
 *   3. 其类对象可复制构造且行为正常
 *   4. 有对其类对象和其类另一对象的 `operator%` 重载且行为正常
 *   5. 类满足概念 `std::unsigned_integral` 或 `std::signed_integral`（有符号整型还需支持调用 `std::abs` 函数）
 *   6. 以上提到的函数、操作符重载等最好标记为 `noexcept` ）
 * @deprecated 因为与标准库中的std::gcd / std::lcm 重复，将要删除
 */

#ifndef __TNRW_MATHS_FUNCTIONS_HPP__
#define __TNRW_MATHS_FUNCTIONS_HPP__

#include <cmath>
#include <concepts>

namespace tnrw {

    namespace maths {

        /// @cond INTERNAL
        /// @brief `maths/function.hpp` 文件的一些功能的实现细节
        namespace FunctionsDetails {
            /**
         * @brief 求最大公约数的函数模板
         * @tparam UnsignedIntegerType 非负整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return UnsignedIntegerType 求得的最大公约数
         * @note 当 `a == 0 || b == 0` 时，函数返回 `0`
         * @warning 此函数仅为抽象层提供，外部不用直接访问
         */
            template <typename UnsignedIntegerType>
            [[nodiscard]] inline UnsignedIntegerType gcd(UnsignedIntegerType a,
                                                         UnsignedIntegerType b) noexcept {
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
         * @warning 此函数仅为抽象层提供，外部不用直接访问
         */
            template <typename UnsignedIntegerType>
            [[nodiscard]] inline UnsignedIntegerType lcm(UnsignedIntegerType a,
                                                         UnsignedIntegerType b) noexcept {
                // 确保始终 `a > b` ，加速循环
                if (a < b)
                    std::swap(a, b);
                // 处理特殊情况
                if (b == 0)
                    return 0;
                return a / gcd(a, b) * b;
            }
        } // namespace FunctionsDetails
        /// @endcond

        /**
         * @brief 求最大公约数的函数模板
         * @tparam UnsignedIntegerType 非负整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return UnsignedIntegerType 求得的最大公约数
         * @note 当 `a == 0 || b == 0` 时，函数返回 `0`
         */
        template <std::unsigned_integral UnsignedIntegerType>
        [[deprecated("与标准库重复，请使用 `std::gcd` 代替")]] [[nodiscard]] UnsignedIntegerType
        gcd(UnsignedIntegerType a, UnsignedIntegerType b) noexcept {
            return FunctionsDetails::gcd(a, b);
        }

        /**
         * @brief 求最小公倍数的函数模板
         * @tparam UnsignedIntegerType 非负整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return UnsignedIntegerType 求得的最大公约数
         * @note 当 `a == 0 || b == 0` 时，函数返回 `0`
         */
        template <std::unsigned_integral UnsignedIntegerType>
        [[deprecated("与标准库重复，请使用 `std::lcm` 代替")]] [[nodiscard]] UnsignedIntegerType
        lcm(UnsignedIntegerType a, UnsignedIntegerType b) noexcept {
            return FunctionsDetails::lcm(a, b);
        }

        /**
         * @brief 求最大公约数的函数模板
         * @tparam SignedIntegerType 有符号整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return SignedIntegerType 求得的最大公约数
         * @note 此函数将有符号类型取绝对值（ `std::abs` ）后转发给无符号整型的函数
         */
        template <std::signed_integral SignedIntegerType>
        [[deprecated("与标准库重复，请使用 `std::gcd` 代替")]] [[nodiscard]] SignedIntegerType
        gcd(SignedIntegerType a, SignedIntegerType b) noexcept {
            return FunctionsDetails::gcd(std::abs(a), std::abs(b));
        }

        /**
         * @brief 求最小公倍数的函数模板
         * @tparam SignedIntegerType 有符号整数类型
         * @param [in] a 操作数1
         * @param [in] b 操作数2
         * @return SignedIntegerType 求得的最小公倍数
         * @note 此函数将有符号类型取绝对值（ `std::abs` ）后转发给无符号整型的函数
         */
        template <std::signed_integral SignedIntegerType>
        [[deprecated("与标准库重复，请使用 `std::lcm` 代替")]] [[nodiscard]] SignedIntegerType
        lcm(SignedIntegerType a, SignedIntegerType b) noexcept {
            return FunctionsDetails::lcm(std::abs(a), std::abs(b));
        }

    } // namespace maths

} // namespace tnrw

#endif // __TNRW_MATHS_FUNCTIONS_HPP__