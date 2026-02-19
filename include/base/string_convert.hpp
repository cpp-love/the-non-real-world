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
    [[nodiscard]] inline std::string to_utf8_string(char character) { return {character}; }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] character 字符
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string to_utf8_string(char8_t character) {
        return {static_cast<char>(character)};
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] strview 字符串视图
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string to_utf8_string(std::string_view strview) {
        return std::string(strview);
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] strview 字符串视图
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string to_utf8_string(std::u8string_view strview) {
        // NOLINTNEXTLINE(bugprone-bitwise-pointer-cast)
        return {std::bit_cast<const char *>(strview.data()), strview.size()};
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] str C风格的字符串
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string to_utf8_string(const char *str) {
        return (str != nullptr) ? to_utf8_string(std::string_view(str)) : "(null)";
    }
    /**
     * @brief 转换到UTF-8编码的字符串
     * @param [in] str C风格的字符串
     * @return std::string UTF-8编码的字符串
     */
    [[nodiscard]] inline std::string to_utf8_string(const char8_t *str) {
        return (str != nullptr) ? to_utf8_string(std::u8string_view(str)) : "(null)";
    }

    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] strview 字符串视图
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view to_utf8_string_view(std::string_view strview) {
        return strview;
    }
    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] strview 字符串视图
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view to_utf8_string_view(std::u8string_view strview) {
        // NOLINTNEXTLINE(bugprone-bitwise-pointer-cast)
        return {std::bit_cast<const char *>(strview.data()), strview.size()};
    }
    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] str C风格的字符串
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view to_utf8_string_view(const char *str) {
        return (str != nullptr) ? to_utf8_string_view(std::string_view(str)) : "(null)";
    }
    /**
     * @brief 转换到UTF-8编码的字符串视图
     * @param [in] str C风格的字符串
     * @return std::string_view UTF-8编码的字符串视图
     */
    [[nodiscard]] inline std::string_view to_utf8_string_view(const char8_t *str) {
        return (str != nullptr) ? to_utf8_string_view(std::u8string_view(str)) : "(null)";
    }

    /**
     * @brief 概念：可以支持转换字符串的类型
     * @tparam T 类型
     */
    template <typename T>
    concept Convertible = requires(const T &value) {
        { to_utf8_string(value) } -> std::same_as<std::string>;
        { to_utf8_string_view(value) } -> std::same_as<std::string_view>;
    };

} // namespace tnrw::string_convert

#endif // TNRW_BASE_STRING_CONVERT_HPP