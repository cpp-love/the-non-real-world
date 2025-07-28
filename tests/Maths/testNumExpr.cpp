/**
 * @file testNumExpr.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `Maths::NumericExpression` 类的测试用例或使用示例
 * @version 0.1.0-1
 * @date 2025-07-26
 * 
 * @copyright cpp-love
 * 
 */

#include "Maths/Maths_base.hpp"
#include "Maths/NumericExpression.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

constexpr std::size_t cmd_size = 10; ///< 命令的个数

const std::array<std::string, cmd_size> commands{
    "help",     "new",    "delete",     "operator", "clear",
    "getValue", "isZero", "toOpposite", "compare",  "quit"}; ///< 命令列表
const std::array<std::string, cmd_size> briefs{u8"提供帮助",
                                               u8"添加无字母的代数式",
                                               u8"删除无字母的代数式",
                                               u8"对一个无字母的代数式使用运算符",
                                               u8"清除一个无字母的代数式",
                                               u8"获取一个无字母的代数式的值(暂未加入)",
                                               u8"判断一个无字母的代数式是否为0",
                                               u8"对一个无字母的代数式取相反数",
                                               u8"比较两个无字母的代数式",
                                               u8"退出程序"}; ///< 命令的简要描述列表
const std::array<std::string, cmd_size> details{u8"\
- help\n\
  用于快速获取命令列表\n\
- help [command]\n\
  用于获取command命令的详细使用方式", u8"\
- new [index]\n\
  用于在第index(从0开始)个无字母的代数式前添加一个无字母的代数式\n", u8"\
- delete [index]\n\
  用于删除第index(从0开始)个无字母的代数式\n", u8"\
- operator [index] [+= | -= | *= | /= ] [value]\n\
  用于对第index(从0开始)个无字母的代数式[+= | -= | *= | /=]常量的value\n\
- operaotor [index] [+ | - | * | /] [value]\n\
  用于获取第index(从0开始)个无字母的代数式[+ | - | * | /]常量的value的结果\n\
- operator [index] [front++ | front--]\n\
  用于获取第index(从0开始)个无字母的代数式前置[++ | --]的结果\n\
- operator [index] [back++ | back--]\n\
  用于获取第index(从0开始)个无字母的代数式后置[++ | --]的结果\n\
- operator [index] [+ | -]\n\
  用于获取[+ | -]第index(从0开始)个无字母的代数式的结果", u8"\
- clear [index]\n\
  用于清空第index(从0开始)个无字母的代数式",
                                                u8"\
- getValue [index] ??\n\
  本节未完成\n\
  原因：未定义tnrw::Maths::NumericExpression的getValue方法", u8"\
- isZero [index] ??\n\
  本节未完成\n\
  原因：未定义tnrw::Maths::NumericExpression的isZero方法", u8"\
- toOpposite [index]\n\
  原因将第index(从0开始)个无字母的代数式改为相反数", u8"\
- compare [index1] [index2]\n\
  用于比较第index1(从0开始)个无字母的代数式和第index2(从0开始)个无字母的代数式是否相等", u8"\
- quit\n\
  用于退出程序"}; ///< 命令的详细描述列表

std::vector<tnrw::Maths::NumericExpression> algevec(1); ///< 无字母的代数式列表

/**
 * @brief 处理大小不合适的参数
 * @param [in] cmd 主命令
 */
void processUnsuitedArgs(std::string cmd) {
    std::cout << u8"错误：命令 \"" << cmd
              << u8"\" 的参数过少或过多，请键入 \"help\" 获取帮助\n";
}

/**
 * @brief 处理非法命令
 * @param [in] cmd 主命令
 */
void processInvalidCmds(std::string cmd) {
    std::cout << u8"错误：不存在命令 \"" << cmd << u8"\"，请键入 \"help\" 获取帮助\n";
}

/**
 * @brief 处理非法参数
 * @param [in] cmd 主命令
 */
void processInvalidArgs(std::string cmd) {
    std::cout << u8"错误：命令 \"" << cmd
              << u8"\" 有非法的参数，请键入 \"help\" 获取帮助\n";
}

/**
 * @brief 处理超限参数
 * @param [in] cmd 主命令
 */
void processOverLimitArgs(std::string cmd) {
    std::cout << u8"错误：命令 \"" << cmd
              << u8"\" 的参数过大或过小，请键入 \"help\" 获取帮助\n";
}

/**
 * @brief 获取命令行参数
 * @return std::vector<std::string> 命令行参数
 */
