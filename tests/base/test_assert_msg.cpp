/**
 * @file test_assert_msg.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `assert_msg` 的测试用例使用示例
 * @version 0.1.0-1
 * @date 2025-08-10
 * 
 * @copyright cpp-love
 * 
 */

#include "base/assert_msg.hpp"
#include "base/string_convert.hpp"
#include <iostream>
#include <numeric>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32

    int x = std::gcd(100, 10);

    // 这个断言不会触发
    assert_msg(x > 9, u8"x 必须大于9");

    // 这个断言也不会触发
    assert_msg(x > 0);

    // 这个断言一定会被触发
    assert_msg(x < 8, u8"这个断言一定会被触发，因为x为{}，大于8", x);

    // 这句话不会被打印
    std::cout << tnrw::string_convert::to_utf8_string(u8"这句话不会被打印") << std::endl;

    return 0;
}