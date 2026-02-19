/**
 * @file assert_msg.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加支持自定义输出的assert断言
 * @version 0.1.0-2
 * @date 2025-11-9
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_ASSERT_MSG_HPP
#define TNRW_BASE_ASSERT_MSG_HPP

#include <format>
#include <optional>
#include <source_location>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

#ifdef NDEBUG
#define assert_msg(expr, ...) ((void)0) //< NOLINT(readability-identifier-naming)
#else

/// @brief 一些内部实现，用户不应访问
/// @cond INTERNAL
namespace tnrw::details {
    /**
     * @brief 断言失败处理函数
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     * @param [in] message 断言失败时输出的消息（如果没有，为 std::nullopt)
     */
    [[noreturn]] constexpr void assert_fail(std::string_view expr, const std::source_location &loc,
                                            std::optional<std::string> message = std::nullopt) {
        // 输出信息
        if (message != std::nullopt) {
            spdlog::critical("Assertion failed at {}:{}:{} (in function :{}):\n>> Expression: "
                             "{}\n>> Message: {}",
                             loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr,
                             *message);
        } else {
            spdlog::critical("Assertion failed at {}:{}:{} (in function :{}):\n>> Expression: {}",
                             loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr);
        }

        // 及时刷新
        spdlog::default_logger()->flush();

        // 终止程序
        std::abort();
    }

    /**
     * @brief 断言检查函数
     * @tparam Args 格式化字符串参数类型
     * @param [in] condition 条件
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     * @param [in] fmt 格式化字符串
     * @param [in] args 格式化字符串参数
     */
    template <typename... Args>
    constexpr void assert_check(bool condition, std::string_view expr, const std::source_location &loc,
                                std::format_string<Args...> fmt, Args &&...args) {
        if (condition) {
            return;
        }
        assert_fail(expr, loc, std::format(fmt, std::forward<Args>(args)...));
    }
    /**
     * @brief 断言检查函数
     * @tparam Args 格式化字符串参数类型
     * @param [in] condition 条件
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     * @param [in] fmt 格式化字符串
     * @param [in] args 格式化字符串参数
     */
    template <typename... Args>
    constexpr void assert_check(bool condition, std::string_view expr, const std::source_location &loc,
                                std::wformat_string<Args...> fmt, Args &&...args) {
        if (condition) {
            return;
        }
        assert_fail(expr, loc, std::format(fmt, std::forward<Args>(args)...));
    }
    /**
     * @brief 断言检查函数
     * @param [in] condition 条件
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     */
    constexpr void assert_check(bool condition, std::string_view expr, const std::source_location &loc) {
        if (condition) {
            return;
        }
        assert_fail(expr, loc);
    }
} // namespace tnrw::details
/// @endcond

// NOLINTBEGIN(readability-identifier-naming)
/**
 * @brief 带消息的assert断言
 * @param [in] expr 断言表达式（需要可以隐式转化为 `bool` 类型或本来就是 `bool` 类型）
 * @param [in] other_params(...) 可选的断言失败时输出的消息，支持格式化
 * @note 格式化与标准库的 `std::format` 格式化相同
 * @note 当定义宏 `NDEBUG` 时与标准库的 `assert` 行为相同，都不启用
 */
#define assert_msg(expr, ...)                                                                           \
    ::tnrw::details::assert_check(expr, #expr,                                                          \
                                  std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)
// NOLINTEND(readability-identifier-naming)

#endif // NDEBUG

#endif // TNRW_BASE_ASSERT_MSG_HPP