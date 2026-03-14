/**
 * @file type_traits.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义一些标准库没有的 type traits
 * @version 0.1.0-1
 * @date 2026-03-08
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_TYPE_TRAITS_HPP
#define TNRW_BASE_TYPE_TRAITS_HPP

#include <variant>

namespace tnrw {

    /**
     * @brief 类型萃取：类型 `T` 是否为 变体( `std::variant` )类型 `V` 的成员
     * @tparam T 判断类型
     * @tparam V 变体类型
     */
    template <typename T, typename V>
    struct is_variant_member : std::false_type {};
    template <typename T, typename T1, typename... Rest>
    struct is_variant_member<T, std::variant<T1, Rest...>>
        : std::conditional_t<std::is_same_v<T, T1>, std::true_type,
                             is_variant_member<T, std::variant<Rest...>>> {};

    /**
     * @brief 类型萃取： @ref tnrw::math::details::IsVariantMember 的值的模板缩写
     * @tparam T 判断类型
     * @tparam V 变体类型
     */
    template <typename T, typename V>
    constexpr bool is_variant_member_v = is_variant_member<T, V>::value;

} // namespace tnrw

#endif // TNRW_BASE_TYPE_TRAITS_HPP