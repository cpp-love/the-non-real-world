/**
 * @file test_expressions.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::math::algebraic_expression` 与 `tnrw::math::numeric_expression` 类的测试用例或使用示例
 * @version 0.1.0-3
 * @date 2026-02-14
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
 *  - alge operator 0 += algeexpr 1
 */

#include "math/algebraic_expression.hpp"
#include "math/expressions_base.hpp"
#include "math/numeric_expression.hpp"
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

using handler = std::function<void(std::span<const std::string>)>;

/**
 * @brief 将字符以空白为分隔符，分为多个子字符串
 * @param [in] line 字符串
 * @return std::vector<std::string> 子字符串列表
 */
[[nodiscard]] std::vector<std::string> split_line(std::string_view line) {
    std::vector<std::string> out;
    std::size_t              pos = 0;
    while (pos < line.size()) {
        while (pos < line.size() && (std::isspace(static_cast<unsigned char>(line[pos])) != 0)) {
            ++pos;
        }
        if (pos >= line.size()) {
            break;
        }
        std::size_t start = pos;
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
[[nodiscard]] std::vector<std::string> get_input_args() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return {};
    }
    return split_line(line);
}

/// @brief 字符串类型转换的结果
enum class str_to_type_result : std::uint8_t { invalid, over_limit, normal };

/**
 * @brief 字符串转为 `long long` 类型
 * @param [in] str 字符串
 * @param [out] result 转换的类型的结果
 * @return str_to_type_result 转换是否成功的结果
 */
[[nodiscard]] str_to_type_result try_str_to_ll(const std::string &str, long long &result) {
    try {
        std::size_t pos = 0;
        long long   value = std::stoll(str, &pos);
        if (pos != str.size()) {
            return str_to_type_result::invalid;
        }
        result = value;
        return str_to_type_result::normal;
    } catch (const std::invalid_argument &) {
        return str_to_type_result::invalid;
    } catch (const std::out_of_range &) { return str_to_type_result::over_limit; }
}

/**
 * @brief 字符串转为 `unsigned long long` 类型
 * @param [in] str 字符串
 * @param [out] result 转换的类型的结果
 * @return str_to_type_result 转换是否成功的结果
 */
[[nodiscard]] str_to_type_result try_str_to_ull(const std::string &str, unsigned long long &result) {
    try {
        std::size_t        pos = 0;
        unsigned long long value = std::stoull(str, &pos);
        if (pos != str.size()) {
            return str_to_type_result::invalid;
        }
        result = value;
        return str_to_type_result::normal;
    } catch (const std::invalid_argument &) {
        return str_to_type_result::invalid;
    } catch (const std::out_of_range &) { return str_to_type_result::over_limit; }
}

/**
 * @brief 字符串转为 `double` 类型
 * @param [in] str 字符串
 * @param [out] result 转换的类型的结果
 * @return str_to_type_result 转换是否成功的结果
 */
[[nodiscard]] str_to_type_result try_str_to_double(const std::string &str, double &result) {
    try {
        std::size_t pos = 0;
        double      value = std::stod(str, &pos);
        if (pos != str.size()) {
            return str_to_type_result::invalid;
        }
        result = value;
        return str_to_type_result::normal;
    } catch (const std::invalid_argument &) {
        return str_to_type_result::invalid;
    } catch (const std::out_of_range &) { return str_to_type_result::over_limit; }
}

