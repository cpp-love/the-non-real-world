/**
 * @file test_assert_msg.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `assert_msg` 和 `tnrw::unreachable` 的测试用例和使用示例
 * @version 0.1.0-3
 * @date 2026-02-25
 * 
 * @copyright cpp-love
 * 
 */

#include "base/assert_msg.hpp"
#include <numeric>
#include <print>
#ifdef _WIN32

#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    int var = std::gcd(100, 10); //NOLINT(cppcoreguidelines-avoid-magic-numbers)

    // 1. 编译时字符串（与下面的 2. 原理相同）
    TNRW_ASSERT_MSG(var > 9, "var 必须大于9");

    // 2. 运行时字符串
    std::string message = "var 必须大于9，因为 var 为";
    message += std::format("{}", var);
    TNRW_ASSERT_MSG(var > 9, message);

    // 3. 没有字符串
    TNRW_ASSERT_MSG(var > 0);

    // 这个断言一定会被触发
    // 4. 编译时格式化字符串+格式化参数
    TNRW_ASSERT_MSG(var < 8, "这个断言一定会被触发，因为var为 {} ，大于8", var);

    // 下面不会运行到
    tnrw::unreachable("这句话不会被打印, {}", "^_^");
    tnrw::unreachable("这句话不会被打印");
    tnrw::unreachable(message);
    tnrw::unreachable();

    return 0;
}