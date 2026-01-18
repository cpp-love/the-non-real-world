/**
 * @file testAlgeExpr.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `math::AlgebraicExpression` 类的测试用例或使用示例
 * @version 0.1.0-2
 * @date 2026-01-18
 * 
 * @copyright cpp-love
 * 
 */

#include "math/AlgebraicExpression.hpp"
#include "math/expressions_base.hpp"
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

constexpr std::size_t                   cmd_size = 9; ///< 命令的个数

const std::array<std::string, cmd_size> commands{"help",       "new",     "delete",
                                                 "operator",   "clear",   "calculateApproximation",
                                                 "toOpposite", "compare", "quit"}; ///< 命令列表
const std::array<std::string, cmd_size> briefs{"提供帮助",
                                               "添加代数式",
                                               "删除代数式",
                                               "对一个代数式使用运算符",
                                               "清除一个代数式",
                                               "计算一个代数式的近似值",
                                               "对一个代数式取相反数",
                                               "比较两个代数式",
                                               "退出程序"}; ///< 命令的简要描述列表
const std::array<std::string, cmd_size> details{"\
- help\n\
  用于快速获取命令列表\n\
- help [command]\n\
  用于获取command命令的详细使用方式", "\
- new [index]\n\
  用于在第index(从0开始)个代数式前添加一个代数式\n", "\
- delete [index]\n\
  用于删除第index(从0开始)个代数式\n", "\
- operator [index] [+= | -= | *= | /= ] [constant | variable] [value]\n\
  用于对第index(从0开始)个代数式[+= | -= | *= | /=][常量 | 变量]的value\n\
- operaotor [index] [+ | - | * | /] [constant | variable] [value]\n\
  用于获取第index(从0开始)个代数式[+ | - | * | /][常量 | 变量]的value的结果\n\
- operator [index] [front++ | front--]\n\
  用于获取第index(从0开始)个代数式前置[++ | --]的结果\n\
- operator [index] [back++ | back--]\n\
  用于获取第index(从0开始)个代数式后置[++ | --]的结果\n\
- operator [index] [+ | -]\n\
  用于获取[+ | -]第index(从0开始)个代数式的结果", "\
- clear [index]\n\
  用于清空第index(从0开始)个代数式", "\
- calculateApproximation [index] [[var] [value]]...\n\
  用于将var替换为value，计算代数式的近似值", "\
- toOpposite [index]\n\
  原因将第index(从0开始)个代数式改为相反数", "\
- compare [index1] [index2]\n\
  用于比较第index1(从0开始)个代数式和第index2(从0开始)个代数式是否相等", "\
- quit\n\
  用于退出程序"};                                           ///< 命令的详细描述列表

/**
 * @brief 获取命令行参数
 * @return std::vector<std::string> 命令行参数
 */
[[nodiscard]] std::vector<std::string> getInputArgs() {
    std::vector<std::string> args;
    while (true) {
        char character = ' ';
        while (std::isspace(static_cast<unsigned char>(character)) != 0) {
            std::cin.get(character);
            if (character == '\n') {
                return args;
            }
        }
        std::cin.putback(character);
        if (std::string str; std::cin >> str, !str.empty()) {
            args.emplace_back(std::move(str));
        }
    }

    return args;
}

/**
 * @brief 处理大小不合适的参数
 * @param [in] cmd 主命令
 */
void processUnsuitedArgs(std::string_view cmd) {
    std::println("错误：命令 \"{}\" 的参数过少或过多，请键入 \"help\" 获取帮助", cmd);
}

/**
 * @brief 处理非法命令
 * @param [in] cmd 主命令
 */
void processInvalidCmds(std::string_view cmd) {
    std::println("错误：不存在命令 \"{}\"，请键入 \"help\" 获取帮助", cmd);
}

/**
 * @brief 处理非法参数
 * @param [in] cmd 主命令
 */
void processInvalidArgs(std::string_view cmd) {
    std::println("错误：命令 \"{}\" 有非法的参数，请键入 \"help\" 获取帮助", cmd);
}

/**
 * @brief 处理超限参数
 * @param [in] cmd 主命令
 */
void processOverLimitArgs(std::string_view cmd) {
    std::println("错误：命令 \"{}\" 的参数过大或过小，请键入 \"help\" 获取帮助", cmd);
}

