/**
 * @file floating_point_compare.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了浮点数的比较运算（包含误差）
 * @version 0.1.0-3
 * @date 2026-03-08
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_FLOATING_POINT_COMPARE_HPP
#define TNRW_BASE_FLOATING_POINT_COMPARE_HPP

#include "base/assert_msg.hpp"
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
            using value_type = T;                                                ///< 浮点数值类型
            value_type                  value;                                   ///< 浮点数值
            static constexpr value_type epsilon = static_cast<value_type>(5e-3); ///< 浮点比较最小阈值

            // 构造、赋值、析构
            /**
             * @brief 构造一个浮点包装类
             * @param [in] rhs 浮点数值
             */
            constexpr explicit float_wrapper(value_type rhs) noexcept : value(rhs) {}

            /**
             * @brief 转换到原始类型( `ValueType` )的运算符重载
             * @return ValueType 转换后的类型
             * @details 与使用 `.value` 获取相同
             */
            constexpr explicit operator value_type() const noexcept { return value; }
        };

    } // namespace details

    template <std::floating_point T>
    struct has_nan_inf; // 前向声明

    /**
     * @brief 不含 `Inf` 或 `NaN` 的浮点包装类
     * @tparam T 原始浮点类型
     */
    template <std::floating_point T>
    struct no_nan_inf : public details::float_wrapper<T> {
      private:
        // using别名
        using base = details::float_wrapper<T>; ///< 基类别名

      public:
        // using解决模板依赖
        using typename base::value_type;
        using has_nan_inf_type = has_nan_inf<value_type>;

        // 构造、赋值、析构
        /**
         * @brief 构造一个浮点包装类
         * @param [in] rhs 浮点数值
         * @warning 如果 `rhs` 是 `NaN` 或 `Inf`，可能会触发断言
         */
        constexpr explicit no_nan_inf(value_type rhs) noexcept : base(rhs) {
            TNRW_ASSERT_MSG(std::isfinite(rhs),
                            "no_nan_inf<T> cannot be constructed from the values `NaN` or `Inf`");
        }
        /**
         * @brief 构造一个浮点包装类
         * @param [in] rhs 允许含有 `NaN` 或 `Inf` 的包装类
         * @warning 如果 `rhs` 是 `NaN` 或 `Inf`，可能会触发断言
         */
        explicit no_nan_inf(has_nan_inf_type rhs) noexcept : base(rhs.value) {
            TNRW_ASSERT_MSG(std::isfinite(rhs.value),
                            "no_nan_inf<T> cannot be constructed from the values `NaN` or `Inf`");
        }
    };
    using no_nan_inf_f = no_nan_inf<float>;

    /**
     * @brief 可以含 `Inf` 或 `NaN` 的浮点包装类
     * @tparam T 原始浮点类型
     */
    template <std::floating_point T>
    struct has_nan_inf : public details::float_wrapper<T> {
      private:
        // using别名
        using base = details::float_wrapper<T>; ///< 基类别名

      public:
        // using解决模板依赖
        using typename base::value_type;
        using no_nan_inf_type = no_nan_inf<value_type>;

        // 构造、赋值、析构
        using base::base; // 直接继承基类的构造函数
        /**
         * @brief 构造一个浮点包装类
         * @param [in] rhs 不允许含有 `NaN` 或 `Inf` 的类型
         */
        constexpr explicit has_nan_inf(no_nan_inf_type rhs) noexcept : base(rhs.value) {}
    };
    using has_nan_inf_f = has_nan_inf<float>;

    /**
     * @brief @ref no_nan_inf 浮点包装类相等比较运算符重载
     * @tparam T1 浮点类型1
     * @tparam T2 浮点类型2
     * @param [in] lhs 左操作数
     * @param [in] rhs 右操作数
     * @return true 相等
     * @return false 不相等
     */
    template <std::floating_point T1, std::floating_point T2>
    constexpr bool operator==(no_nan_inf<T1> lhs, no_nan_inf<T2> rhs) {
        return std::abs(lhs.value - rhs.value) < no_nan_inf<decltype(lhs.value - rhs.value)>::epsilon;
    }

    /**
    * @brief @ref has_nan_inf 浮点数包装类比较运算符重载
    * @tparam T1 浮点类型1
    * @tparam T2 浮点类型2
    * @param [in] lhs 左操作数
    * @param [in] rhs 右操作数
     * @return true 相等
     * @return false 不相等
     */
    template <std::floating_point T1, std::floating_point T2>
    constexpr bool operator==(has_nan_inf<T1> lhs, has_nan_inf<T2> rhs) {
        if (std::isnan(lhs.value) || std::isnan(rhs.value)) {
            return false;
        }
        if (std::isinf(lhs.value) || std::isinf(rhs.value)) {
            return lhs.value == rhs.value;
        }
        return static_cast<no_nan_inf<T1>>(lhs) == static_cast<no_nan_inf<T2>>(rhs);
    }

    /**
    * @brief @ref no_nan_inf 浮点数包装类三路比较运算符重载
    * @tparam T1 浮点类型1
    * @tparam T2 浮点类型2
    * @param [in] lhs 左操作数
    * @param [in] rhs 右操作数
    * @return std::weak_ordering 比较结果
    */
    template <std::floating_point T1, std::floating_point T2>
    constexpr std::weak_ordering operator<=>(no_nan_inf<T1> lhs, no_nan_inf<T2> rhs) {
        if (std::abs(lhs.value - rhs.value) < no_nan_inf<decltype(lhs.value - rhs.value)>::epsilon) {
            return std::weak_ordering::equivalent;
        }
        return lhs.value < rhs.value ? std::weak_ordering::less : std::weak_ordering::greater;
    }

    /**
     * @brief @ref has_nan_inf 浮点数包装类三路比较运算符重载
     * @tparam T1 浮点类型1
     * @tparam T2 浮点类型2
     * @param [in] lhs 左操作数
     * @param [in] rhs 右操作数
     * @return std::paritial_ordering 比较结果
     */
    template <std::floating_point T1, std::floating_point T2>
    constexpr std::partial_ordering operator<=>(has_nan_inf<T1> lhs, has_nan_inf<T2> rhs) {
        if (!std::isfinite(lhs.value) || !std::isfinite(rhs.value)) {
            return lhs.value <=> rhs.value;
        }
        return static_cast<no_nan_inf<T1>>(lhs) <=> static_cast<no_nan_inf<T2>>(rhs);
    }

} // namespace tnrw

#endif // TNRW_BASE_FLOATING_POINT_COMPARE_HPP