int main(int argc, char *argv[]) {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    if (argc > 2) {
        std::println("error: 过多的命令行参数");
        return 0;
    }
    bool print_verbose = true;
    if (argc == 2) {
        std::span<char *, 2> args(argv, argc);
        if (std::string_view(args[1]) != std::string_view("--simplify-output")) {
            std::println("error: 非法的命令行参数 {}", args[1]);
            return 0;
        }
        print_verbose = false;
    }

    std::vector<tnrw::math::algebraic_expression> algevec(1); //< 代数式数组
    std::vector<tnrw::math::numeric_expression>   numvec(1);  //< 无字母的代数式数组

    if (print_verbose) {
        std::println("想查看帮助，请键入 help 或 alge help 或 num help");
        std::println("想要退出，请键入 quit 或 exit");
    }

    constexpr std::array<std::string_view, 10> alge_cmds = {"help",
                                                            "new",
                                                            "delete",
                                                            "operator",
                                                            "clear",
                                                            "calculate_approximation",
                                                            "change_to_opposite",
                                                            "compare",
                                                            "has_variable",
                                                            "toNum"};
    constexpr std::array<std::string_view, 10> alge_briefs{"提供帮助",
                                                           "添加代数式",
                                                           "删除代数式",
                                                           "对一个代数式使用运算符",
                                                           "清除一个代数式",
                                                           "计算一个代数式的近似值",
                                                           "对一个代数式取相反数",
                                                           "比较两个代数式",
                                                           "判断一个代数式是否有指定变量",
                                                           "将一个代数式转换成一个无字母的代数式"};
    constexpr std::array<std::string_view, 10> alge_details{
        "- help\n"
        "  用于快速获取命令列表\n"
        "- help [command]\n"
        "  用于获取command命令的详细使用方式",
        "- new [index]\n"
        "  用于在第index(从0开始)个代数式前添加一个代数式",
        "- delete [index]\n"
        "  用于删除第index(从0开始)个代数式",
        "- operator [index] [+= | -= | *= | /= ] [constant | variable] [value]\n"
        "  用于对第index(从0开始)个代数式[+= | -= | *= | /=][常量 | 变量]value\n"
        "- operator [index1] [+= | -= | *= | /= ] algeexpr [index2]\n"
        "  用于对第index1(从0开始)个代数式[+= | -= | *= | /=]第index2(从0开始)个代数式\n"
        "- operaotor [index] [+ | - | * | /] [constant | variable] [value]\n"
        "  用于获取第index(从0开始)个代数式[+ | - | * | /][常量 | 变量]value的结果\n"
        "- operator [index1] [+ | - | * | / ] algeexpr [index2]\n"
        "  用于获取第index1(从0开始)个代数式[+ | - | * | /]第index2(从0开始)个代数式的结果\n"
        "- operator [index] [front++ | front--]\n"
        "  用于获取第index(从0开始)个代数式前置[++ | --]的结果\n"
        "- operator [index] [back++ | back--]\n"
        "  用于获取第index(从0开始)个代数式后置[++ | --]的结果\n"
        "- operator [index] [+ | -]\n"
        "  用于获取[+ | -]第index(从0开始)个代数式的结果",
        "- clear [index]\n"
        "  用于清空第index(从0开始)个代数式",
        "- calculate_approximation [index] [[var] [value]]...\n"
        "  用于将var替换为value，计算代数式的近似值",
        "- change_to_opposite [index]\n"
        "  用于将第index(从0开始)个代数式改为相反数",
        "- compare [index1] [index2]\n"
        "  用于比较第index1(从0开始)个代数式和第index2(从0开始)个代数式是否相等",
        "- has_variable [index]\n"
        "  用于判断第index(从0开始)个代数式是否有变量\n"
        "- has_variable [index] [var]\n"
        "  用于判断第index(从0开始)个代数式是否有指定变量var",
        "- toNum [index1] [index2]\n"
        "  用于尝试删除第index1(从0开始)个代数式，将其转换成无字母的代数式，"
        "并在第index2(从0开始)个无字母的代数式前添加；"
        "如果失败，输出错误信息"};

    constexpr std::array<std::string_view, 9> num_cmds = {"help",
                                                          "new",
                                                          "delete",
                                                          "operator",
                                                          "clear",
                                                          "calculate_approximation",
                                                          "change_to_opposite",
                                                          "compare",
                                                          "toAlge"};
    constexpr std::array<std::string_view, 9> num_briefs{"提供帮助",
                                                         "添加无字母的代数式",
                                                         "删除无字母的代数式",
                                                         "对一个无字母的代数式使用运算符",
                                                         "清除一个无字母的代数式",
                                                         "计算一个无字母的代数式的近似值",
                                                         "对一个无字母的代数式取相反数",
                                                         "比较两个无字母的代数式",
                                                         "将一个无字母的代数式转换成一个代数式"};
    constexpr std::array<std::string_view, 9> num_details{
        "- help\n"
        "  用于快速获取命令列表\n"
        "- help [command]\n"
        "  用于获取command命令的详细使用方式",
        "- new [index]\n"
        "  用于在第index(从0开始)个无字母的代数式前添加一个无字母的代数式",
        "- delete [index]\n"
        "  用于删除第index(从0开始)个无字母的代数式",
        "- operator [index] [+= | -= | *= | /= ] [value]\n"
        "  用于对第index(从0开始)个无字母的代数式[+= | -= | *= | /=]常量的value\n"
        "- operator [index1] [+= | -= | *= | /= ] numexpr [index2]\n"
        "  用于对第index1(从0开始)个无字母的代数式[+= | -= | *= | /=]第index2(从0开始)个无字母的代数式\n"
        "- operator [index] [+ | - | * | /] [value]\n"
        "  用于获取第index(从0开始)个无字母的代数式[+ | - | * | /]常量的value的结果\n"
        "- operator [index1] [+ | - | * | / ] numexpr [index2]\n"
        "  用于获取第index1(从0开始)个无字母的代数式[+ | - | * | "
        "/]第index2(从0开始)个无字母的代数式的结果\n"
        "- operator [index] [front++ | front--]\n"
        "  用于获取第index(从0开始)个无字母的代数式前置[++ | --]的结果\n"
        "- operator [index] [back++ | back--]\n"
        "  用于获取第index(从0开始)个无字母的代数式后置[++ | --]的结果\n"
        "- operator [index] [+ | -]\n"
        "  用于获取[+ | -]第index(从0开始)个无字母的代数式的结果",
        "- clear [index]\n"
        "  用于清空第index(从0开始)个无字母的代数式",
        "- calculate_approximation [index]\n"
        "  用于计算无字母的代数式的近似值",
        "- change_to_opposite [index]\n"
        "  用于将第index(从0开始)个无字母的代数式改为相反数",
        "- compare [index1] [index2]\n"
        "  用于比较第index1(从0开始)个无字母的代数式和第index2(从0开始)个无字母的代数式是否相等",
        "- toAlge [index1] [index2]\n"
        "  用于删除第index1(从0开始)个无字母的代数式，将其转换成代数式，"
        "并在第index2(从0开始)个代数式前添加"};

    std::unordered_map<std::string, std::unordered_map<std::string, handler>>
                                                              table; //< 装载对应命令处理的映射
    // 别名映射：name -> 替换的 token 列表
    std::unordered_map<std::string, std::vector<std::string>> aliases;

    // 填充 table
    table["alge"]["help"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.empty() || (rest_args.size() == 1 && rest_args[0] == "help")) {
            std::println("如要详细查看某条命令的使用方式，请键入 alge help [command] 获取");
            std::println("alge 子命令列表:");
            for (const auto &[cmd, brief] : std::views::zip(alge_cmds, alge_briefs)) {
                std::println("  {}  {}", cmd, brief);
            }
            return;
        }
        if (rest_args.size() == 1) {
            // 输出某条命令的详细描述
            for (const auto &[cmd, detail] : std::views::zip(alge_cmds, alge_details)) {
                if (rest_args[0] == cmd) {
                    std::println("{}", detail);
                    return;
                }
            }
            std::println("未知命令：{}", rest_args[0]);
            return;
        }
        std::println("用法: alge help [command]");
    };

    table["alge"]["new"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: alge new [index]");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
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
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
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
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        algevec[idx].clear();
    };

    table["alge"]["change_to_opposite"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: alge change_to_opposite [index]");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        algevec[idx].change_to_opposite();
    };

    table["alge"]["compare"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 2) {
            std::println("用法: alge compare [i] [j]");
            return;
        }
        std::size_t first = 0;
        std::size_t second = 0;
        if (try_str_to_ull(rest_args[0], first) != str_to_type_result::normal
            || try_str_to_ull(rest_args[1], second) != str_to_type_result::normal) {
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
        if (rest_args.empty()) {
            std::println("用法: alge operator [index] ...");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
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
            std::println("用法: alge operator [index] [+(=)|-(=)|*(=)|/(=)|...] "
                         "[constant|variable|algeexpr] [value]");
            return;
        }
        const std::string &kind = rest_args[2];
        if (kind == "constant") {
            long long val = 0;
            if (try_str_to_ll(rest_args[3], val) != str_to_type_result::normal) {
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
        } else if (kind == "algeexpr") {
            std::size_t idx2 = 0;
            if (try_str_to_ull(rest_args[3], idx2) != str_to_type_result::normal) {
                std::println("index 非法");
                return;
            }
            if (idx2 >= algevec.size()) {
                std::println("index 越界");
                return;
            }
            if (oper == "+") {
                std::println("结果：{}", algevec[idx] + algevec[idx2]);
            } else if (oper == "-") {
                std::println("结果：{}", algevec[idx] - algevec[idx2]);
            } else if (oper == "*") {
                std::println("结果：{}", algevec[idx] * algevec[idx2]);
            } else if (oper == "/") {
                std::println("结果：{}", algevec[idx] / algevec[idx2]);
            } else if (oper == "+=") {
                algevec[idx] += algevec[idx2];
            } else if (oper == "-=") {
                algevec[idx] -= algevec[idx2];
            } else if (oper == "*=") {
                algevec[idx] *= algevec[idx2];
            } else if (oper == "/=") {
                algevec[idx] /= algevec[idx2];
            } else {
                std::println("不支持的操作：{}", oper);
            }
        } else {
            std::println("未知的 operand 类型：{}", kind);
        }
    };

    table["alge"]["calculate_approximation"] = [&](std::span<const std::string> rest_args) {
        if ((rest_args.size() & 1u) != 1) {
            std::println("用法: alge calculate_approximation [index] [[var] [value]]...");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        std::unordered_map<tnrw::math::variable_view, double> var_values; //< 变量对应的近似值的键值对
        for (std::size_t i = 1; i + 1 < rest_args.size(); i += 2) {
            double value = 0;
            if (try_str_to_double(rest_args[i + 1], value) != str_to_type_result::normal) {
                std::println("近似值非法：{}", value);
                return;
            }
            if (!var_values.try_emplace(rest_args[i], value).second) {
                std::println("有重复的值：{}", rest_args[i]);
                return;
            }
        }
        std::println("结果：{}", algevec[idx].calculate_approximation<double>(
                                     [&](tnrw::math::variable_view var) { return var_values[var]; }));
    };

    table["alge"]["has_variable"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() > 2 || rest_args.empty()) {
            std::println("用法: alge has_variable [index] 或 alge has_variable [index] [variable]");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        if (rest_args.size() == 1) {
            std::println("结果：{}", algevec[idx].has_variable());
        } else {
            std::println("结果：{}", algevec[idx].has_variable(rest_args[1]));
        }
    };

    table["alge"]["toNum"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 2) {
            std::println("用法: alge toNum [index1] [index2]");
            return;
        }
        std::size_t idx1 = 0;
        if (try_str_to_ull(rest_args[0], idx1) != str_to_type_result::normal) {
            std::println("index1 非法");
            return;
        }
        if (idx1 >= algevec.size()) {
            std::println("index1 越界");
            return;
        }
        std::size_t idx2 = 0;
        if (try_str_to_ull(rest_args[1], idx2) != str_to_type_result::normal) {
            std::println("index2 非法");
            return;
        }
        if (idx2 > numvec.size()) {
            std::println("index2 越界");
            return;
        }

        if (algevec[idx1].has_variable()) {
            std::println("error: 第{}个代数式有变量，无法转换", idx1);
        } else {
            numvec.emplace(numvec.begin() + static_cast<long long>(idx2),
                           *std::move(algevec[idx1]).to_numeric_expression());
            algevec.erase(algevec.begin() + static_cast<long long>(idx1));
        }
    };

    table["num"]["help"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.empty() || (rest_args.size() == 1 && rest_args[0] == "help")) {
            std::println("num 命令列表：");
            std::println("如要详细查看某条命令的使用方式，请键入 num help [command] 获取");
            for (const auto &[cmd, brief] : std::views::zip(num_cmds, num_briefs)) {
                std::println("  {}  {}", cmd, brief);
            }
            return;
        }
        if (rest_args.size() == 1) {
            for (const auto &[cmd, detail] : std::views::zip(num_cmds, num_details)) {
                if (rest_args[0] == cmd) {
                    std::println("{}", detail);
                    return;
                }
            }
            std::println("未知命令：{}", rest_args[0]);
            return;
        }
        std::println("用法: num help [command]");
    };

    table["num"]["new"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num new [index]");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
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
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
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
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        numvec[idx].clear();
    };

    table["num"]["change_to_opposite"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num change_to_opposite [index]");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= numvec.size()) {
            std::println("index 越界");
            return;
        }
        numvec[idx].change_to_opposite();
    };

    table["num"]["compare"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 2) {
            std::println("用法: num compare [i] [j]");
            return;
        }
        std::size_t first = 0;
        std::size_t second = 0;
        if (try_str_to_ull(rest_args[0], first) != str_to_type_result::normal
            || try_str_to_ull(rest_args[1], second) != str_to_type_result::normal) {
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
        if (rest_args.empty()) {
            std::println("用法: num operator [index] ...");
            return;
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
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
            std::println("用法: num operator [index] [op] [value] 或 "
                         "num operator [index1] [op] numexpr [index2]");
            return;
        }
        if (rest_args[2] == "numexpr") {
            if (rest_args.size() != 4) {
                std::println("用法: num operator [index1] [op] numexpr [index2]");
                return;
            }
            std::size_t idx2 = 0;
            if (try_str_to_ull(rest_args[3], idx2) != str_to_type_result::normal) {
                std::println("index 非法");
                return;
            }
            if (idx2 >= numvec.size()) {
                std::println("index 越界");
                return;
            }
            if (oper == "+") {
                std::println("结果：{}", numvec[idx] + numvec[idx2]);
            } else if (oper == "-") {
                std::println("结果：{}", numvec[idx] - numvec[idx2]);
            } else if (oper == "*") {
                std::println("结果：{}", numvec[idx] * numvec[idx2]);
            } else if (oper == "/") {
                std::println("结果：{}", numvec[idx] / numvec[idx2]);
            } else if (oper == "+=") {
                numvec[idx] += numvec[idx2];
            } else if (oper == "-=") {
                numvec[idx] -= numvec[idx2];
            } else if (oper == "*=") {
                numvec[idx] *= numvec[idx2];
            } else if (oper == "/=") {
                numvec[idx] /= numvec[idx2];
            } else {
                std::println("不支持的操作：{}", oper);
            }
        } else {
            if (rest_args.size() != 3) {
                std::println("用法: num operator [index] [op] [value]");
                return;
            }
            long long val = 0;
            if (try_str_to_ll(rest_args[2], val) != str_to_type_result::normal) {
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
        }
    };

    table["num"]["calculate_approximation"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 1) {
            std::println("用法: num calculate_approximation [index]");
        }
        std::size_t idx = 0;
        if (try_str_to_ull(rest_args[0], idx) != str_to_type_result::normal) {
            std::println("index 非法");
            return;
        }
        if (idx >= algevec.size()) {
            std::println("index 越界");
            return;
        }
        std::println("结果：{}", numvec[idx].calculate_approximation<double>());
    };
    table["num"]["toAlge"] = [&](std::span<const std::string> rest_args) {
        if (rest_args.size() != 2) {
            std::println("用法: num toAlge [index1] [index2]");
            return;
        }
        std::size_t idx1 = 0;
        if (try_str_to_ull(rest_args[0], idx1) != str_to_type_result::normal) {
            std::println("index1 非法");
            return;
        }
        if (idx1 >= numvec.size()) {
            std::println("index1 越界");
            return;
        }
        std::size_t idx2 = 0;
        if (try_str_to_ull(rest_args[1], idx2) != str_to_type_result::normal) {
            std::println("index2 非法");
            return;
        }
        if (idx2 > algevec.size()) {
            std::println("index2 越界");
            return;
        }

        algevec.emplace(algevec.begin() + static_cast<long long>(idx2), std::move(numvec[idx1]));
        numvec.erase(numvec.begin() + static_cast<long long>(idx1));
    };

    while (true) {
        // 输出数组
        if (print_verbose) {
            std::println("现在 alge 数组：");
        } else {
            std::println("alge:");
        }
        for (std::size_t i = 0; i < algevec.size(); ++i) { std::println("{}: {}", i, algevec[i]); }
        if (print_verbose) {
            std::println("现在 num 数组：");
        } else {
            std::println("num:");
        }
        for (std::size_t i = 0; i < numvec.size(); ++i) { std::println("{}: {}", i, numvec[i]); }

        // 获取输入
        if (print_verbose) {
            std::print(">>> ");
        }
        (void)std::fflush(stdout);
        auto args = get_input_args();
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
        constexpr int max_expand = 12;
        int           depth = 0;
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

        // 处理退出命令
        if (args.size() == 1 && (args[0] == "quit" || args[0] == "exit")) {
            break;
        }

        // 处理 help 命令
        if (args[0] == "help") {
            if (args.size() == 1) {
                std::println("目前的可用类型： alge  num");
                std::println(
                    "查看某个类型的命令列表与其帮助： help alge 或 help num 或 alge help 或 num help");
                std::println("使用： alge ... 或 num ...");
                std::println("退出： quit 或 exit");
                continue;
            }
            // help [type] [command...]
            const std::string &help_type = args[1];
            auto               type_it = table.find(help_type);
            if (type_it == table.end()) {
                std::println("未知类型：{}", help_type);
                continue;
            }
            auto help_it = type_it->second.find("help");
            if (help_it == type_it->second.end()) {
                std::println("该类型无帮助信息：{}", help_type);
                continue;
            }
            // 将剩余参数传递给类型的 help 处理器（可能为空或含命令名）
            help_it->second(std::span(&args[2], args.size() - 2));
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