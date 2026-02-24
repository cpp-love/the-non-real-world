/**
 * @file assert_msg.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加 支持自定义消息的 `assert` 和 支持自定义消息的 `std::unreachable`
 * @version 0.1.0-5
 * @date 2026-02-24
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_ASSERT_MSG_HPP
#define TNRW_BASE_ASSERT_MSG_HPP

#include <concepts>
#include <exception>
#include <format>
#include <iterator>
#include <optional>
#include <ranges>
#include <source_location>
#include <spdlog/spdlog.h>
#include <stacktrace>
#include <string>
#include <string_view>
#include <utility>

/// @cond INTERNAL
/// @brief 一些内部实现，用户不应访问
namespace tnrw::details {

    /**
     * @brief 获取堆栈信息（默认为当前）
     * @param [in] stack_trace 堆栈对象，默认为当前堆栈
     * @return std::string 堆栈信息
     */
    [[nodiscard]] std::string
    get_stack_trace_message(const std::stacktrace &stack_trace = std::stacktrace::current()) noexcept {
        try {
            std::string str;
            for (const auto &[index, entry] : stack_trace | std::views::enumerate) {
                std::format_to(std::back_inserter(str), "frame #{} : {} in line {} in file {}\n", index,
                               entry.description(), entry.source_line(), entry.source_file());
            }
            return str;
        } catch (std::exception &exception) {
            return "Due to the following exception, we lost stack trace message.\n"
                   + std::string(exception.what());
        } catch (...) { return "Due to the unknown exception, we lost stack trace message.\n"; }
    }

    /**
     * @brief 断言失败处理函数
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     * @param [in] message 断言失败时输出的消息（默认为 `std::nullopt`)
     * @param [in] stack_trace 堆栈信息（默认为当前堆栈）
     */
    [[noreturn]] constexpr void
    assert_fail(std::string_view expr, const std::source_location &loc,
                std::optional<std::string> message = std::nullopt,
                const std::stacktrace     &stack_trace = std::stacktrace::current()) noexcept {
        std::string stack_trace_message = get_stack_trace_message(stack_trace);
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
                                std::format_string<Args...> fmt, Args &&...args) noexcept {
        if (condition) {
            return;
        }
        try {
            assert_fail(expr, loc, std::format(fmt, std::forward<Args>(args)...));
        } catch (std::exception &exception) {
            assert_fail(expr, loc,
                        "Due to the following exception, we lost assertion message.\n"
                            + std::string(exception.what()));
        } catch (...) {
            assert_fail(expr, loc, "Due to the unknown exception, we lost assertion message.\n");
        }
    }

    /**
     * @brief 断言检查函数
     * @param [in] condition 条件
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     * @param [in] message 运行时消息（如果有）
     */
    constexpr void assert_check(bool condition, std::string_view expr, const std::source_location &loc,
                                std::optional<std::string> message = std::nullopt) noexcept {
        if (condition) {
            return;
        }
        assert_fail(expr, loc, std::move(message));
    }

    template <typename T>
    struct with_debug_info {
        T                    value;
        std::source_location loc;
        std::stacktrace      stack_trace;

        // NOLINTBEGIN(hicpp-explicit-conversions)
        /**
         * @brief 构造一个包含 `T` 类型的对象
         * @tparam U 值的类型
         * @param [in] val 值
         * @param [in] location 源码位置（可忽略，默认为调用的位置）
         * @param [in] trace 堆栈信息（可忽略，默认为调用处的堆栈）
         */
        template <typename U>
            requires std::constructible_from<T, U>
        with_debug_info(U &&val, std::source_location location = std::source_location::current(),
                        std::stacktrace trace = std::stacktrace::current())
            : value(std::forward<U>(val)), loc(location), stack_trace(std::move(trace)) {}
        // NOLINTEND(hicpp-explicit-conversions)
        /**
         * @brief 默认复制构造函数
         * @param [in] rhs 另一个对象
         */
        with_debug_info(const with_debug_info &rhs) = default;
        /**
         * @brief 默认移动构造函数
         * @param [in] rhs 另一个对象
         */
        with_debug_info(with_debug_info &&rhs) = default;
        /**
         * @brief 默认复制赋值运算符重载
         * @param [in] rhs 另一个对象
         * @return with_debug_info& 当前对象的引用(`*this`)
         */
        with_debug_info &operator=(const with_debug_info &rhs) = default;
        /**
         * @brief 默认移动赋值运算符重载
         * @param [in] rhs 另一个对象
         * @return with_debug_info& 当前对象的引用(`*this`)
         */
        with_debug_info &operator=(with_debug_info &&rhs) = default;
        /// @brief 默认析构函数
        ~with_debug_info() = default;
    };

} // namespace tnrw::details
/// @endcond