/// @brief 作为String转为任意类型(Type)的结果
enum class StrToTypeResult : std::uint8_t { Invalid, OverLimit, Normal };

/**
 * @brief 尝试将 `std::string` 转为 `long long`
 * @param [in] str 字符串
 * @param [out] result 指向转换结果的指针，默认为 `nullptr`
 * @return StrToTypeResult 转换结果
 * @note 当 `result` 参数为 `nullptr` 时，忽略该参数，不存储转换结果
 */
[[nodiscard]] StrToTypeResult tryStrToLL(const std::string &str, long long *result = nullptr) {
    try {
        size_t    pos = 0;
        long long value = std::stoll(str, &pos);

        if (pos != str.size()) {
            return StrToTypeResult::Invalid;
        }

        if (result != nullptr) {
            *result = value;
        }
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
[[nodiscard]] StrToTypeResult tryStrToULL(const std::string &str, unsigned long long *result) {
    try {
        size_t             pos = 0;
        unsigned long long value = std::stoull(str, &pos);

        if (pos != str.size()) {
            return StrToTypeResult::Invalid;
        }

        if (result != nullptr) {
            *result = value;
        }
        return StrToTypeResult::Normal;
    } catch (const std::invalid_argument &) {
        return StrToTypeResult::Invalid;
    } catch (const std::out_of_range &) { return StrToTypeResult::OverLimit; }
}

/**
 * @brief 字符串转为 `double` 类型
 * @param [in] str 字符串
 * @param [out] result 指向转换结果的指针，默认为 `nullptr`
 * @return StrToTypeResult 转换是否成功的结果
 * @note 当 `result` 参数为 `nullptr` 时，忽略该参数，不存储转换结果
 */
[[nodiscard]] StrToTypeResult tryStrToDouble(const std::string &str, double *result = nullptr) {
    try {
        size_t pos = 0;
        double value = std::stod(str, &pos);

        if (pos != str.size()) {
            return StrToTypeResult::Invalid;
        }

        if (result != nullptr) {
            *result = value;
        }
        return StrToTypeResult::Normal;
    } catch (const std::invalid_argument &) {
        return StrToTypeResult::Invalid;
    } catch (const std::out_of_range &) { return StrToTypeResult::OverLimit; }
}

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    std::vector<tnrw::math::AlgebraicExpression> algevec(1); ///< 代数式列表
    std::println("想查看帮助，请键入 help 获取");

    while (true) {
        std::println("现在代数式数组为：");
        for (std::size_t i = 0; i < algevec.size(); ++i) { std::println("{}: {}", i, algevec[i]); }
        std::print(">>> ");
        std::fflush(stdout);
        std::vector<std::string> args = getInputArgs();
        // 未键入
        if (args.empty()) {
            continue;
        }
        if (args[0] == commands[0]) {
            if (args.size() > 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            if (args.size() == 1) {
                // 输出简要描述
                std::println("如要详细查看某条命令的使用方式，请键入 help [command] 获取");
                for (std::size_t i = 0; i < cmd_size; ++i) {
                    std::println("{}  {}", commands[i], briefs[i]);
                }
                continue;
            }
            // 输出详细描述
            std::size_t i = 0;
            for (; i < cmd_size; ++i) {
                if (args[1] == commands[i]) {
                    std::println("{}", details[i]);
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
            // 新建代数式
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res > algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        algevec.emplace(algevec.begin() + static_cast<long long>(*res));
                    }
                    break;
            }
        } else if (args[0] == commands[2]) {
            if (args.size() != 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 删除代数式
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res >= algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        algevec.erase(algevec.begin() + static_cast<long long>(*res));
                    }
                    break;
            }
        } else if (args[0] == commands[3]) {
            if (args.size() != 3 && args.size() != 5) {
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
                continue;
            }
            if (args[2] == "front++") {
                if (args.size() == 3) {
                    std::println("结果：{}", ++algevec[index]);
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else if (args[2] == "front--") {
                if (args.size() == 3) {
                    std::println("结果：{}", --algevec[index]);
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else if (args[2] == "back++") {
                if (args.size() == 3) {
                    std::println("结果：{}", algevec[index]++);
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else if (args[2] == "back--") {
                if (args.size() == 3) {
                    std::println("结果：{}", algevec[index]--);
                } else {
                    processUnsuitedArgs(args[0]);
                }
            } else {
                if (args[2] == "+") {
                    if (args.size() == 3) {
                        std::println("结果：{}", +algevec[index]);
                        continue;
                    }
                } else if (args[2] == "-") {
                    if (args.size() == 3) {
                        std::println("结果：{}", -algevec[index]);
                        continue;
                    }
                }
                if (args.size() == 3) {
                    processInvalidArgs(args[0]);
                    continue;
                }
                if (args[3] == "constant") {
                    std::unique_ptr<long long> res2 = std::make_unique<long long>(0);
                    const StrToTypeResult      result2 = tryStrToLL(args[4], res2.get());
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
                        std::println("结果：{}", algevec[index] + val);
                    } else if (args[2] == "-") {
                        std::println("结果：{}", algevec[index] - val);
                    } else if (args[2] == "*") {
                        std::println("结果：{}", algevec[index] * val);
                    } else if (args[2] == "/") {
                        std::println("结果：{}", algevec[index] / val);
                    } else if (args[2] == "+=") {
                        algevec[index] += val;
                    } else if (args[2] == "-=") {
                        algevec[index] -= val;
                    } else if (args[2] == "*=") {
                        algevec[index] *= val;
                    } else if (args[2] == "/=") {
                        algevec[index] /= val;
                    } else {
                        processInvalidArgs(args[0]);
                    }
                } else if (args[3] == "variable") {
                    const std::string_view val = args[4];
                    if (args[2] == "+") {
                        std::println("结果：{}", algevec[index] + val);
                    } else if (args[2] == "-") {
                        std::println("结果：{}", algevec[index] - val);
                    } else if (args[2] == "*") {
                        std::println("结果：{}", algevec[index] * val);
                    } else if (args[2] == "/") {
                        std::println("结果：{}", algevec[index] / val);
                    } else if (args[2] == "+=") {
                        algevec[index] += val;
                    } else if (args[2] == "-=") {
                        algevec[index] -= val;
                    } else if (args[2] == "*=") {
                        algevec[index] *= val;
                    } else if (args[2] == "/=") {
                        algevec[index] /= val;
                    } else {
                        processInvalidArgs(args[0]);
                    }
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
            // 清空代数式
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
            // 计算一个代数式的近似值
            if ((args.size() & 1u) != 0) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            std::unique_ptr<std::size_t> res = std::make_unique<std::size_t>(0);
            switch (tryStrToULL(args[1], res.get())) {
                case StrToTypeResult::Invalid: processInvalidArgs(args[0]); break;
                case StrToTypeResult::OverLimit: processOverLimitArgs(args[0]); break;
                case StrToTypeResult::Normal:
                    if (*res >= algevec.size()) {
                        processOverLimitArgs(args[0]);
                    } else {
                        std::unordered_map<tnrw::math::VariableView, double>
                             var_values; //< 变量对应的近似值的键值对
                        bool not_finished = true;
                        for (std::size_t i = 2; i + 1 < args.size(); i += 2) {
                            std::unique_ptr<double> val_p = std::make_unique<double>(0);
                            auto                    result = tryStrToDouble(args[i + 1], val_p.get());
                            if (result == StrToTypeResult::Invalid) {
                                processInvalidArgs(args[0]);
                                not_finished = false;
                                break;
                            }
                            if (result == StrToTypeResult::OverLimit) {
                                processOverLimitArgs(args[0]);
                                not_finished = false;
                                break;
                            }
                            if (!var_values.try_emplace(args[i], *val_p).second) {
                                std::println("错误：有重复的值：{}", args[i]);
                                not_finished = false;
                                break;
                            }
                        }
                        if (not_finished) {
                            std::println("结果：{}", algevec[*res].calculateApproximation<double>(
                                                         [&](tnrw::math::VariableView var) {
                                                             return var_values[var];
                                                         }));
                        }
                    }
                    break;
            }
        } else if (args[0] == commands[6]) {
            if (args.size() != 2) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 使代数式取相反数
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
        } else if (args[0] == commands[7]) {
            if (args.size() != 3) {
                // 非法
                processUnsuitedArgs(args[0]);
                continue;
            }
            // 比较两个代数式
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
                        std::println("结果：{:b}", algevec[*res1] == algevec[*res2]);
                    }
                    break;
            }
        } else if (args[0] == commands[8]) {
            break;
        } else {
            processInvalidCmds(args[0]);
        }
    }

    std::println("退出中...");

    return 0;
}