std::vector<std::string> getArgs() {
    std::vector<std::string> args;
    bool                     not_finished = true;
    args.reserve(4); //< 提升性能
    for (int i = 0; not_finished; ++i) {
        char ch;
        // 忽略前导空格
        while ((ch = getchar()) == ' ');
        if (ch == '\n') {
            break;
        } else {
            args.emplace_back();
            args[i].reserve(8); //< 提升性能
            args[i].push_back(ch);
        }
        while (true) {
            ch = std::getchar();
            if (ch == '\n') {
                not_finished = false;
                break;
            } else if (ch == ' ') {
                break;
            }
            args[i].push_back(ch);
        }
    }

    return args;
}

/// @brief 作为String转为任意类型(Type)的结果
enum class StrToTypeResult { Invalid, OverLimit, Normal };

/**
 * @brief 尝试将 `std::string` 转为 `long long`
 * @param [in] str 字符串
 * @param [out] result 指向转换结果的指针，默认为 `nullptr`
 * @return StrToTypeResult 转换结果
 * @note 当 `result` 参数为 `nullptr` 时，忽略该参数，不存储转换结果
 */
StrToTypeResult tryStrToLL(const std::string &str, long long *result = nullptr) {
    try {
        size_t    pos;
        long long value = std::stoll(str, &pos);

        if (pos != str.size())
            return StrToTypeResult::Invalid;

        if (result)
            *result = value;
        return StrToTypeResult::Normal;
    } catch (const std::invalid_argument &) {
        return StrToTypeResult::Invalid;
    } catch (const std::out_of_range &) { return StrToTypeResult::OverLimit; }
}

/**
 * @brief 尝试将 `std::string` 转为 `unsigned long long`
 * @param [in] str 字符串
 * @param [out] result 指向转换结果的指针，默认为 `nullptr`
 * @return StrToTypeResult 转换结果
 * @note 当 `result` 参数为 `nullptr` 时，忽略该参数，不存储转换结果
 */
