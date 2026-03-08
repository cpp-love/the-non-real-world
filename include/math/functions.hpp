/**
 * @file functions.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加了与数学相关的辅助函数
 * @version 0.1.0-3
 * @date 2026-03-08
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_MATH_FUNCTIONS_HPP
#define TNRW_MATH_FUNCTIONS_HPP

#include "base/floating_point_compare.hpp"
#include <array>
#include <cmath>
#include <compare>
#include <concepts>
#include <optional>

namespace tnrw::math {

    /**
     * @brief 求解一元二次方程
     * @tparam T 浮点类型
     * @param [in] quadratic_coeff 二次项系数
     * @param [in] linear_coeff 一次项系数
     * @param [in] constant_term 常数项
     * @return constexpr std::array<std::optional<T>, 2> 方程的可能的两个解
     * @attention 此函数会处理浮点误差
     * @note 若 `quadratic_coeff` 接近 `0`，则退化为一元一次方程解决
     * @warning 若 `quadratic_coeff`、`linear_coeff`、`constant_term` 均接近 `0`，
     *          此函数认为无解，返回 `{std::nullopt, std::nullopt}`
     */
    template <std::floating_point T>
    constexpr std::array<std::optional<T>, 2> solve_quadratic_equation(T quadratic_coeff, T linear_coeff,
                                                                       T constant_term) {
        if (quadratic_coeff == 0) {
            // 退化为一元一次方程
            if (linear_coeff == 0) {
                // 认为无解，虽然也可能有无数解
                return {};
            }
            return {constant_term / linear_coeff};
        }
        float delta = (linear_coeff * linear_coeff) - (4 * quadratic_coeff * constant_term);
        auto  compare_res = no_nan_inf_f{delta} <=> no_nan_inf_f{0};
        if (compare_res > 0) {
            float sqrt_delta = std::sqrt(delta);
            return {(-linear_coeff + sqrt_delta) / (2 * quadratic_coeff),
                    (-linear_coeff - sqrt_delta) / (2 * quadratic_coeff)};
        }
        if (compare_res == 0) {
            return {-linear_coeff / (2 * quadratic_coeff)};
        }
        return {};
    }

} // namespace tnrw::math

#endif // TNRW_MATH_FUNCTIONS_HPP