/**
 * @file test_expressions.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::math::AlgebraicExpression` 与 `tnrw::math::NumericExpression` 类的测试用例或使用示例
 * @version 0.1.0-1
 * @date 2026-01-17
 * 
 * @copyright cpp-love
 * 
 * @details 
 * 使用方式示例：
 *  - alge help
 *  - num help
 *  - alge new 0
 *  - num operator 0 + 5
 *  - alge operator 0 += constant 3
 *  - alge operator 0 + variable x
 */

#include "math/AlgebraicExpression.hpp"
#include "math/NumericExpression.hpp"
#include "math/expressions_base.hpp"
#include <cstddef>
#include <functional>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

using Handler = std::function<void(std::span<const std::string>)>;

/**
 * @brief 将字符以空白为分隔符，分为多个子字符串
 * @param [in] line 字符串
 * @return std::vector<std::string> 子字符串列表
 */
[[nodiscard]] std::vector<std::string> splitLine(std::string_view line) {
    std::vector<std::string> out;
    size_t                   pos = 0;
    while (pos < line.size()) {
        while (pos < line.size() && (std::isspace(static_cast<unsigned char>(line[pos])) != 0)) {
            ++pos;
        }
        if (pos >= line.size()) {
            break;
        }
        size_t start = pos;
        while (pos < line.size() && (std::isspace(static_cast<unsigned char>(line[pos])) == 0)) {
            ++pos;
        }
        out.emplace_back(line.substr(start, pos - start));
    }
    return out;
}

/**
 * @brief 获取一行输入参数
 * @return std::vector<std::string> 输入参数
 */
[[nodiscard]] std::vector<std::string> getInputArgs() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return {};
    }
    return splitLine(line);
}

/// @brief 字符串类型转换的结果
enum class StrToTypeResult : std::uint8_t { Invalid, OverLimit, Normal };

/**
 * @brief 字符串转为 `long long` 类型
 * @param [in] str 字符串
 * @param [out] result 转换的类型的结果
 * @return StrToTypeResult 转换是否成功的结果
 */
[[nodiscard]] StrToTypeResult tryStrToLL(const std::string &str, long long &result) {
    try {
        size_t    pos = 0;
        long long value = std::stoll(str, &pos);
        if (pos != str.size()) {
            return StrToTypeResult::Invalid;
        }
        result = value;
        return StrToTypeResult::Normal;
    } catch (const std::invalid_argument &) {
        return StrToTypeResult::Invalid;
    } catch (const std::out_of_range &) { return StrToTypeResult::OverLimit; }
}

/**
 * @brief 字符串转为 `unsigned long long` 类型
 * @param [in] str 字符串
 * @param [out] result 转换的类型的结果
 * @return StrToTypeResult 转换是否成功的结果
 */
[[nodiscard]] StrToTypeResult tryStrToULL(const std::string &str, unsigned long long &result) {
    try {
        size_t             pos = 0;
        unsigned long long value = std::stoull(str, &pos);
        if (pos != str.size()) {
            return StrToTypeResult::Invalid;
        }
        result = value;
        return StrToTypeResult::Normal;
    } catch (const std::invalid_argument &) {
        return StrToTypeResult::Invalid;
    } catch (const std::out_of_range &) { return StrToTypeResult::OverLimit; }
}

/**
 * @brief 字符串转为 `double` 类型
 * @param [in] str 字符串
 * @param [out] result 转换的类型的结果
 * @return StrToTypeResult 转换是否成功的结果
 */
