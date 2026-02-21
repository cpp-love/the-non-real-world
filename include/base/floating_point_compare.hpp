/**
 * @file floating_point_compare.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了浮点数的比较运算（包含误差）
 * @version 0.1.0-1
 * @date 2025-11-15
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_FLOATING_POINT_COMPARE_HPP
#define TNRW_BASE_FLOATING_POINT_COMPARE_HPP

#include <cmath>
#include <compare>
#include <concepts>

namespace tnrw {

    namespace details {

        /**
         * @brief 浮点数包装器，用于提取 @ref tnrw::FastFloat 和 @ref tnrw::SafeFloat 的重复部分
         * @tparam T 要包装的浮点类型
         * @warning 该类是私有的，用户不应直接访问
         */
        template <std::floating_point T>
        struct float_wrapper {
            using value_type = T;                                               ///< 浮点数值类型
            value_type                  value;                                  ///< 浮点数值
            static constexpr value_type epsilon = static_cast<value_type>(0.5); ///< 浮点比较最小阈值

            // 构造、赋值、析构
            constexpr explicit float_wrapper(value_type rhs) noexcept : value(rhs) {}

            /**
             * @brief 转换到原始类型( `ValueType` )的运算符重载
             * @return ValueType 转换后的类型
             * @details 与使用 `.value` 获取类型相同
             */
            constexpr explicit operator value_type() const noexcept { return value; }
        };

    } // namespace details

    template <std::floating_point T>
    struct safe_float; // 前向声明

    /**
     * @brief 快速比较的浮点类型，不含非法值（ `inf` 和 `nan` ）
     * @tparam T 原始浮点类型
     */
    template <std::floating_point T>
    struct fast_float : public details::float_wrapper<T> {
      private:
        // using别名
        using base = details::float_wrapper<T>; ///< 基类别名

      public:
        // using解决模板依赖
        using base::base;
        using typename base::value_type;
        using safe_type = safe_float<value_type>;

        // 构造、赋值、析构
        constexpr explicit fast_float(safe_type rhs) noexcept : base(rhs.value) {}
    };
    using fast_floatf = fast_float<float>;

    /**
     * @brief 快速比较的浮点类型，含非法值（ `inf` 和 `nan` ）
     * @tparam T 原始浮点类型
     */
    template <std::floating_point T>
    struct safe_float : public details::float_wrapper<T> {
      private:
        // using别名
        using base = details::float_wrapper<T>; ///< 基类别名

      public:
        // using解决模板依赖
        using base::base;
        using typename base::value_type;
        using fast_type = fast_float<value_type>;

        // 构造、赋值、析构
        constexpr explicit safe_float(fast_type rhs) noexcept : base(rhs.value) {}
    };
    using safe_floatf = safe_float<float>;

    /**
     * @brief 浮点数相等比较运算符重载，不含非法值（ `inf` 和 `nan` ）
     * @tparam T1 浮点类型1
     * @tparam T2 浮点类型2
     * @param [in] lhs 左操作数
     * @param [in] rhs 右操作数
     * @return true 相等
     * @return false 不相等
     */
    template <std::floating_point T1, std::floating_point T2>
    constexpr bool operator==(fast_float<T1> lhs, fast_float<T2> rhs) {
        return std::abs(lhs.value - rhs.value) < fast_float<decltype(lhs.value - rhs.value)>::epsilon;
    }

    /**
    * @brief 浮点数相等比较运算符重载，含非法值（ `inf` 和 `nan` ）
    * @tparam T1 浮点类型1
    * @tparam T2 浮点类型2
    * @param [in] lhs 左操作数
    * @param [in] rhs 右操作数
     * @return true 相等
     * @return false 不相等
     */
    template <std::floating_point T1, std::floating_point T2>
    constexpr bool operator==(safe_float<T1> lhs, safe_float<T2> rhs) {
        if (std::isnan(lhs.value) || std::isnan(rhs.value)) {
            return false;
        }
        if (std::isinf(lhs.value) || std::isinf(rhs.value)) {
            return lhs.value == rhs.value;
        }
        return static_cast<fast_float<T1>>(lhs) == static_cast<fast_float<T2>>(rhs);
    }

    /**
    * @brief 浮点数三路比较运算符重载，不含非法值（ `inf` 和 `nan` ）
    * @tparam T1 浮点类型1
    * @tparam T2 浮点类型2
    * @param [in] lhs 左操作数
    * @param [in] rhs 右操作数
    * @return std::paritial_ordering 比较结果
    */
    template <std::floating_point T1, std::floating_point T2>
    constexpr std::partial_ordering operator<=>(fast_float<T1> lhs, fast_float<T2> rhs) {
        if (std::abs(lhs.value - rhs.value) < fast_float<decltype(lhs.value - rhs.value)>::epsilon) {
            return std::partial_ordering::equivalent;
        }
        return lhs.value <=> rhs.value;
    }

    /**
     * @brief 浮点数三路比较运算符重载，含非法值（ `inf` 和 `nan` ）
     * @tparam T1 浮点类型1
     * @tparam T2 浮点类型2
     * @param [in] lhs 左操作数
     * @param [in] rhs 右操作数
     * @return std::paritial_ordering 比较结果
     */
    template <std::floating_point T1, std::floating_point T2>
    constexpr std::partial_ordering operator<=>(safe_float<T1> lhs, safe_float<T2> rhs) {
        if (std::isnan(lhs.value) || std::isnan(rhs.value) || std::isinf(lhs.value)
            || std::isinf(rhs.value)) {
            return lhs.value <=> rhs.value;
        }
        return static_cast<fast_float<T1>>(lhs) <=> static_cast<fast_float<T2>>(rhs);
    }

} // namespace tnrw

#endif // TNRW_BASE_FLOATING_POINT_COMPARE_HPP