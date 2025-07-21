/**
 * @file testAlgeExpr.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 用来测试 `Maths::AlgebraicExpression` 类的源文件
 * @version 0.1.0-1
 * @date 2025-07-13
 * 
 * @copyright Copyright 2025 cpp-love
 * 
 */

#include "Maths/AlgebraicExpression.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace tnrw::literals::AlgebraicExpression_literals;

const std::vector<std::string> commands{"help",     "new",        "delete",
                                        "operator", "clear",      "getValue",
                                        "isZero",   "toOpposite", "compare"};
const std::vector<std::string> briefs{"提供帮助",
                                      "添加代数式",
                                      "删除代数式",
                                      "对一个代数式使用运算符",
                                      "清除一个代数式",
                                      "获取一个代数式的值(暂未加入)",
                                      "判断一个代数式是否为0",
                                      "对一个代数式取相反数",
                                      "比较两个代数式"};
const std::vector<std::string> details{"\
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
- getValue [index] ??\n\
  本节未完成\n\
  原因：未定义tnrw::Maths::AlgebraicExpression的getValue方法", "\
- isZero [index] ??\n\
  本节未完成\n\
  原因：未定义tnrw::Maths::AlgebraicExpression的isZero方法", "\
- toOpposite [index]\n\
  原因将第index(从0开始)个代数式改为相反数", "\
- compare [index1] [index2]\n\
  用于比较第index1(从0开始)个代数式和第index2(从0开始)个代数式是否相等"};

std::vector<tnrw::Maths::AlgebraicExpression> algevec(1);

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    while (true) {
        std::cout << ">>> " << std::flush;
        std::string command;
        std::getline(std::cin, command);
        if (command.empty())
            continue;
        std::size_t index = command.find(' ');
        if (index == std::string::npos) {
            index = command.size();
        }
        std::string cmd = command.substr(0, index);
        if (cmd == "help") {
            if (index == command.size()) {
                for (std::size_t i = 0; i < commands.size(); ++i) {
                    std::cout << commands[i] << briefs[i] << std::endl;
                }
            } else {
                std::string arg = command.substr(index + 1);
                bool        flag = false;
                for (std::size_t i = 0; i < commands.size(); ++i) {
                    if (arg == commands[i]) {
                        std::cout << details[i] << std::endl;
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    std::cout << "错误：不存在命令 \"" << cmd
                              << "\"，请输入 \"help\" 获取帮助" << std::endl;
                }
            }
        } else if (cmd == "new") {
            if (index == command.size()) {
                std::cout << "错误：命令 \"new\" 无法接收参数，请输入 \"help\" 获取帮助"
                          << std::endl;
            } else {
                std::string arg = command.substr(index + 1);
                errno = 0;
                char     *endptr = nullptr;
                long long num = std::strtoll(arg.c_str(), &endptr, 0);
                if (endptr == nullptr || *endptr != '\0') {
                    std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                              << std::endl;
                } else if (errno == ERANGE) {
                    std::cout << "错误：参数过大，请输入 \"help\" 获取帮助" << std::endl;
                } else {
                    algevec.emplace(algevec.begin() + num);
                }
            }
        } else if (cmd == "delete") {
            if (index == command.size()) {
                std::cout
                    << "错误：命令 \"delete\" 无法接收参数，请输入 \"help\" 获取帮助"
                    << std::endl;
            } else {
                std::string arg = command.substr(index + 1);
                errno = 0;
                char     *endptr = nullptr;
                long long num = std::strtoll(arg.c_str(), &endptr, 0);
                if (endptr == nullptr || *endptr != '\0') {
                    std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                              << std::endl;
                } else if (errno == ERANGE) {
                    std::cout << "错误：参数过大，请输入 \"help\" 获取帮助" << std::endl;
                } else {
                    algevec.erase(algevec.begin() + num);
                }
            }
        } else if (cmd == "operator") {
            if (index == command.size()) {
                std::cout << "错误：命令 \"operator\" 没有参数，请输入 \"help\" 获取帮助"
                          << std::endl;
            } else {
                std::string args = command.substr(index + 1);
                std::size_t pos = args.find(' ');
                if (pos == std::string::npos) {
                    std::cout << "错误：参数过少，请输入 \"help\" 获取帮助" << std::endl;
                }
                std::string arg1 = args.substr(0, pos);
                errno = 0;
                char     *endptr = nullptr;
                long long index = std::strtoll(arg1.c_str(), &endptr, 0);
                if (endptr == nullptr || *endptr != '\0') {
                    std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                              << std::endl;
                } else if (errno == ERANGE) {
                    std::cout << "错误：参数过大，请输入 \"help\" 获取帮助" << std::endl;
                } else {
                    std::string arg2s = args.substr(pos + 1);
                    std::size_t pos2 = arg2s.find(' ');
                    if (pos2 == std::string::npos) {
                        pos2 = arg2s.size();
                    }
                    std::string arg2 = arg2s.substr(0, pos2);
                    if (arg2 == "front++") {
                        std::cout << "now: " << (algevec[index]++).toString()
                                  << std::endl;
                    } else if (arg2 == "front--") {
                        std::cout << "now: " << (algevec[index]++).toString()
                                  << std::endl;
                    } else if (arg2 == "back++") {
                        std::cout << "now: " << (++algevec[index]).toString()
                                  << std::endl;
                    } else if (arg2 == "back--") {
                        std::cout << "now: " << (--algevec[index]).toString()
                                  << std::endl;
                    } else if (arg2 == "+") {
                        if (pos2 == arg2s.size()) {
                            std::cout << "now: " << (+algevec[index]).toString()
                                      << std::endl;
                        } else {
                            std::string arg3s = arg2s.substr(pos2 + 1);
                            std::size_t pos3 = arg3s.find(' ');
                            if (pos3 == std::string::npos) {
                                std::cout << "错误：参数过少，请输入 \"help\" 获取帮助"
                                          << std::endl;
                            } else {
                                std::string arg3 = args.substr(0, pos);
                                std::string arg4 = args.substr(pos + 1);
                                errno = 0;
                                char *endptr = nullptr;
                                endptr = nullptr;
                                long long num = std::strtoll(arg4.c_str(), &endptr, 0);
                                if (endptr == nullptr || *endptr != '\0') {
                                    std::cout
                                        << "错误：非法的参数，请输入 \"help\" 获取帮助"
                                        << std::endl;
                                } else if (errno == ERANGE) {
                                    std::cout
                                        << "错误：参数过大，请输入 \"help\" 获取帮助"
                                        << std::endl;
                                } else {
                                    if (arg3 == "constant") {
                                        std::cout
                                            << "now: "
                                            << (algevec[index]
                                                + static_cast<
                                                    tnrw::Maths::AlgebraicExpression::
                                                        ConstantType>(num))
                                                   .toString()
                                            << std::endl;
                                    } else if (arg3 == "variable") {
                                        if (-127 < num || num > 127) {
                                            std::cout << "错误：参数过大，请输入 "
                                                         "\"help\" 获取帮助"
                                                      << std::endl;
                                        } else {
                                            std::cout
                                                << "now: "
                                                << (algevec[index]
                                                    + static_cast<
                                                        tnrw::Maths::AlgebraicExpression::
                                                            VariableType>(num))
                                                       .toString()
                                                << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    } else if (arg2 == "-") {
                        if (pos2 == arg2s.size()) {
                            std::cout << "now: " << (-algevec[index]).toString()
                                      << std::endl;
                        } else {
                            std::string arg3s = arg2s.substr(pos2 + 1);
                            std::size_t pos3 = arg3s.find(' ');
                            if (pos3 == std::string::npos) {
                                std::cout << "错误：参数过少，请输入 \"help\" 获取帮助"
                                          << std::endl;
                            } else {
                                std::string arg3 = args.substr(0, pos);
                                std::string arg4 = args.substr(pos + 1);
                                errno = 0;
                                char *endptr = nullptr;
                                endptr = nullptr;
                                long long num = std::strtoll(arg4.c_str(), &endptr, 0);
                                if (endptr == nullptr || *endptr != '\0') {
                                    std::cout
                                        << "错误：非法的参数，请输入 \"help\" 获取帮助"
                                        << std::endl;
                                } else if (errno == ERANGE) {
                                    std::cout
                                        << "错误：参数过大，请输入 \"help\" 获取帮助"
                                        << std::endl;
                                } else {
                                    if (arg3 == "constant") {
                                        std::cout
                                            << "now: "
                                            << (algevec[index]
                                                + static_cast<
                                                    tnrw::Maths::AlgebraicExpression::
                                                        ConstantType>(num))
                                                   .toString()
                                            << std::endl;
                                    } else if (arg3 == "variable") {
                                        if (-127 < num || num > 127) {
                                            std::cout << "错误：参数过大，请输入 "
                                                         "\"help\" 获取帮助"
                                                      << std::endl;
                                        } else {
                                            std::cout
                                                << "now: "
                                                << (algevec[index]
                                                    + static_cast<
                                                        tnrw::Maths::AlgebraicExpression::
                                                            VariableType>(num))
                                                       .toString()
                                                << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (pos2 == arg2s.size()) {
                        std::cout << "错误：参数过少，请输入 \"help\" 获取帮助"
                                  << std::endl;
                    } else {
                        std::string arg3s = arg2s.substr(pos2 + 1);
                        std::size_t pos3 = arg3s.find(' ');
                        if (pos3 == std::string::npos) {
                            std::cout << "错误：参数过少，请输入 \"help\" 获取帮助"
                                      << std::endl;
                        } else {
                            std::string arg3 = args.substr(0, pos);
                            std::string arg4 = args.substr(pos + 1);
                            errno = 0;
                            char *endptr = nullptr;
                            endptr = nullptr;
                            long long num = std::strtoll(arg4.c_str(), &endptr, 0);
                            if (endptr == nullptr || *endptr != '\0') {
                                std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                                          << std::endl;
                            } else if (errno == ERANGE) {
                                std::cout << "错误：参数过大，请输入 \"help\" 获取帮助"
                                          << std::endl;
                            } else {
                                if (arg3 == "constant") {
                                    if (arg2 == "*") {
                                        std::cout
                                            << "now: "
                                            << (algevec[index]
                                                * static_cast<
                                                    tnrw::Maths::AlgebraicExpression::
                                                        ConstantType>(num))
                                                   .toString()
                                            << std::endl;
                                    } else if (arg2 == "/") {
                                        std::cout
                                            << "now: "
                                            << (algevec[index]
                                                / static_cast<
                                                    tnrw::Maths::AlgebraicExpression::
                                                        ConstantType>(num))
                                                   .toString()
                                            << std::endl;
                                    } else if (arg2 == "+=") {
                                        algevec[index] +=
                                            static_cast<tnrw::Maths::AlgebraicExpression::
                                                            ConstantType>(num);
                                    } else if (arg2 == "-=") {
                                        algevec[index] +=
                                            static_cast<tnrw::Maths::AlgebraicExpression::
                                                            ConstantType>(num);
                                    } else if (arg2 == "*=") {
                                        algevec[index] +=
                                            static_cast<tnrw::Maths::AlgebraicExpression::
                                                            ConstantType>(num);
                                    } else if (arg2 == "/=") {
                                        algevec[index] +=
                                            static_cast<tnrw::Maths::AlgebraicExpression::
                                                            ConstantType>(num);
                                    } else {
                                        std::cout << "错误：非法的参数，请输入 \"help\" "
                                                     "获取帮助"
                                                  << std::endl;
                                    }
                                } else if (arg3 == "variable") {
                                    if (-127 < num || num > 127) {
                                        std::cout
                                            << "错误：参数过大，请输入 \"help\" 获取帮助"
                                            << std::endl;
                                    } else {
                                        if (arg2 == "*") {
                                            std::cout
                                                << "now: "
                                                << (algevec[index]
                                                    * static_cast<
                                                        tnrw::Maths::AlgebraicExpression::
                                                            VariableType>(num))
                                                       .toString()
                                                << std::endl;
                                        } else if (arg2 == "/") {
                                            std::cout
                                                << "now: "
                                                << (algevec[index]
                                                    / static_cast<
                                                        tnrw::Maths::AlgebraicExpression::
                                                            VariableType>(num))
                                                       .toString()
                                                << std::endl;
                                        } else if (arg2 == "+=") {
                                            algevec[index] += static_cast<
                                                tnrw::Maths::AlgebraicExpression::
                                                    VariableType>(num);
                                        } else if (arg2 == "-=") {
                                            algevec[index] -= static_cast<
                                                tnrw::Maths::AlgebraicExpression::
                                                    VariableType>(num);
                                        } else if (arg2 == "*=") {
                                            algevec[index] *= static_cast<
                                                tnrw::Maths::AlgebraicExpression::
                                                    VariableType>(num);
                                        } else if (arg2 == "/=") {
                                            algevec[index] /= static_cast<
                                                tnrw::Maths::AlgebraicExpression::
                                                    VariableType>(num);
                                        } else {
                                            std::cout << "错误：非法的参数，请输入 "
                                                         "\"help\" 获取帮助"
                                                      << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (cmd == "clear") {
            if (index == command.size()) {
                std::cout << "错误：命令 \"clear\" 没有参数，请输入 \"help\" 获取帮助"
                          << std::endl;
            } else {
                std::string arg = command.substr(index + 1);
                errno = 0;
                char     *endptr = nullptr;
                long long num = std::strtoll(arg.c_str(), &endptr, 0);
                if (endptr == nullptr || *endptr != '\0') {
                    std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                              << std::endl;
                } else if (errno == ERANGE) {
                    std::cout << "错误：参数过大，请输入 \"help\" 获取帮助" << std::endl;
                } else {
                    algevec[num].clear();
                }
            }
        } else if (cmd == "getValue") {
            std::cout << "本节未完成\n原因：未定义tnrw::Maths::"
                         "AlgebraicExpression的getValue方法"
                      << std::endl;
        } else if (cmd == "isZero") {
            std::cout
                << "本节未完成\n原因：未定义tnrw::Maths::AlgebraicExpression的isZero方法"
                << std::endl;
        } else if (cmd == "toOpposite") {
            if (index == command.size()) {
                std::cout
                    << "错误：命令 \"toOpppsite\" 无法接收参数，请输入 \"help\" 获取帮助"
                    << std::endl;
            } else {
                std::string arg = command.substr(index + 1);
                errno = 0;
                char     *endptr = nullptr;
                long long num = std::strtoll(arg.c_str(), &endptr, 0);
                if (endptr == nullptr || *endptr != '\0') {
                    std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                              << std::endl;
                } else if (errno == ERANGE) {
                    std::cout << "错误：参数过大，请输入 \"help\" 获取帮助" << std::endl;
                } else {
                    algevec[num].changeToOpposite();
                }
            }
        } else if (cmd == "compare") {
            if (index == command.size()) {
                std::cout
                    << "错误：命令 \"compare\" 无法接收参数，请输入 \"help\" 获取帮助"
                    << std::endl;
            } else {
                std::string args = command.substr(index + 1);
                std::size_t pos = args.find(' ');
                if (pos == std::string::npos) {
                    std::cout << "错误：参数过少，请输入 \"help\" 获取帮助" << std::endl;
                } else {
                    std::string arg = args.substr(0, pos);
                    std::string arg2 = args.substr(pos + 1);
                    errno = 0;
                    char     *endptr = nullptr;
                    bool      flag = false;
                    long long num1 = std::strtoll(arg.c_str(), &endptr, 0);
                    if (endptr == nullptr || *endptr != '\0')
                        flag = true;
                    endptr = nullptr;
                    long long num2 = std::strtoll(arg2.c_str(), &endptr, 0);
                    if (flag || endptr == nullptr || *endptr != '\0') {
                        std::cout << "错误：非法的参数，请输入 \"help\" 获取帮助"
                                  << std::endl;
                    } else if (errno == ERANGE) {
                        std::cout << "错误：参数过大，请输入 \"help\" 获取帮助"
                                  << std::endl;
                    } else {
                        std::cout << "compare: " << std::ios::boolalpha
                                  << (algevec[num1] == algevec[num2]) << std::endl;
                    }
                }
            }
        } else {
            std::cout << "错误：不存在命令 \"" << cmd << "\"，请输入 \"help\" 获取帮助"
                      << std::endl;
        }

        std::cout << "现在代数式数组为：" << std::endl;
        for (std::size_t i = 0; i < algevec.size(); ++i) {
            std::cout << i << ": " << algevec[i].toString() << std::endl;
        }
    }

    return 0;
}