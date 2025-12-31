/**
 * @file string_convert.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了基于UTF-8的字符（串）转换函数
 * @version 0.1.0-1
 * @date 2025-08-10
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_STRING_CONVERT_HPP
#define TNRW_BASE_STRING_CONVERT_HPP

#include <bit>
#include <concepts>
#include <string>
#include <string_view>

/// @brief 字符（串）转换命名空间
namespace tnrw::string_convert {

    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] character 字符
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string toUtf8String(char character) { return {character}; }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] character 字符
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string toUtf8String(char8_t character) {
        return {static_cast<char>(character)};
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] strview 字符串视图
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string toUtf8String(std::string_view strview) {
        return std::string(strview);
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] strview 字符串视图
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string toUtf8String(std::u8string_view strview) {
        // NOLINTNEXTLINE(bugprone-bitwise-pointer-cast)
        return {std::bit_cast<const char *>(strview.data()), strview.size()};
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] str C风格的字符串
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string toUtf8String(const char *str) {
        return (str != nullptr) ? toUtf8String(std::string_view(str)) : "(null)";
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] str C风格的字符串
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string toUtf8String(const char8_t *str) {
        return (str != nullptr) ? toUtf8String(std::u8string_view(str)) : "(null)";
    }

    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] strview 字符串视图
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view toUtf8StringView(std::string_view strview) { return strview; }
    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] strview 字符串视图
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view toUtf8StringView(std::u8string_view strview) {
        // NOLINTNEXTLINE(bugprone-bitwise-pointer-cast)
        return {std::bit_cast<const char *>(strview.data()), strview.size()};
    }
    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] str C风格的字符串
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view toUtf8StringView(const char *str) {
        return (str != nullptr) ? toUtf8StringView(std::string_view(str)) : "(null)";
    }
    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] str C风格的字符串
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view toUtf8StringView(const char8_t *str) {
        return (str != nullptr) ? toUtf8StringView(std::u8string_view(str)) : "(null)";
    }

    /**
     * @brief 支持转换字符串的概念约束
     * @tparam T 类型
     */
    template <typename T>
    concept Convertible = requires(const T &value) {
        { toUtf8String(value) } -> std::same_as<std::string>;
        { toUtf8StringView(value) } -> std::same_as<std::string_view>;
    };

} // namespace tnrw::string_convert

#endif // TNRW_BASE_STRING_CONVERT_HPP