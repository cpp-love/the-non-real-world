/**
 * @file expressions_base.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了一些表达式的基本元素
 * @version 0.1.0-4
 * @date 2026-03-15
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 声明了 `integer_constant_type` 和 `variable_type` 来表示常量与变量类型
 * - 声明了 `node` 表示代数式的节点类型
 * 
 */

#ifndef TNRW_MATH_EXPRESSIONS_BASE_HPP
#define TNRW_MATH_EXPRESSIONS_BASE_HPP

#include "base/overload.hpp"
#include "base/type_traits.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace tnrw {

    namespace math {

        // using别名
        using integer_constant_type = long long; ///< 常量值类型
        using variable_type = std::string;       ///< 变量字符串类型
        using variable_view = std::string_view;  ///< 变量字符串视图类型

        /// @brief 代数式基本节点
        struct node;
        using node_ptr = std::unique_ptr<node>; ///< 节点的智能指针的别名

        /**
         * @brief 单项式节点
         * @details 
         *  - 其下没有子节点
         */
        struct monomial {
            integer_constant_type m_coeff = 0; ///< 单项式系数
            using map_var_exp = std::map<variable_view, integer_constant_type,
                                         std::ranges::less>; //< 变量 -> 指数 的键值对
            map_var_exp m_var_exps{};                        ///< 变量与幂次的键值对
        };

        /**
         * @brief 加法节点
         * @detailts
         *  - 若加的项中有常量（包括非整型的），则必须为 `m_children` 成员的第0项（即 `m_children[0]` 的位置）
         *  - 其下的直接子节点只能为 @ref monomial, @ref division
         */
        struct addition {
            std::vector<node_ptr> m_children; ///< 子节点

            /**
             * @brief 从节点列表构造的构造函数
             * @tparam Args 参数类型
             * @param [in] args 节点列表
             */
            template <typename... Args>
                requires(std::is_same_v<Args &&, node_ptr &&> && ...)
            explicit addition(Args &&...args) noexcept;
            /**
             * @brief 深复制构造函数
             * @param [in] rhs 另一个对象
             */
            addition(const addition &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个对象
             */
            addition(addition &&rhs) noexcept = default;
            /// @brief 析构函数
            ~addition() noexcept = default;
            /// @brief 禁止复制赋值运算符重载
            addition &operator=(const addition &rhs) noexcept = delete;
            /// @brief 禁止移动赋值运算符重载
            addition &operator=(addition &&rhs) noexcept = delete;
        };

        /**
         * @brief 乘法节点
         * @details
         *  - 此节点为临时节点，在简化后不应出现
         */
        struct multiplication {
            std::vector<node_ptr> m_children; ///< 子节点

            /**
             * @brief 从节点列表构造的构造函数
             * @tparam Args 参数类型
             * @param [in] args 节点列表
             */
            template <typename... Args>
                requires(std::is_same_v<Args &&, node_ptr &&> && ...)
            explicit multiplication(Args &&...args) noexcept;
            /**
             * @brief 深复制构造函数
             * @param [in] rhs 另一个对象
             */
            multiplication(const multiplication &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个对象
             */
            multiplication(multiplication &&rhs) noexcept = default;
            /// @brief 析构函数
            ~multiplication() noexcept = default;
            /// @brief 禁止复制赋值运算符重载
            multiplication &operator=(const multiplication &rhs) noexcept = delete;
            /// @brief 禁止移动赋值运算符重载
            multiplication &operator=(multiplication &&rhs) noexcept = delete;
        };

        /**
         * @brief 除法节点
         * @details
         *  - `m_children[0]` 为分子， `m_children[1]` 为分母
         *  - 其分子的最高同一字母的最高此项应小于分母的
         *  - 其分子分母的最高此项的系数必须为正
         *  - 应提取所有在子节点中可提取的取相反数的操作，若本身为负，则在外面套 @ref Negation 节点
         *  - 此节点下的变量必须按名字排序
         *  - 其下的直接子节点只能为 @ref monomial, @ref addition
         */
        struct division {
            std::array<node_ptr, 2> m_children; ///< 子节点

            /**
             * @brief 从分子分母节点构造的构造函数
             * @param [in] num 分子
             * @param [in] den 分母
             */
            division(node_ptr num, node_ptr den) noexcept;
            /**
             * @brief 深复制构造函数
             * @param [in] rhs 另一个对象
             */
            division(const division &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个对象
             */
            division(division &&rhs) noexcept = default;
            /// @brief 析构函数
            ~division() noexcept = default;
            /// @brief 禁止复制赋值运算符重载
            division &operator=(const division &rhs) noexcept = delete;
            /// @brief 禁止移动赋值运算符重载
            division &operator=(division &&rhs) noexcept = delete;
        };

        /// @brief 代数式基本节点
        struct node {
            using variant_type = std::variant<monomial, addition, multiplication, division>;
            variant_type m_value; ///< 值
        };

        /**
         * @brief 概念：是子节点的类型
         * @tparam T 类型
         */
        template <typename T>
        concept sub_node = is_variant_member_v<T, node::variant_type>;

        /**
         * @brief 创建代数式节点的工厂函数
         * @tparam SubNodeT 创建的子节点类型
         * @tparam Args 参数类型
         * @param [in] args 创建子节点的参数
         * @return node_ptr 创建的节点
         */
        template <sub_node SubNodeT, typename... Args>
            requires std::constructible_from<SubNodeT, Args...>
        [[nodiscard]] inline node_ptr make_node(Args &&...args) noexcept {
            return std::make_unique<node>(SubNodeT{std::forward<Args>(args)...});
        }

        /**
         * @brief 判断代数式节点是否为0
         * @param [in] root 代数式节点
         * @return true 代数式节点为0
         * @return false 代数式节点不为0
         * @warning 这要求 root 节点被化简过
         */
        inline bool is_zero(const node_ptr &root) noexcept {
            return std::holds_alternative<monomial>(root->m_value)
                   && std::get<monomial>(root->m_value).m_coeff == 0;
        }
        /**
         * @brief 判断两个代数式是否相等
         * @param [in] lhs 代数式1
         * @param [in] rhs 代数式2
         * @return true 相等
         * @return false 不相等
         * @warning 这要求 root 节点被化简过
         * @bug 此函数会将 1 + x 与 x + 1 判断为不相等， 1 + (1 / x) 与 (x + 1) / x 也会被判断为不相等。
         *      计划通过规范化简步骤，实现相同代数式的化简结果一致来解决
         */
        inline bool is_equal(const node_ptr &lhs, const node_ptr &rhs) noexcept {
            return std::visit(
                make_overloaded(
                    [&](const monomial &value1, const monomial &value2) -> bool {
                        if (value1.m_coeff != value2.m_coeff) {
                            return false;
                        }
                        return value1.m_var_exps == value2.m_var_exps;
                    },
                    [&]<typename T>(const T &value1, const T &value2) -> bool {
                        if (value1.m_children.size() != value2.m_children.size()) {
                            return false;
                        }
                        return std::ranges::all_of(std::views::zip(value1.m_children, value2.m_children),
                                                   [](const auto &elem) {
                                                       const auto &[ch1, ch2] = elem;
                                                       return is_equal(ch1, ch2);
                                                   });
                    },
                    [&](const auto & /*unused*/, const auto & /*unused*/) { return false; }),
                lhs->m_value, rhs->m_value);
        }

        /**
         * @brief 判断代数式是否有指定变量
         * @param [in] lhs 代数式节点
         * @param [in] variable 指定变量，若为 `std::nullopt`，则查询是否有任意变量
         * @return true 有指定变量
         * @return false 没有指定变量
         * @warning 这要求 root 节点被化简过
         */
        inline bool has_variable(const node_ptr              &lhs,
                                 std::optional<variable_view> variable = std::nullopt) noexcept {
            return std::visit(make_overloaded(
                                  [&](const monomial &value) {
                                      return variable.has_value() ? value.m_var_exps.contains(*variable)
                                                                  : !value.m_var_exps.empty();
                                  },
                                  [&](const auto &value) {
                                      return std::ranges::any_of(
                                          value.m_children, [&](const node_ptr &sub_node) {
                                              return has_variable(sub_node, variable);
                                          });
                                  }),
                              lhs->m_value);
        }

        /**
         * @brief 将节点转换为 std::string
         * @param [in] root 代数式节点
         * @return std::string 节点的字符串表示
         * @attention 建议 root 节点被化简过
         */
        inline std::string node_to_string(const node_ptr &root) {
            return std::visit(make_overloaded(
                                  [](const monomial &value) {
                                      std::string str;
                                      str += std::format("{}", value.m_coeff);
                                      for (auto [var, exp] : value.m_var_exps) {
                                          if (exp == 1) {
                                              str += std::format("*{}", var);
                                          } else {
                                              str += std::format("*{}^{}", var, exp);
                                          }
                                      }
                                      if (str.size() > 2 && value.m_coeff == 1) {
                                          return std::move(str).substr(2);
                                      }
                                      return str;
                                  },
                                  [](const addition &value) {
                                      std::string str = "(";
                                      for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                                          if (i) {
                                              str += ")+(";
                                          }
                                          str += node_to_string(value.m_children[i]);
                                      }
                                      str += ")";
                                      return str;
                                  },
                                  [](const multiplication &value) {
                                      std::string str = "(";
                                      for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                                          if (i) {
                                              str += ")*(";
                                          }
                                          str += node_to_string(value.m_children[i]);
                                      }
                                      str += ")";
                                      return str;
                                  },
                                  [](const division &value) {
                                      return std::format("({})/({})",
                                                         node_to_string(value.m_children[0]),
                                                         node_to_string(value.m_children[1]));
                                  }),
                              root->m_value);
        }

        /**
         * @brief 计算代数式的近似值
         * @tparam FloatT 近似值的结果（浮点数）
         * @param [in] root 代数式节点
         * @param [in] converter 获取变量对应的近似值的函数，参数是变量的视图，返回值是变量对应的近似值
         * @return FloatT 代数式的近似值
         */
        template <std::floating_point FloatT>
        FloatT calculate_approximation(const node_ptr                             &root,
                                       const std::function<FloatT(variable_view)> &converter) {
            return std::visit(
                make_overloaded(
                    [&](const monomial &value) {
                        auto ret = static_cast<FloatT>(value.m_coeff);
                        for (auto [var, exp] : value.m_var_exps) {
                            ret *= std::pow(converter(var), exp);
                        }
                        return ret;
                    },
                    [&](const addition &value) {
                        FloatT sum = 0;
                        for (const auto &child : value.m_children) {
                            sum += calculate_approximation<FloatT>(child, converter);
                        }
                        return sum;
                    },
                    [&](const multiplication &value) {
                        FloatT mul = 1;
                        for (const auto &child : value.m_children) {
                            mul *= calculate_approximation<FloatT>(child, converter);
                        }
                        return mul;
                    },
                    [&](const division &value) {
                        return calculate_approximation<FloatT>(value.m_children[0], converter)
                               / calculate_approximation<FloatT>(value.m_children[1], converter);
                    }),
                root->m_value);
        }

        /**
         * @brief 计算代数式的精确值
         * @param [in] root 代数式节点
         * @param [in] converter 获取变量对应的精确值的函数，参数是变量的视图，返回值是变量对应的精确值
         * @return node_ptr 代数式的精确值
         */
        inline node_ptr calculate(const node_ptr                               &root,
                                  const std::function<node_ptr(variable_view)> &converter) {
            return std::visit(make_overloaded(
                                  [&](const monomial &value) {
                                      multiplication mul_ans(make_node<monomial>(value.m_coeff));
                                      mul_ans.m_children.reserve(value.m_var_exps.size() + 1);
                                      for (auto [var, exp] : value.m_var_exps) {
                                          node_ptr var_value = converter(var);
                                          while (exp--) {
                                              mul_ans.m_children.push_back(
                                                  std::make_unique<node>(*var_value));
                                          }
                                      }
                                      return make_node<multiplication>(std::move(mul_ans));
                                  },
                                  [&]<typename T>(const T &value) {
                                      T cpy = value;
                                      for (auto &child : cpy.m_children) {
                                          child = calculate(child, converter);
                                      }
                                      return make_node<T>(std::move(cpy));
                                  }),
                              root->m_value);
        }

    } // namespace math

    inline namespace literals {

        /// @brief `algebraic_expression` 和 `numeric_expression` 类的内联自定义字面量命名空间
        inline namespace expressions_base_literals {

            /**
             * @brief 创建常量
             * @param [in] constant 常量值字面量
             * @return math::integer_constant_type 创建的常量类型
             */
            [[nodiscard]] constexpr math::integer_constant_type
                                              operator""_c(unsigned long long constant) noexcept;
            /**
             * @brief 创建变量
             * @param [in] variable 变量字符串字面量
             * @param [in] len 变量字符串字面量的长度
             * @return math::variable_type 创建的变量类型
             */
            [[nodiscard]] math::variable_type operator""_v(const char *variable,
                                                           std::size_t len) noexcept;
            /**
             * @brief 创建变量视图
             * @param [in] variable 变量字符串字面量
             * @param [in] len 变量字符串字面量的长度
             * @return math::variable_view 创建的变量视图
             */
            [[nodiscard]] math::variable_view operator""_vv(const char *variable,
                                                            std::size_t len) noexcept;

        } // namespace expressions_base_literals

    } // namespace literals

} // namespace tnrw

#endif // TNRW_MATH_EXPRESSIONS_BASE_HPP