[[nodiscard]] StrToTypeResult tryStrToDouble(const std::string &str, double &result) {
    try {
        size_t pos = 0;
        double value = std::stod(str, &pos);
        if (pos != str.size()) {
            return StrToTypeResult::Invalid;
        }
        result = value;
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

    std::vector<tnrw::math::AlgebraicExpression> algevec(1); //< 代数式数组
    std::vector<tnrw::math::NumericExpression>   numvec(1);  //< 无字母的代数式数组

    std::println("想查看帮助，请键入 alge help 或 num help");
    std::println("想要退出，请键入 quit 或 exit");

    const std::vector<std::string> cmds = {"help",       "new",    "delete",
                                           "operator",   "clear",  "calculateApproximation",
                                           "toOpposite", "compare"}; //< 命令

    /**
     * @brief 生成命令帮助的函数
     * @param [in] type 命令帮助的大类型
     */
    auto                           make_type_help = [&](std::string_view type) {
        std::println("{} 可用命令：", type);
        for (const auto &cmd : cmds) { std::println("  {} {}", type, cmd); }
    };

    std::unordered_map<std::string, std::unordered_map<std::string, Handler>>
                                                              table; //< 装载对应命令处理的映射
    // 别名映射：name -> 替换的 token 列表
    std::unordered_map<std::string, std::vector<std::string>> aliases;

    // 填充 table
    table["alge"]["help"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.empty()) {
            make_type_help("alge");
            return;
        }
        if (rest_args.size() == 1 && rest_args[0] == "help") {
            make_type_help("alge");
            return;
        }
        std::println("alge help [command] 暂时支持同名命令列表");
    };

    table["alge"]["new"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: alge new [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx > algevec.size()) {
            std::println("index 越界");
            return;
        }
        algevec.emplace(algevec.begin() + static_cast<long long>(idx));
    };

    table["alge"]["delete"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: alge delete [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        algevec.erase(algevec.begin() + static_cast<long long>(idx));
    };

    table["alge"]["clear"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: alge clear [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        algevec[idx].clear();
    };

    table["alge"]["toOpposite"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: alge toOpposite [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        algevec[idx].changeToOpposite();
    };

    table["alge"]["compare"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 2) {
            std::println("用法: alge compare [i] [j]");
            return;
        }
        std::size_t first = 0;
        std::size_t second = 0;
        if (tryStrToULL(rest_args[0], first) != StrToTypeResult::Normal
            || tryStrToULL(rest_args[1], second) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (first >= algevec.size() || second >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        std::println("结果：{}", algevec[first] == algevec[second]);
    };

    table["alge"]["operator"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() < 1) {
            std::println("用法: alge operator [index] ...");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        if (rest_args.size() == 1) {
            std::println("请提供操作");
            return;
        }
        const std::string &oper = rest_args[1];
        if (oper == "front++") {
            std::println("结果：{}", ++algevec[idx]);
            return;
        }
        if (oper == "front--") {
            std::println("结果：{}", --algevec[idx]);
            return;
        }
        if (oper == "back++") {
            std::println("结果：{}", algevec[idx]++);
            return;
        }
        if (oper == "back--") {
            std::println("结果：{}", algevec[idx]--);
            return;
        }
        if (oper == "+" || oper == "-") {
            if (rest_args.size() == 2) {
                std::println("结果：{}", (oper == "+") ? +algevec[idx] : -algevec[idx]);
                return;
            }
        }
        if (rest_args.size() != 4) {
            std::println(
                "用法: alge operator [index] [+(=)|-(=)|*(=)|/(=)|...] [constant|variable] [value]");
            return;
        }
        const std::string &kind = rest_args[2];
        if (kind == "constant") {
            long long val = 0;
            if (tryStrToLL(rest_args[3], val) != StrToTypeResult::Normal) {
                std::println("常量非法");
                return;
            }
            if (oper == "+") {
                std::println("结果：{}", algevec[idx] + val);
            } else if (oper == "-") {
                std::println("结果：{}", algevec[idx] - val);
            } else if (oper == "*") {
                std::println("结果：{}", algevec[idx] * val);
            } else if (oper == "/") {
                std::println("结果：{}", algevec[idx] / val);
            } else if (oper == "+=") {
                algevec[idx] += val;
            } else if (oper == "-=") {
                algevec[idx] -= val;
            } else if (oper == "*=") {
                algevec[idx] *= val;
            } else if (oper == "/=") {
                algevec[idx] /= val;
            } else {
                std::println("不支持的操作：{}", oper);
            }
        } else if (kind == "variable") {
            const std::string_view var = rest_args[3];
            if (oper == "+") {
                std::println("结果：{}", algevec[idx] + var);
            } else if (oper == "-") {
                std::println("结果：{}", algevec[idx] - var);
            } else if (oper == "*") {
                std::println("结果：{}", algevec[idx] * var);
            } else if (oper == "/") {
                std::println("结果：{}", algevec[idx] / var);
            } else if (oper == "+=") {
                algevec[idx] += var;
            } else if (oper == "-=") {
                algevec[idx] -= var;
            } else if (oper == "*=") {
                algevec[idx] *= var;
            } else if (oper == "/=") {
                algevec[idx] /= var;
            } else {
                std::println("不支持的操作：{}", oper);
            }
        } else {
            std::println("未知的 operand 类型：{}", kind);
        }
    };

    table["alge"]["calculateApproximation"] = [&](std::span<const std::string> rest_args) {
        if ((rest_args.size() & 1u) != 1) {
            std::println("用法: alge calculateApproximation [index] [[var] [value]]...");
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        std::unordered_map<tnrw::math::VariableView, double> var_values; //< 变量对应的近似值的键值对
        for (std::size_t i = 1; i + 1 < rest_args.size(); i += 2) {
            double value = 0;
            if (tryStrToDouble(rest_args[i + 1], value) != StrToTypeResult::Normal) {
                std::println("近似值非法：{}", value);
                return;
            }
            if (!var_values.try_emplace(rest_args[i], value).second) {
                std::println("有重复的值：{}", rest_args[i]);
                return;
            }
        }
        std::println("结果：{}", algevec[idx].calculateApproximation<double>(
                                     [&](tnrw::math::VariableView var) { return var_values[var]; }));
    };

    table["num"]["help"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.empty()) {
            make_type_help("num");
            return;
        }
        if (rest_args.size() == 1 && rest_args[0] == "help") {
            make_type_help("num");
            return;
        }
        std::println("num help [command] 暂时支持同名命令列表");
    };

    table["num"]["new"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num new [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx > numvec.size()) {
            std::println("index 越界");
            return;
        }
        numvec.emplace(numvec.begin() + static_cast<long long>(idx));
    };

    table["num"]["delete"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num delete [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        numvec.erase(numvec.begin() + static_cast<long long>(idx));
    };

    table["num"]["clear"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num clear [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        numvec[idx].clear();
    };

    table["num"]["toOpposite"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num toOpposite [index]");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        numvec[idx].changeToOpposite();
    };

    table["num"]["compare"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 2) {
            std::println("用法: num compare [i] [j]");
            return;
        }
        std::size_t first = 0;
        std::size_t second = 0;
        if (tryStrToULL(rest_args[0], first) != StrToTypeResult::Normal
            || tryStrToULL(rest_args[1], second) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (first >= numvec.size() || second >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        std::println("结果：{}", numvec[first] == numvec[second]);
    };

    table["num"]["operator"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() < 1) {
            std::println("用法: num operator [index] ...");
            return;
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        if (rest_args.size() == 1) {
            std::println("请提供操作");
            return;
        }
        const std::string &oper = rest_args[1];
        if (oper == "front++") {
            std::println("结果：{}", ++numvec[idx]);
            return;
        }
        if (oper == "front--") {
            std::println("结果：{}", --numvec[idx]);
            return;
        }
        if (oper == "back++") {
            std::println("结果：{}", numvec[idx]++);
            return;
        }
        if (oper == "back--") {
            std::println("结果：{}", numvec[idx]--);
            return;
        }
        if (oper == "+" || oper == "-") {
            if (rest_args.size() == 2) {
                std::println("结果：{}", (oper == "+") ? +numvec[idx] : -numvec[idx]);
                return;
            }
        }
        if (rest_args.size() < 3) {
            std::println("用法: num operator [index] [op] [value]");
            return;
        }
        long long val = 0;
        if (tryStrToLL(rest_args[2], val) != StrToTypeResult::Normal) {
            std::println("value 非法");
            return;
        }
        if (oper == "+") {
            std::println("结果：{}", numvec[idx] + val);
        } else if (oper == "-") {
            std::println("结果：{}", numvec[idx] - val);
        } else if (oper == "*") {
            std::println("结果：{}", numvec[idx] * val);
        } else if (oper == "/") {
            std::println("结果：{}", numvec[idx] / val);
        } else if (oper == "+=") {
            numvec[idx] += val;
        } else if (oper == "-=") {
            numvec[idx] -= val;
        } else if (oper == "*=") {
            numvec[idx] *= val;
        } else if (oper == "/=") {
            numvec[idx] /= val;
        } else {
            std::println("不支持的操作：{}", oper);
        }
    };

    table["num"]["calculateApproximation"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num calculateApproximation [index]");
        }
        std::size_t idx = 0;
        if (tryStrToULL(rest_args[0], idx) != StrToTypeResult::Normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        std::println("结果：{}", numvec[idx].calculateApproximation<double>());
    };

    while (true) {
        // 输出数组
        std::println("现在 alge 数组：");
        for (std::size_t i = 0; i < algevec.size(); ++i) { std::println("{}: {}", i, algevec[i]); }
        std::println("现在 num 数组：");
        for (std::size_t i = 0; i < numvec.size(); ++i) { std::println("{}: {}", i, numvec[i]); }

        // 获取输入
        std::print(">>> ");
        (void)std::fflush(stdout);
        auto args = getInputArgs();
        if (args.empty()) {
            continue;
        }

        // 处理 alias 命令：
        if (args[0] == "alias") {
            if (args.size() < 2) {
                std::println(
                    "用法: alias set <name> <replacement...> | alias [rm|remove] <name> | alias list");
                continue;
            }
            const std::string &sub = args[1];
            if (sub == "set") {
                if (args.size() < 4) {
                    std::println("用法: alias set <name> <replacement...>");
                    continue;
                }
                const std::string &name = args[2];
                aliases[name] = {std::from_range, args | std::views::drop(3)};
                std::println("已设置别名：{}", name);
            } else if (sub == "rm" || sub == "remove") {
                if (args.size() != 3) {
                    std::println("用法: alias [rm|remove] <name>");
                    continue;
                }
                const std::string &name = args[2];
                if (aliases.erase(name) != 0u) {
                    std::println("已删除别名：{}", name);
                } else {
                    std::println("别名不存在：{}", name);
                }
            } else if (sub == "list") {
                if (aliases.empty()) {
                    std::println("当前无别名");
                    continue;
                }
                for (auto &pair : aliases) {
                    std::print("{} ->", pair.first);
                    for (auto &piece : pair.second) { std::print(" {}", piece); }
                    std::println("");
                }
            } else {
                std::println("未知 alias 子命令：{}", sub);
            }
            continue;
        }

        // 若首 token 为别名，则展开（最多展开若干层以防循环）
        const int max_expand = 8;
        int       depth = 0;
        for (auto iter = aliases.find(args[0]); depth < max_expand && iter != aliases.end();
             ++depth, iter = aliases.find(args[0])) {
            const auto &expand = iter->second;
            if (expand.empty()) {
                args.erase(args.begin());
            }
            args[0] = expand[0];
            args.insert_range(args.begin() + 1, expand | std::views::drop(1));
            if (args.empty()) {
                break;
            }
        }
        if (depth >= max_expand) {
            std::println("别名展开过深，可能存在循环，请换一种方式使用");
            continue;
        }

        if (args.size() == 1 && (args[0] == "quit" || args[0] == "exit")) {
            break;
        }
        if (args[0] == "help") {
            std::println("使用: alge ... 或 num ...，或 quit 退出 或 exit 退出");
            continue;
        }
        if (args.size() < 2) {
            std::println("命令格式: <alge|num> <command> ...");
            continue;
        }
        const std::string &type = args[0];
        const std::string &cmd = args[1];
        auto               type_it = table.find(type);
        if (type_it == table.end()) {
            std::println("未知类型：{}", type);
            continue;
        }
        auto cmd_it = type_it->second.find(cmd);
        if (cmd_it == type_it->second.end()) {
            std::println("未知命令：{} {}", type, cmd);
            continue;
        }
        cmd_it->second(std::span(&args[2], args.size() - 2));
    }

    std::println("退出中...");
    return 0;
}