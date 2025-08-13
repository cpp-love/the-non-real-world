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

#ifndef __BASE_STRING_CONVERT_HPP__
#define __BASE_STRING_CONVERT_HPP__

#include <format>
#include <string>
#include <string_view>

namespace tnrw {

    /// @brief 字符（串）转换命名空间
    namespace string_convert {

        /**
         * @brief 转换到UTF-8编码的字符串
         * @param [in] c 字符
         * @return std::string UTF-8编码的字符串
         */
        [[nodiscard]] inline std::string to_utf8_string(char c) { return std::string(1, c); }
        /**
         * @brief 转换到UTF-8编码的字符串
         * @param [in] c 字符
         * @return std::string UTF-8编码的字符串
         */
        [[nodiscard]] inline std::string to_utf8_string(char8_t c) {
            return std::string(1, static_cast<char>(c));
        }
        /**
         * @brief 转换到UTF-8编码的字符串
         * @param [in] sv 字符串视图
         * @return std::string UTF-8编码的字符串
         */
        [[nodiscard]] inline std::string to_utf8_string(std::string_view sv) { return std::string(sv); }
        /**
         * @brief 转换到UTF-8编码的字符串
         * @param [in] sv 字符串视图
         * @return std::string UTF-8编码的字符串
         */
        [[nodiscard]] inline std::string to_utf8_string(std::u8string_view sv) {
            return std::string(reinterpret_cast<const char *>(sv.data()), sv.size());
        }
        /**
         * @brief 转换到UTF-8编码的字符串
         * @param [in] s C风格的字符串
         * @return std::string UTF-8编码的字符串
         */
        [[nodiscard]] inline std::string to_utf8_string(const char *s) {
            return s ? to_utf8_string(std::string_view(s)) : "(null)";
        }
        /**
         * @brief 转换到UTF-8编码的字符串
         * @param [in] s C风格的字符串
         * @return std::string UTF-8编码的字符串
         */
        [[nodiscard]] inline std::string to_utf8_string(const char8_t *s) {
            return s ? to_utf8_string(std::u8string_view(s)) : "(null)";
        }

        /**
         * @brief 转换到UTF-8编码的字符串视图
         * @param [in] sv 字符串视图
         * @return std::string_view UTF-8编码的字符串视图
         */
        [[nodiscard]] inline std::string_view to_utf8_string_view(std::string_view sv) { return sv; }
        /**
         * @brief 转换到UTF-8编码的字符串视图
         * @param [in] sv 字符串视图
         * @return std::string_view UTF-8编码的字符串视图
         */
        [[nodiscard]] inline std::string_view to_utf8_string_view(std::u8string_view sv) {
            return std::string_view(reinterpret_cast<const char *>(sv.data()), sv.size());
        }
        /**
         * @brief 转换到UTF-8编码的字符串视图
         * @param [in] s C风格的字符串
         * @return std::string_view UTF-8编码的字符串视图
         */
        [[nodiscard]] inline std::string_view to_utf8_string_view(const char *s) {
            return s ? to_utf8_string_view(std::string_view(s)) : "(null)";
        }
        /**
         * @brief 转换到UTF-8编码的字符串视图
         * @param [in] s C风格的字符串
         * @return std::string_view UTF-8编码的字符串视图
         */
        [[nodiscard]] inline std::string_view to_utf8_string_view(const char8_t *s) {
            return s ? to_utf8_string_view(std::u8string_view(s)) : "(null)";
        }

        /**
         * @brief 支持转换字符串的概念约束
         * @tparam T 类型
         */
        template <typename T>
        concept Convertible = requires(const T &v) {
            { to_utf8_string(v) } -> std::convertible_to<std::string>;
        };

    } // namespace string_convert

} // namespace tnrw
#endif // __BASE_STRING_CONVERT_HPP__