namespace tnrw {

    /**
     * @brief 带消息的 `std::unreachable`（执行到正常情况无法到达的位置时的处理函数）
     * @param [in] message 若执行到不该执行的的此处时输出的消息（默认为空，即 `std::nullopt`)
     * @details
     * - 当定义宏 `NDEBUG` 时，函数只调用 `std::unreachable()`，**不输出消息**，若到达 **会导致UB**；
     *   未定义宏 `NDEBUG` 时，会输出消息，行为定义（调用 `std::abort()` 终止函数）
     * - 输出格式：
     * ```plain
     * Control reached an unreachable place at <file>:<line>:<column> (in function :<function>):
     * >> Message: <message>
     * Stack trace:
     * frame #0 : <description> in line <line> in file <file>
     * ```
     * @warning 此函数会直接调用 `std::abort()` / `std::unreachable()`，不会返回，请确认调用处是否无法到达
     */
    [[noreturn]] constexpr void
    unreachable([[maybe_unused]] details::with_debug_info<std::optional<std::string>> message =
                    std::nullopt) noexcept {
#ifdef NEBUG
        std::unreachable();
#else
        std::string stack_trace_message = details::get_stack_trace_message(message.stack_trace);
        // 输出信息
        if (message.value != std::nullopt) {
            spdlog::critical("Control reached an unreachable place at {}:{}:{} (in function :{}):\n"
                             ">> Message: {}\n"
                             "Stack trace:\n"
                             "{}",
                             message.loc.file_name(), message.loc.line(), message.loc.column(),
                             message.loc.function_name(), *message.value, stack_trace_message);
        } else {
            spdlog::critical("Control reached an unreachable place at {}:{}:{} (in function :{}):\n"
                             "Stack trace:\n"
                             "{}",
                             message.loc.file_name(), message.loc.line(), message.loc.column(),
                             message.loc.function_name(), stack_trace_message);
        }

        // 及时刷新
        spdlog::default_logger()->flush();

        // 终止程序
        std::abort();
#endif // NDEBUG
    }

    /**
     * @brief 带消息的 `std::unreachable`（执行到正常情况无法到达的位置时的处理函数）
     * @tparam Args 格式化字符串参数类型
     * @param [in] fmt 格式化字符串
     * @param [in] args 格式化字符串参数
     * @details
     * - 当定义宏 `NDEBUG` 时，函数只调用 `std::unreachable()`，**不输出消息**，若到达 **会导致UB**；
     *   未定义宏 `NDEBUG` 时，会输出消息，行为定义（调用 `std::abort()` 终止函数）
     * - 输出格式：
     * ```plain
     * Control reached an unreachable place at <file>:<line>:<column> (in function :<function>):
     * >> Message: <message>
     * Stack trace:
     * frame #0 : <description> in line <line> in file <file>
     * ```
     * @warning 此函数会直接调用 `std::abort()` / `std::unreachable()`，不会返回，请确认调用处是否无法到达
     */
    template <typename... Args>
    [[noreturn]] constexpr void
    unreachable([[maybe_unused]] details::with_debug_info<std::format_string<Args...>> fmt,
                [[maybe_unused]] Args &&...args) noexcept {
#ifdef NEBUG
        std::unreachable();
#else
        unreachable(details::with_debug_info<std::optional<std::string>>{
            std::format(fmt.value, std::forward<Args>(args)...), fmt.loc, fmt.stack_trace});
#endif // NDEBUG
    }

} // namespace tnrw

/**
 * @brief 带消息的 `assert`（断言）
 * @param [in] expr 断言表达式（需要可以隐式转化为 `bool` 类型或本来就是 `bool` 类型）
 * @param [in] ... 可选的断言失败时输出的消息，支持格式化
 * @details
 * - 与标准库的 `assert` 行为相同，当定义宏 `NDEBUG` 时不启用，未定义宏 `NDEBUG` 时才启用
 * - 格式化与标准库的 `std::format` 格式化相同
 * - 输出格式：
 * ```plain
 * Assertion failed at <file>:<line>:<column> (in function :<function>):
 * >> Expression: <expression>
 * >> Message: <message>
 * Stack trace:
 * frame #0 : <description> in line <line> in file <file>
 * ```
 */
#ifdef NDEBUG
#define ASSERT_MSG(expr, ...) static_cast<void>(0)
#else
#define ASSERT_MSG(expr, ...)                                                                           \
    ::tnrw::details::assert_check(expr, #expr,                                                          \
                                  std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)
#endif // NDEBUG

#endif // TNRW_BASE_ASSERT_MSG_HPP