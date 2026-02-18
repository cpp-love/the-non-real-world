/**
 * @file sfml_formatter.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加第三方库SFML对 `std::format` 的支持
 * @version 0.1.0-1
 * @date 2025-12-07
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_SFML_FORMATTER_HPP
#define TNRW_BASE_SFML_FORMATTER_HPP

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
    using FmtType = sf::Vector2<T>; ///< 格式化参数
    using CharType = CharT;         ///< 字符类型
    std::formatter<T, CharType> m_formatter_impl;
    /**
     * @brief 解析格式化参数的解析器
     * @tparam ParseCtx 解析的上下文类型
     * @param [in] ctx 上下文
     * @return ParseCtx::iterator 解析后的迭代器
     */
    template <typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
        return m_formatter_impl.parse(ctx);
    }
    /**
     * @brief 格式化器
     * @tparam ParseCtx 格式化的上下文类型
     * @param [in] vector 要格式化的对象
     * @param [in] ctx 上下文
     * @return FmtCtx::iterator 格式化后的迭代器
     */
    template <typename FmtCtx>
    typename FmtCtx::iterator format(const FmtType &vector, FmtCtx &ctx) const {
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
    using FmtType = sf::Vector3<T>; ///< 格式化参数
    using CharType = CharT;         ///< 字符类型
    std::formatter<T, CharT> m_formatter_impl;
    /**
     * @brief 解析格式化参数的解析器
     * @tparam ParseCtx 解析的上下文类型
     * @param [in] ctx 上下文
     * @return ParseCtx::iterator 解析后的迭代器
     */
    template <typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
        return m_formatter_impl.parse(ctx);
    }
    /**
     * @brief 格式化器
     * @tparam ParseCtx 格式化的上下文类型
     * @param [in] vector 要格式化的对象
     * @param [in] ctx 上下文
     * @return FmtCtx::iterator 格式化后的迭代器
     */
    template <typename FmtCtx>
    typename FmtCtx::iterator format(const FmtType &vector, FmtCtx &ctx) const {
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

#endif // TNRW_BASE_SFML_FORMATTER_HPP