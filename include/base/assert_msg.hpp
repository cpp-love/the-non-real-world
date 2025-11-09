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

#ifndef __TNRW_BASE_ASSERT_MSG_HPP__
#define __TNRW_BASE_ASSERT_MSG_HPP__

#include <format>
#include <source_location>
#include <spdlog/spdlog.h>

#ifdef NDEBUG
#define assert_msg(expr, ...) ((void)0)
#else

/// @brief 一些内部实现，用户不应访问
/// @cond INTERNAL
namespace tnrw::details {
    /**
     * @brief 断言失败处理函数
     * @param [in] expr 断言表达式
     * @param [in] loc 断言位置
     * @param [in] message 断言失败时输出的消息（如果没有，为 `nullptr)
     */
    [[noreturn]] constexpr void assert_fail(std::string_view expr, const std::source_location &loc,
                                            const char *message = nullptr) {
        // 输出信息
        if (message != nullptr) {
            spdlog::error("Assertion failed at {} : {} : {} (in function : {}):\n>> Expression: "
                          "{}\n>> Message: {}",
                          loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr, message);
        } else {
            spdlog::error("Assertion failed at {} : {} : {} (in function : {}):\n>> Expression: {}",
                          loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr);
        }

        // 终止程序
        std::terminate();
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
                                std::format_string<Args...> fmt, Args... args) {
        if (condition) {
            return;
        }
        assert_fail(expr, loc, std::format(fmt, std::forward<Args>(args)...).c_str());
    }
    constexpr void assert_check(bool condition, const char *expr, const std::source_location &loc) {
        if (condition) {
            return;
        }
        assert_fail(expr, loc);
    }
} // namespace tnrw::details
/// @endcond

/**
 * @brief 带消息的assert断言
 * @param [in] expr 断言表达式（需要可以转化为 `bool` 类型）
 * @param [in] other_params(...) 可选的断言失败时输出的消息，支持格式化
 * @note 格式化与标准库的 `std::format` 格式化相同，且支持简易的字符串转换（参见 @ref tnrw::string_convert::to_utf8_string_view 重载）
 */
#define assert_msg(expr, ...)                                                                           \
    ::tnrw::details::assert_check(static_cast<bool>(expr), #expr,                                       \
                                  std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)

#endif // NDEBUG

#endif // __TNRW_BASE_ASSERT_MSG_HPP__