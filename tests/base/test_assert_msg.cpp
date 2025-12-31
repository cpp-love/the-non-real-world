/**
 * @file test_assert_msg.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `assert_msg` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-08-10
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

    // 这个断言不会触发
    assert_msg(var > 9, "var 必须大于9");

    // 这个断言也不会触发
    assert_msg(var > 0);

    // 这个断言一定会被触发
    assert_msg(var < 8, "这个断言一定会被触发，因为var为 {} ，大于8", var);

    // 这句话不会被打印
    std::println("这句话不会被打印");

    return 0;
}