StrToTypeResult tryStrToULL(const std::string &str, unsigned long long *result) {
    try {
        size_t             pos;
        unsigned long long value = std::stoull(str, &pos);

        if (pos != str.size())
            return StrToTypeResult::Invalid;

        if (result)
            *result = value;
        return StrToTypeResult::Normal;
    } catch (const std::invalid_argument &) {
        return StrToTypeResult::Invalid;
    } catch (const std::out_of_range &) { return StrToTypeResult::OverLimit; }
}

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32

    std::cout << u8"想查看帮助，请键入 help 获取\n";

    while (true) {
        std::cout << u8"现在无字母的代数式数组为：\n";
        for (std::size_t i = 0; i < algevec.size(); ++i) {
            std::cout << i << ": " << algevec[i].toString() << '\n';
        }
        std::cout << u8">>> " << std::flush;
        std::vector<std::string> args = getArgs();
        // 未键入
        if (args.empty())
            continue;
        if (args[0] == commands[0]) {
            if (args.size() > 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            if (args.size() == 1) {
                // 输出简要描述
                std::cout
                    << u8"如要详细查看某条命令的使用方式，请键入 help [command] 获取\n";
                for (std::size_t i = 0; i < cmd_size; ++i) {
                    std::cout << commands[i] << ' ' << briefs[i] << '\n';
                }
                continue;
            }
            // 输出详细描述
            std::size_t i = 0;
            for (; i < cmd_size; ++i) {
                if (args[1] == commands[i]) {
                    std::cout << details[i] << '\n';
                    break;
                }
            }
            if (i == cmd_size) {
                processInvalidArgs(args[0]);
            }
        } else if (args[0] == commands[1]) {
            if (args.size() != 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 新建无字母的代数式
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res > algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        algevec.emplace(algevec.begin() + *res);
                    }
                    break;
            }
        } else if (args[0] == commands[2]) {
            if (args.size() != 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 删除无字母的代数式
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res >= algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        algevec.erase(algevec.begin() + *res);
                    }
                    break;
            }
        } else if (args[0] == commands[3]) {
            if (args.size() != 3 && args.size() != 4) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 获取下标
            std::unique_ptr<std::size_t> res1 = std::make_unique<std::size_t>(0);
            const StrToTypeResult        result1 = tryStrToULL(args[1], res1.get());
            if (result1 == StrToTypeResult::Invalid) {
                processInvalidArgs(args[0]);
                continue;
            }
            if (result1 == StrToTypeResult::OverLimit) {
                processOverLimitArgs(args[0]);
                continue;
            }
            const std::size_t index = *res1;
            if (index >= algevec.size()) {
                processOverLimitArgs(args[0]);
            }
            if (args[2] == "front++") {
                if (args.size() == 3) {
                    std::cout << u8"结果：" << (++algevec[index]).toString() << '\n';
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else if (args[2] == "front--") {
                if (args.size() == 3) {
                    std::cout << u8"结果：" << (--algevec[index]).toString() << '\n';
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else if (args[2] == "back++") {
                if (args.size() == 3) {
                    std::cout << u8"结果：" << (algevec[index]++).toString() << '\n';
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else if (args[2] == "back--") {
                if (args.size() == 3) {
                    std::cout << u8"结果：" << (algevec[index]--).toString() << '\n';
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else {
                if (args[2] == "+") {
                    if (args.size() == 3) {
                        std::cout << u8"结果：" << (+algevec[index]).toString() << '\n';
                    }
                } else if (args[2] == "-") {
                    if (args.size() == 3) {
                        std::cout << u8"结果：" << (-algevec[index]).toString() << '\n';
                    }
                }
                if (args.size() == 3) {
                    processInvalidArgs(args[0]);
                    continue;
                }
                std::unique_ptr<long long> res2 = std::make_unique<long long>(0);
                const StrToTypeResult      result2 = tryStrToLL(args[3], res2.get());
                if (result2 == StrToTypeResult::Invalid) {
                    processInvalidArgs(args[0]);
                    continue;
                }
                if (result2 == StrToTypeResult::OverLimit) {
                    processOverLimitArgs(args[0]);
                    continue;
                }
                const long long val = *res2;
                if (args[2] == "+") {
                    std::cout << u8"结果："
                              << (algevec[index]
                                  + static_cast<tnrw::Maths::ConstantType>(val))
                                     .toString()
                              << '\n';
                } else if (args[2] == "-") {
                    std::cout << u8"结果："
                              << (algevec[index]
                                  - static_cast<tnrw::Maths::ConstantType>(val))
                                     .toString()
                              << '\n';
                } else if (args[2] == "*") {
                    std::cout << u8"结果："
                              << (algevec[index]
                                  * static_cast<tnrw::Maths::ConstantType>(val))
                                     .toString()
                              << '\n';
                } else if (args[2] == "/") {
                    std::cout << u8"结果："
                              << (algevec[index]
                                  / static_cast<tnrw::Maths::ConstantType>(val))
                                     .toString()
                              << '\n';
                } else if (args[2] == "+=") {
                    algevec[index] += static_cast<tnrw::Maths::ConstantType>(val);
                } else if (args[2] == "-=") {
                    algevec[index] -= static_cast<tnrw::Maths::ConstantType>(val);
                } else if (args[2] == "*=") {
                    algevec[index] *= static_cast<tnrw::Maths::ConstantType>(val);
                } else if (args[2] == "/=") {
                    algevec[index] /= static_cast<tnrw::Maths::ConstantType>(val);
                } else {
                    processInvalidArgs(args[0]);
                }
            }
        } else if (args[0] == commands[4]) {
            if (args.size() != 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 清空无字母的代数式
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res >= algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        algevec[*res].clear();
                    }
                    break;
            }
        } else if (args[0] == commands[5]) {
            std::cout << u8"本节未完成\n原因：未定义tnrw::Maths::"
                         "NumericExpression的getValue方法\n";
        } else if (args[0] == commands[6]) {
            std::cout << u8"本节未完成\n原因：未定义tnrw::Maths::"
                         "NumericExpression的isZero方法\n";
        } else if (args[0] == commands[7]) {
            if (args.size() != 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 使无字母的代数式取相反数
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res >= algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        algevec[*res].changeToOpposite();
                    }
                    break;
            }
        } else if (args[0] == commands[8]) {
            if (args.size() != 3) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 比较两个无字母的代数式
            std::unique_ptr<std::size_t> res1 = std::make_unique<std::size_t>(0);
            const StrToTypeResult        result = tryStrToULL(args[1], res1.get());
            if (result == StrToTypeResult::Invalid) {
                processInvalidArgs(args[0]);
                continue;
            }
            if (result == StrToTypeResult::OverLimit) {
                processOverLimitArgs(args[0]);
                continue;
            }
            if (*res1 >= algevec.size()) {
                processOverLimitArgs(args[0]);
                continue;
            }
            std::unique_ptr<std::size_t> res2 = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[2], res2.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res2 >= algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        std::cout << u8"结果：" << std::boolalpha
                                  << (algevec[*res1] == algevec[*res2]) << '\n';
                    }
                    break;
            }
        } else if (args[0] == commands[9]) {
            break;
        } else {
            processInvalidCmds(args[0]);
        }
    }

    std::cout << u8"退出中..." << std::endl;

    return 0;
}