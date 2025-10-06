/**
 * @file assert_msg.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加支持自定义输出的assert断言
 * @version 0.1.0-1
 * @date 2025-08-08
 * 
 * @copyright cpp-love
 * 
 * @details
 * - assert_msg为本文件定义的自定义输出的assert断言
 * - 使用此文件必须支持C++20
 * 
 */

#ifndef __BASE_ASSERT_MSG_HPP__
#define __BASE_ASSERT_MSG_HPP__

#include "base/string_convert.hpp"
#include <cstdlib>
#include <source_location>
#ifdef USE_FMT_OUTPUT
#include <fmt/format.h>
#else
#include <cstdio>
#include <format>
#endif

// 发布模式：完全消除
#ifdef NDEBUG
#define assert_msg(expr, ...) ((void)0)
#else
// 调试模式：使用函数实现
namespace debug_assert {
    // 断言失败处理函数
    [[noreturn]] inline void assert_fail(const char *expr, const std::source_location &loc,
                                         const char *message = nullptr) {
        // 输出基本信息
#ifdef USE_FMT_OUTPUT
        fmt::println(stderr, "Assertion failed at {} : {} : {} (in function : {}):", loc.file_name(),
                     loc.line(), loc.column(), loc.function_name());
        fmt::println(stderr, ">> Expression: {}", expr);
#else
        std::fprintf(stderr, "Assertion failed at %s : %u : %u (in function : %s):\n", loc.file_name(),
                     loc.line(), loc.column(), loc.function_name());
        std::fprintf(stderr, ">> Expression: %s\n", expr);
#endif

        // 处理自定义消息
        if (message) {
#ifdef USE_FMT_OUTPUT
            fmt::println(stderr, ">> Message: {}", message);
#else
            std::fprintf(stderr, ">> Message: %s\n", message);
#endif
        }

        // 终止程序
        std::terminate();
    }

    // 断言检查函数（核心实现）
    template <::tnrw::string_convert::Convertible FmtT, typename... Args>
    inline void assert_check(bool condition, const char *expr, const std::source_location &loc, FmtT fmt,
                             Args... args) {
        if (condition)
            return;
        assert_fail(expr, loc,
#ifdef USE_FMT_OUTPUT
                    fmt::vformat(::tnrw::string_convert::to_utf8_string_view(fmt),
                                 fmt::make_format_args(args...))
                        .c_str());
#else
                    std::vformat(::tnrw::string_convert::to_utf8_string_view(fmt),
                                 std::make_format_args(args...))
                        .c_str());
#endif
    }
    inline void assert_check(bool condition, const char *expr, const std::source_location &loc) {
        if (condition)
            return;
        assert_fail(expr, loc);
    }
} // namespace debug_assert

// 宏定义：捕获表达式和位置
#define assert_msg(expr, ...)                                                                           \
    ::debug_assert::assert_check((expr), #expr, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)

#endif // NDEBUG

#endif // __BASE_ASSERT_MSG_HPP__