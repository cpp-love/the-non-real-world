/**
 * @file test_string_convert.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `string_convert` 命名空间的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-08-10
 * 
 * @copyright cpp-love
 * 
 */

#include "base/string_convert.hpp"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    std::cout << tnrw::string_convert::toUtf8String(u8"这是一个类型为 const char8_t * 类型的字符串")
              << ' '
              << tnrw::string_convert::toUtf8String(
                     std::u8string(u8"这是一个类型为 std::u8string 类型的字符串"))
              << '\n'
              << std::flush;

    return 0;
}