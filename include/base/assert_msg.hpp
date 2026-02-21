/**
 * @file assert_msg.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加支持自定义输出的assert断言
 * @version 0.1.0-3
 * @date 2026-02-21
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 输出格式：
 * ```plain
 * Assertion failed at <file>:<line>:<column (in function :<function>):
 * >> Expression: <expression>
 * >> Message: <message>
 * Stack trace:
 * frame #0 : <description> in line <line> in file <file>
 * ```
 */

#ifndef TNRW_BASE_ASSERT_MSG_HPP
#define TNRW_BASE_ASSERT_MSG_HPP

#include <format>
#include <iterator>
#include <optional>
#include <ranges>
#include <source_location>
#include <spdlog/spdlog.h>
#include <stacktrace>
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
     * @param [in] stack_trace 堆栈信息
     * @param [in] message 断言失败时输出的消息（如果没有，为 std::nullopt)
     */
    [[noreturn]] constexpr void assert_fail(std::string_view expr, const std::source_location &loc,
                                            const std::stacktrace     &stack_trace,
                                            std::optional<std::string> message = std::nullopt) {
        std::string stack_trace_message = [&stack_trace] {
            std::string str;
            // trace[0] is `assert_check`
            // 正常情况下是这样的，但是在优化情况下可能不稳定，所以不删除可能多余的堆栈信息
            for (const auto &[index, entry] : stack_trace | std::views::enumerate) {
                std::format_to(std::back_inserter(str), "frame #{} : {} in line {} in file {}\n", index,
                               entry.description(), entry.source_line(), entry.source_file());
            }
            return str;
        }();
        // 输出信息
        if (message != std::nullopt) {
            spdlog::critical("Assertion failed at {}:{}:{} (in function :{}):\n"
                             ">> Expression: {}\n"
                             ">> Message: {}\n"
                             "Stack trace:\n"
                             "{}",
                             loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr,
                             *message, stack_trace_message);
        } else {
            spdlog::critical("Assertion failed at {}:{}:{} (in function :{}):\n"
                             ">> Expression: {}\n"
                             "Stack trace:\n"
                             "{}",
                             loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr,
                             stack_trace_message);
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
        assert_fail(expr, loc, std::stacktrace::current(),
                    std::format(fmt, std::forward<Args>(args)...));
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
        assert_fail(expr, loc, std::stacktrace::current(),
                    std::format(fmt, std::forward<Args>(args)...));
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
        assert_fail(expr, loc, std::stacktrace::current());
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