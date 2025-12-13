/**
 * @file SFML_formatter.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加第三方库SFML对 `std::format` 的支持
 * @version 0.1.0-1
 * @date 2025-12-07
 * 
 * @copyright cpp-love
 * 
 */

#ifndef __TNRW_BASE_SFML_FORMATTER_HPP__
#define __TNRW_BASE_SFML_FORMATTER_HPP__

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <format>
#include <locale>

/**
 * @brief `std::formatter` 对 `sf::Vector2` 的偏特化，用于格式化
 * @tparam T `sf::Vector2` 的模板参数
 * @tparam CharT `std::formatter` 的输出字符参数
 * @details 格式化参数使用 `T` 类型的格式化参数，来格式化成员 `x` 和 `y`，
 *          格式化结果为： `(x, y)`
 */
template <typename T, typename CharT>
struct std::formatter<sf::Vector2<T>, CharT> {
    using FmtType = sf::Vector2<T>;
    using CharType = CharT;
    std::formatter<T, CharT> m_formatter_impl;
    template <typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
        return m_formatter_impl.parse(ctx);
    }
    template <typename FmtCtx>
    FmtCtx::iterator format(const FmtType &vector, FmtCtx &ctx) const {
        const auto &facet = std::use_facet<std::ctype<CharType>>(ctx.locale());
        *ctx.out() = facet.widen('(');
        ++ctx.out();
        ctx.out() = m_formatter_impl.format(vector.x, ctx);
        *ctx.out() = facet.widen(',');
        ++ctx.out();
        *ctx.out() = facet.widen(' ');
        ++ctx.out();
        ctx.out() = m_formatter_impl.format(vector.y, ctx);
        *ctx.out() = facet.widen(')');
        ++ctx.out();
        return ctx.out();
    }
};

/**
 * @brief `std::formatter` 对 `sf::Vector3` 的偏特化，用于格式化
 * @tparam T `sf::Vector3` 的模板参数
 * @tparam CharT `std::formatter` 的输出字符参数
 * @details 格式化参数使用 `T` 类型的格式化参数，来格式化成员 `x`、`y` 和 `z`，
 *          格式化结果为： `(x, y, z)`
 */
template <typename T, typename CharT>
struct std::formatter<sf::Vector3<T>, CharT> {
    using FmtType = sf::Vector3<T>;
    using CharType = CharT;
    std::formatter<T, CharT> m_formatter_impl;
    template <typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
        return m_formatter_impl.parse(ctx);
    }
    template <typename FmtCtx>
    FmtCtx::iterator format(const FmtType &vector, FmtCtx &ctx) const {
        auto facet = std::use_facet<std::ctype<CharType>>(ctx.locale());
        *ctx.out() = facet.widen('(');
        ++ctx.out();
        ctx.out() = m_formatter_impl.format(vector.x, ctx);
        *ctx.out() = facet.widen(',');
        ++ctx.out();
        *ctx.out() = facet.widen(' ');
        ++ctx.out();
        ctx.out() = m_formatter_impl.format(vector.y, ctx);
        *ctx.out() = facet.widen(',');
        ++ctx.out();
        *ctx.out() = facet.widen(' ');
        ++ctx.out();
        ctx.out() = m_formatter_impl.format(vector.z, ctx);
        *ctx.out() = facet.widen(')');
        ++ctx.out();
        return ctx.out();
    }
};

#endif // __TNRW_BASE_SFML_FORMATTER_HPP__