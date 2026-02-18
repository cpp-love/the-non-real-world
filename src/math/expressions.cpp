/**
 * @file expressions.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了代数式和无字母的代数式类的细节
 * @version 0.1.0-4
 * @date 2026-02-14
 * 
 * @copyright cpp-love
 * 
 * @details
 *  - 通过3个节点 @ref Monomial @ref Addition @ref Division ,
 *    一个辅助节点 @ref Multiplication 和一个别名 @ref Polynomial
 *    来实现 @ref Node 节点
 *  - 通过 std::visit 的访问来实现各种功能
 */

#include "base/assert_msg.hpp"
#include "base/overload.hpp"
#include "math/algebraic_expression.hpp"
#include "math/numeric_expression.hpp"
#include "math/expressions_base.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <print>
#include <ranges>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

/// @cond INTERNAL

/// @brief @ref tnrw::math 命名空间的一些功能的实现细节
namespace tnrw::math::details {

    /**
     * @brief 单项式节点
     * @details 
     *  - 其下没有子节点
     */
    struct Monomial {
        IntegerConstantType m_coeff = 0; ///< 单项式系数
        using MapVarExp =
            std::map<VariableView, IntegerConstantType, std::ranges::less>; //< 变量 -> 指数 的键值对
        MapVarExp m_var_exps{};                                             ///< 变量与幂次的键值对
    };

    /**
     * @brief 加法节点
     * @detailts
     *  - 若加的项中有常量（包括非整型的），则必须为 `m_children` 成员的第0项（即 `m_children[0]` 的位置）
     *  - 其下的直接子节点只能为 @ref Monomial, @ref Division
     */
    struct Addition {
        std::vector<NodePtr> m_children; ///< 子节点

        /**
         * @brief 从节点列表构造的构造函数
         * @tparam Args 参数类型
         * @param [in] args 节点列表
         */
        template <typename... Args>
            requires(std::is_same_v<Args &&, NodePtr &&> && ...)
        explicit Addition(Args &&...args) noexcept;
        /**
         * @brief 深复制构造函数
         * @param [in] rhs 另一个对象
         */
        Addition(const Addition &rhs) noexcept;
        /**
         * @brief 移动构造函数
         * @param [in] rhs 另一个对象
         */
        Addition(Addition &&rhs) noexcept = default;
        /// @brief 析构函数
        ~Addition() noexcept = default;
        /// @brief 禁止复制赋值运算符重载
        Addition &operator=(const Addition &rhs) noexcept = delete;
        /// @brief 禁止移动赋值运算符重载
        Addition &operator=(Addition &&rhs) noexcept = delete;
    };

    /**
     * @brief 乘法节点
     * @details
     *  - 此节点为临时节点，在简化后不应出现
     */
    struct Multiplication {
        std::vector<NodePtr> m_children; ///< 子节点

        /**
         * @brief 从节点列表构造的构造函数
         * @tparam Args 参数类型
         * @param [in] args 节点列表
         */
        template <typename... Args>
            requires(std::is_same_v<Args &&, NodePtr &&> && ...)
        explicit Multiplication(Args &&...args) noexcept;
        /**
         * @brief 深复制构造函数
         * @param [in] rhs 另一个对象
         */
        Multiplication(const Multiplication &rhs) noexcept;
        /**
         * @brief 移动构造函数
         * @param [in] rhs 另一个对象
         */
        Multiplication(Multiplication &&rhs) noexcept = default;
        /// @brief 析构函数
        ~Multiplication() noexcept = default;
        /// @brief 禁止复制赋值运算符重载
        Multiplication &operator=(const Multiplication &rhs) noexcept = delete;
        /// @brief 禁止移动赋值运算符重载
        Multiplication &operator=(Multiplication &&rhs) noexcept = delete;
    };

    /**
     * @brief 除法节点
     * @details
     *  - `m_children[0]` 为分子， `m_children[1]` 为分母
     *  - 其分子的最高同一字母的最高此项应小于分母的
     *  - 其分子分母的最高此项的系数必须为正
     *  - 应提取所有在子节点中可提取的取相反数的操作，若本身为负，则在外面套 @ref Negation 节点
     *  - 此节点下的变量必须按名字排序
     *  - 其下的直接子节点只能为 @ref Monomial, @ref Addition
     */
    struct Division {
        std::array<NodePtr, 2> m_children; ///< 子节点

        /**
         * @brief 从分子分母节点构造的构造函数
         * @param [in] num 分子
         * @param [in] den 分母
         */
        Division(NodePtr num, NodePtr den) noexcept;
        /**
         * @brief 深复制构造函数
         * @param [in] rhs 另一个对象
         */
        Division(const Division &rhs) noexcept;
        /**
         * @brief 移动构造函数
         * @param [in] rhs 另一个对象
         */
        Division(Division &&rhs) noexcept = default;
        /// @brief 析构函数
        ~Division() noexcept = default;
        /// @brief 禁止复制赋值运算符重载
        Division &operator=(const Division &rhs) noexcept = delete;
        /// @brief 禁止移动赋值运算符重载
        Division &operator=(Division &&rhs) noexcept = delete;
    };

    /// @brief 代数式基本节点
    struct Node {
        using VariantType = std::variant<Monomial, Addition, Multiplication, Division>;
        VariantType m_value; ///< 值
    };

    /**
     * @brief 类型萃取：类型 `T` 是否为 变体( `std::variant` )类型 `V` 的成员
     * @tparam T 判断类型
     * @tparam V 变体类型
     */
    template <typename T, typename V>
    struct IsVariantMember : std::false_type {};
    template <typename T, typename T1, typename... Rest>
    struct IsVariantMember<T, std::variant<T1, Rest...>>
        : std::conditional_t<std::is_same_v<T, T1>, std::true_type,
                             IsVariantMember<T, std::variant<Rest...>>> {};

    /**
     * @brief 类型萃取： @ref tnrw::math::details::IsVariantMember 的值的模板缩写
     * @tparam T 判断类型
     * @tparam V 变体类型
     */
    template <typename T, typename V>
    constexpr bool is_variant_member_v = IsVariantMember<T, V>::value;

    /**
     * @brief 概念：是子节点的类型
     * @tparam T 类型
     */
    template <typename T>
    concept SubNode = is_variant_member_v<T, Node::VariantType>;

    // 为了避免 Node 未定义的问题，延后定义构造函数
    template <typename... Args>
        requires(std::is_same_v<Args &&, NodePtr &&> && ...)
    Addition::Addition(Args &&...args) noexcept {
        m_children.reserve(sizeof...(Args));
        (m_children.push_back(std::forward<Args>(args)), ...);
    }
    Addition::Addition(const Addition &rhs) noexcept {
        m_children.reserve(rhs.m_children.size());
        for (const auto &value : rhs.m_children) {
            m_children.push_back(std::make_unique<Node>(*value));
        }
    }

    template <typename... Args>
        requires(std::is_same_v<Args &&, NodePtr &&> && ...)
    Multiplication::Multiplication(Args &&...args) noexcept {
        m_children.reserve(sizeof...(Args));
        (m_children.push_back(std::forward<Args>(args)), ...);
    }
    Multiplication::Multiplication(const Multiplication &rhs) noexcept {
        m_children.reserve(rhs.m_children.size());
        for (const auto &value : rhs.m_children) {
            m_children.push_back(std::make_unique<Node>(*value));
        }
    }

    Division::Division(NodePtr num, NodePtr den) noexcept : m_children{std::move(num), std::move(den)} {}
    Division::Division(const Division &rhs) noexcept
        : m_children{std::make_unique<Node>(*rhs.m_children[0]),
                     std::make_unique<Node>(*rhs.m_children[1])} {}

    /**
     * @brief 创建代数式节点的工厂函数
     * @tparam SubNodeT 创建的子节点类型
     * @tparam Args 参数类型
     * @param [in] args 创建子节点的参数
     * @return NodePtr 创建的节点
     */
    template <SubNode SubNodeT, typename... Args>
        requires std::constructible_from<SubNodeT, Args...>
    [[nodiscard]] NodePtr makeNode(Args &&...args) noexcept {
        return std::make_unique<Node>(SubNodeT{std::forward<Args>(args)...});
    }

    /**
     * @brief 生成空节点（为0）
     * @return NodePtr 空节点
     */
    NodePtr createZeroNode() noexcept { return makeNode<Monomial>(0); }

    /**
     * @brief 判断代数式节点是否为0
     * @param [in] root 代数式节点 
     * @return true 代数式节点为0
     * @return false 代数式节点不为0
     * @warning 这要求 root 节点被化简过
     */
    bool    isZero(const NodePtr &root) noexcept {
        return std::holds_alternative<Monomial>(root->m_value)
               && std::get<Monomial>(root->m_value).m_coeff == 0;
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
    bool isEqual(const NodePtr &lhs, const NodePtr &rhs) noexcept {
        return std::visit(makeOverloaded(
                              [&](const Monomial &value1, const Monomial &value2) -> bool {
                                  if (value1.m_coeff != value2.m_coeff) {
                                      return false;
                                  }
                                  return value1.m_var_exps == value2.m_var_exps;
                              },
                              [&]<typename T>(const T &value1, const T &value2) -> bool {
                                  if (value1.m_children.size() != value2.m_children.size()) {
                                      return false;
                                  }
                                  return std::ranges::all_of(
                                      std::views::zip(value1.m_children, value2.m_children),
                                      [](const auto &elem) {
                                          const auto &[ch1, ch2] = elem;
                                          return isEqual(ch1, ch2);
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
    bool hasVariable(const NodePtr &lhs, std::optional<VariableView> variable = std::nullopt) noexcept {
        return std::visit(makeOverloaded(
                              [&](const Monomial &value) {
                                  return variable.has_value() ? value.m_var_exps.contains(*variable)
                                                              : !value.m_var_exps.empty();
                              },
                              [&](const auto &value) {
                                  return std::ranges::any_of(value.m_children,
                                                             [&](const NodePtr &sub_node) {
                                                                 return hasVariable(sub_node, variable);
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
    std::string nodeToString(const NodePtr &root) {
        return std::visit(makeOverloaded(
                              [](const Monomial &value) {
                                  std::string str;
                                  str += std::format("{}", value.m_coeff);
                                  for (const auto &[var, exp] : value.m_var_exps) {
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
                              [](const Addition &value) {
                                  std::string str = "(";
                                  for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                                      if (i) {
                                          str += ")+(";
                                      }
                                      str += nodeToString(value.m_children[i]);
                                  }
                                  str += ")";
                                  return str;
                              },
                              [](const Multiplication &value) {
                                  std::string str = "(";
                                  for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                                      if (i) {
                                          str += ")*(";
                                      }
                                      str += nodeToString(value.m_children[i]);
                                  }
                                  str += ")";
                                  return str;
                              },
                              [](const Division &value) {
                                  return std::format("({})/({})", nodeToString(value.m_children[0]),
                                                     nodeToString(value.m_children[1]));
                              }),
                          root->m_value);
    }

    /**
     * @brief 简化代数式
     * @param [in, out] root 代数式
     */
    void simplify(NodePtr &root);

    /**
     * @brief 代数式节点简化器
     * @tparam SubNodeT 子节点
     */
    template <SubNode SubNodeT>
    struct Simplifier {
        /**
         * @brief 简化代数式节点
         * @param [in, out] root_val 子节点的引用
         * @param [in, out] root 节点的引用
         */
        void operator()(SubNodeT &root_val, NodePtr &root) noexcept;
    };

    template <>
    void Simplifier<Monomial>::operator()(Monomial &root_val, [[maybe_unused]] NodePtr &root) noexcept {
        if (root_val.m_coeff == 0) {
            // 节点为0，清空
            root_val.m_var_exps.clear();
            return;
        }
        for (auto iter = root_val.m_var_exps.begin(); iter != root_val.m_var_exps.end();) {
            IntegerConstantType &exp = iter->second;
            if (exp == 0) {
                // 删除多余无用变量
                iter = root_val.m_var_exps.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    template <>
    void Simplifier<Addition>::operator()(Addition &root_val, NodePtr &root) noexcept {
        // std::println("the raw addition node is {}", nodeToString(root));
        auto &children = root_val.m_children;
        if (children.empty()) {
            root = createZeroNode();
            return;
        }

        IntegerConstantType                       integer_augend = 0; //< 整型常量
        std::unordered_map<std::string, Monomial> monomials; //< Monomial 节点除去系数的字符串 -> 其值
        std::unordered_map<std::string, Division>
            divisions; //< Division 节点分母的字符串 -> 其分母和分子

        // 暂时不能将其改为range-for，因为在 Addition 中有对 children 的更改
        for (std::size_t i = 0; i < children.size(); ++i) {
            NodePtr cur = std::move(children[i]);
            // 获取元素
            assert_msg(cur != nullptr, "Addition 节点的子节点 cur 错误地为 nullptr");
            // 对每个子节点化简
            simplify(cur);

            std::visit(makeOverloaded(
                           [&](Addition &value) {
                               // 展开 Addition 节点
                               auto &sub_children = value.m_children;
                               children.append_range(sub_children | std::views::as_rvalue);
                           },
                           [&](Monomial &value) {
                               if (value.m_var_exps.empty()) {
                                   // 是常量
                                   integer_augend += value.m_coeff;
                                   return;
                               }
                               // 是变量
                               IntegerConstantType coeff = std::exchange(value.m_coeff, 1);
                               std::string         key = nodeToString(cur); //< 原始值的字符串
                               value.m_coeff = coeff;
                               auto [iter, succeeded] = monomials.try_emplace(key, std::move(value));
                               if (!succeeded) {
                                   // 已经存在，相加
                                   iter->second.m_coeff += coeff;
                               }
                           },
                           [&](Multiplication & /*unused*/) {
                               assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                           },
                           [&](Division &value) {
                               std::string key = nodeToString(value.m_children[1]);
                               auto        iter =
                                   divisions
                                       .try_emplace(key, Division{createZeroNode(),
                                                                  std::move(value.m_children[1])})
                                       .first;
                               iter->second.m_children[0] =
                                   makeNode<Addition>(std::move(value.m_children[0]),
                                                      std::move(iter->second.m_children[0]));
                           }),
                       cur->m_value);
        }
        children.clear();

        // 重建 Division 节点
        for (auto &[var, division] : divisions) {
            NodePtr new_child = makeNode<Division>(std::move(division));
            simplify(new_child);

            /// @todo 取消此处与上面重载的重复

            std::visit(
                makeOverloaded(
                    [&](Addition &value) {
                        // 展开 Addition 节点
                        auto &sub_children = value.m_children;
                        for (NodePtr &sub : sub_children) {
                            // sub一定是 Monomial
                            assert_msg(std::holds_alternative<Monomial>(sub->m_value),
                                       "猜想错误：sub 实际上不一定为 Monomial");
                            auto &sub_value = std::get<Monomial>(sub->m_value);
                            if (sub_value.m_var_exps.empty()) {
                                // 是常量
                                integer_augend += sub_value.m_coeff;
                                continue;
                            }
                            // 是变量
                            IntegerConstantType coeff = std::exchange(sub_value.m_coeff, 1);
                            std::string         key = nodeToString(sub); //< 原始值的字符串
                            sub_value.m_coeff = coeff;
                            auto [iter, succeeded] = monomials.try_emplace(key, std::move(sub_value));
                            if (!succeeded) {
                                // 已经存在，相加
                                iter->second.m_coeff += coeff;
                            }
                        }
                    },
                    [&](Monomial &value) {
                        if (value.m_var_exps.empty()) {
                            // 是常量
                            integer_augend += value.m_coeff;
                            return;
                        }
                        // 是变量
                        IntegerConstantType coeff = std::exchange(value.m_coeff, 1);
                        std::string         key = nodeToString(new_child); //< 原始值的字符串
                        value.m_coeff = coeff;
                        auto [iter, succeeded] = monomials.try_emplace(key, std::move(value));
                        if (!succeeded) {
                            // 已经存在，相加
                            iter->second.m_coeff += coeff;
                        }
                    },
                    [&](Multiplication & /*unused*/) {
                        assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                    },
                    [&](Division &value) {
                        // 合并到前面的节点或添加
                        auto merge_or_add = [&children, &integer_augend,
                                             &monomials](this auto &&merge_or_add, Division &root_value,
                                                         NodePtr &root) -> void {
                            std::string key = nodeToString(root_value.m_children[1]);
                            for (NodePtr &child : children) {
                                assert_msg(std::holds_alternative<Division>(child->m_value),
                                           "猜想错误：child 实际上不一定为 Division");
                                auto &child_value = std::get<Division>(child->m_value);
                                if (key != nodeToString(child_value.m_children[1])) {
                                    continue;
                                }
                                child_value.m_children[0] =
                                    makeNode<Addition>(std::move(child_value.m_children[0]),
                                                       std::move(root_value.m_children[0]));
                                simplify(child);
                                NodePtr new_child = std::move(child);
                                std::swap(child, children.back());
                                children.pop_back();
                                /// @todo 取消此处与上面重载的重复

                                // 合并相同分母
                                std::visit(
                                    makeOverloaded(
                                        [&](Addition &value) {
                                            // 展开 Addition 节点
                                            auto &sub_children = value.m_children;
                                            for (NodePtr &sub : sub_children) {
                                                // sub一定是 Monomial
                                                assert_msg(
                                                    std::holds_alternative<Monomial>(sub->m_value),
                                                    "猜想错误：sub 实际上不一定为 Monomial");
                                                auto &sub_value = std::get<Monomial>(sub->m_value);
                                                if (sub_value.m_var_exps.empty()) {
                                                    // 是常量
                                                    integer_augend += sub_value.m_coeff;
                                                    continue;
                                                }
                                                // 是变量
                                                IntegerConstantType coeff =
                                                    std::exchange(sub_value.m_coeff, 1);
                                                std::string key = nodeToString(sub); //< 原始值的字符串
                                                sub_value.m_coeff = coeff;
                                                auto [iter, succeeded] =
                                                    monomials.try_emplace(key, std::move(sub_value));
                                                if (!succeeded) {
                                                    // 已经存在，相加
                                                    iter->second.m_coeff += coeff;
                                                }
                                            }
                                        },
                                        [&](Monomial &value) {
                                            if (value.m_var_exps.empty()) {
                                                // 是常量
                                                integer_augend += value.m_coeff;
                                                return;
                                            }
                                            // 是变量
                                            IntegerConstantType coeff = std::exchange(value.m_coeff, 1);
                                            std::string key = nodeToString(child); //< 原始值的字符串
                                            value.m_coeff = coeff;
                                            auto [iter, succeeded] =
                                                monomials.try_emplace(key, std::move(value));
                                            if (!succeeded) {
                                                // 已经存在，相加
                                                iter->second.m_coeff += coeff;
                                            }
                                        },
                                        [&](Multiplication & /*unused*/) {
                                            assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                                        },
                                        [&](Division &value) {
                                            // 继续递归，防止化简后还有相同分母的其他节点
                                            merge_or_add(value, new_child);
                                        }),
                                    new_child->m_value);
                                return;
                            }
                            // 没有相同分母
                            children.push_back(std::move(root));
                        };
                        merge_or_add(value, new_child);
                    }),
                new_child->m_value);
        }

        // 重建 Monomial 节点
        for (auto &[var, monomial] : monomials) {
            if (monomial.m_coeff == 0) {
                continue;
            }
            children.push_back(makeNode<Monomial>(std::move(monomial)));
        }

        // 重建常量
        if (integer_augend == 0) {
            // 若只有一个或没有子节点，取消 Addition 节点
            if (children.size() == 1) {
                NodePtr tmp = std::move(children[0]);
                root = std::move(tmp);
            } else if (children.empty()) {
                root = createZeroNode();
            }
            return;
        }
        children.push_back(makeNode<Monomial>(integer_augend));
        std::swap(children.front(), children.back());
        // 若只有一个子节点，取消 Addition 节点
        if (children.size() == 1) {
            NodePtr tmp = std::move(children.front());
            root = std::move(tmp);
        }
        // std::println("the simpilfied addition node is {}", nodeToString(root));
    }

    template <>
    void Simplifier<Multiplication>::operator()(Multiplication &root_val, NodePtr &root) noexcept {
        // std::println("the raw multiplication node is {}", nodeToString(root));
        auto &children = root_val.m_children;
        if (children.empty()) {
            root = createZeroNode();
            return;
        }

        Monomial new_node{.m_coeff = 1};
        for (auto iter = children.begin(); iter != children.end(); ++iter) {
            NodePtr cur = std::move(*iter);
            simplify(cur);
            bool finished = false; //< 是否结束
            std::visit(makeOverloaded(
                           [&](Monomial &value) {
                               // 合并
                               new_node.m_coeff *= value.m_coeff;
                               new_node.m_var_exps.merge(value.m_var_exps);
                               for (auto &[var, exp] : value.m_var_exps) {
                                   new_node.m_var_exps[var] += exp;
                               }
                           },
                           [&](Multiplication & /*unused*/) {
                               assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                           },
                           [&](Addition &value) {
                               // 乘进去
                               Multiplication rest{
                                   makeNode<Monomial>(std::move(new_node))}; //< 目前剩余的节点
                               ++iter;
                               for (; iter != children.end(); ++iter) {
                                   rest.m_children.push_back(std::move(*iter));
                               }
                               for (NodePtr &sub_ch : value.m_children) {
                                   Multiplication cpy = rest;
                                   cpy.m_children.push_back(std::move(sub_ch));
                                   sub_ch = makeNode<Multiplication>(std::move(cpy));
                               }
                               root = makeNode<Addition>(std::move(value));
                               simplify(root);
                               finished = true;
                           },
                           [&](Division &value) {
                               // 乘进去
                               Multiplication rest{makeNode<Monomial>(std::move(new_node)),
                                                   std::move(value.m_children[0])}; //< 目前剩余的节点
                               ++iter;
                               for (; iter != children.end(); ++iter) {
                                   rest.m_children.push_back(std::move(*iter));
                               }
                               value.m_children[0] = makeNode<Multiplication>(rest);
                               root = makeNode<Division>(std::move(value));
                               simplify(root);
                               finished = true;
                           }),
                       cur->m_value);
            if (finished) {
                return;
            }
            if (new_node.m_coeff == 0) {
                // 为0，退出
                root = createZeroNode();
                return;
            }
        }

        root = makeNode<Monomial>(std::move(new_node));
        simplify(root);
        // std::println("the simpilfied multiplication node is {}", nodeToString(root));
    }

    using Polynomial = std::vector<NodePtr>; //< 多项式别名，索引 = 指数

    /**
     * @brief 去除多项式的多余节点
     * @param [in] poly 多项式
     */
    void removeRedundantPoly(Polynomial &poly) {
        while (!poly.empty() && isZero(poly.back())) { poly.pop_back(); }
    };

    /**
     * @brief 将普通代数式节点转换成为按主元指数排列的系数多项式向量
     * @param [in] root 代数式节点
     * @param [in] main_var 主元
     * @return Polynomial 多项式向量
     * @warning 这要求 root 节点被化简过
     */
    Polynomial toPoly(NodePtr root, VariableView main_var) {
        return std::visit(
            makeOverloaded(
                [&](Monomial &value) {
                    auto                iter = value.m_var_exps.find(main_var);
                    IntegerConstantType exp = (iter == value.m_var_exps.end()) ? 0 : iter->second;
                    Polynomial          poly;
                    poly.reserve(exp + 1);
                    std::ranges::generate_n(std::back_inserter(poly), exp + 1, createZeroNode);
                    if (iter != value.m_var_exps.end()) {
                        value.m_var_exps.erase(iter);
                    }
                    poly[exp] = std::move(root);
                    return poly;
                },
                [&](Addition &value) {
                    Polynomial poly;
                    for (NodePtr &child : value.m_children) {
                        auto               &value = std::get<Monomial>(child->m_value);
                        auto                iter = value.m_var_exps.find(main_var);
                        IntegerConstantType exp = (iter == value.m_var_exps.end()) ? 0 : iter->second;

                        // 重新调整大小
                        if (std::size_t old_size = poly.size(), new_size = exp + 1;
                            old_size < new_size) {
                            poly.reserve(new_size);
                            std::ranges::generate_n(std::back_inserter(poly),
                                                    static_cast<long long>(new_size - old_size),
                                                    createZeroNode);
                        }

                        if (iter != value.m_var_exps.end()) {
                            value.m_var_exps.erase(iter);
                        }
                        poly[exp] = makeNode<Addition>(std::move(poly[exp]), std::move(child));
                        simplify(poly[exp]);
                    }
                    removeRedundantPoly(poly);
                    return poly;
                },
                [&](Multiplication & /*unused*/) {
                    assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                    return Polynomial{};
                },
                [&](Division & /*unused*/) {
                    assert_msg(false, "多项式节点中不应有 Division 节点");
                    return Polynomial{};
                }),
            root->m_value);
    };

    /**
     * @brief 将按主元指数排列的系数多项式向量转换成为普通代数式节点
     * @param [in] poly 多项式向量
     * @param [in] main_var 主元
     * @return NodePtr 代数式节点
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    NodePtr toNodePtr(Polynomial poly, VariableView main_var) {
        NodePtr ret = createZeroNode();
        for (const auto [exp, child] : std::views::enumerate(poly)) {
            ret = makeNode<Addition>(
                std::move(ret),
                makeNode<Multiplication>(std::move(child),
                                         makeNode<Monomial>(1, Monomial::MapVarExp{{main_var, exp}})));
        }
        simplify(ret);
        return ret;
    };

    /**
     * @brief 获取多项式的最高次数
     * @param [in] poly 多项式
     * @return std::size_t 最高次数
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    std::size_t getDegreePoly(const Polynomial &poly) { return poly.empty() ? 0 : poly.size() - 1; };

    /**
     * @brief 获取多项式的首项系数的副本
     * @param [in] poly 多项式
     * @return NodePtr 多项式的首项系数的副本
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    NodePtr     getLeadingCoefficientPoly(const Polynomial &poly) {
        return std::make_unique<Node>(*poly.back());
    };

    /**
     * @brief 克隆多项式
     * @param [in] poly 多项式
     * @return Polynomial 多项式副本
     */
    Polynomial clonePoly(const Polynomial &poly) {
        Polynomial ret;
        ret.reserve(poly.size());
        for (const auto &child : poly) { ret.push_back(std::make_unique<Node>(*child)); }
        return ret;
    };

    /**
     * @brief 将多项式乘上主元的 offset 次幂
     * @param [in] poly 多项式
     * @param [in] offset 幂数，或者是向高次的移动次数
     * @return Polynomial 更改后的多项式
     */
    Polynomial shiftPoly(Polynomial poly, std::size_t offset) {
        Polynomial ret;
        ret.reserve(offset);
        std::ranges::generate_n(std::back_inserter(ret), static_cast<long long>(offset), createZeroNode);
        ret.append_range(poly | std::views::as_rvalue);
        return ret;
    };

    /**
     * @brief 判断多项式是否为0
     * @param [in] poly 多项式
     * @return true 为0 
     * @return false 不为0
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    bool       isZeroPoly(const Polynomial &poly) { return poly.empty(); };

    /**
     * @brief 将两个多项式相加
     * @param [in] addend1 加数1
     * @param [in] addend2 加数2
     * @return Polynomial 相加结果
     */
    Polynomial addPoly(Polynomial addend1, Polynomial addend2) {
        std::size_t max_size = std::max(addend1.size(), addend2.size());
        Polynomial  ret;
        ret.reserve(max_size);
        for (std::size_t i = 0; i < max_size; ++i) {
            NodePtr addend1_i = i < addend1.size() ? std::move(addend1[i]) : createZeroNode();
            NodePtr addend2_i = i < addend2.size() ? std::move(addend2[i]) : createZeroNode();
            // ret[i] = a + b
            ret.push_back(makeNode<Addition>(std::move(addend1_i), std::move(addend2_i)));
            simplify(ret[i]);
        }
        removeRedundantPoly(ret);
        return ret;
    };

    /**
     * @brief 将两个多项式相减
     * @param [in] minuend 被减数
     * @param [in] subtrahend 减数
     * @return Polynomial 相减结果
     */
    Polynomial subPoly(Polynomial minuend, Polynomial subtrahend) {
        std::size_t max_size = std::max(minuend.size(), subtrahend.size());
        Polynomial  ret;
        ret.reserve(max_size);
        for (std::size_t i = 0; i < max_size; ++i) {
            NodePtr minuend_i = i < minuend.size() ? std::move(minuend[i]) : createZeroNode();
            NodePtr subtrahend_i = i < subtrahend.size() ? std::move(subtrahend[i]) : createZeroNode();
            // ret[i] = a - b  => a + (-1) * b
            ret.push_back(makeNode<Addition>(
                std::move(minuend_i),
                makeNode<Multiplication>(std::move(subtrahend_i), makeNode<Monomial>(-1))));
            simplify(ret[i]);
        }
        removeRedundantPoly(ret);
        return ret;
    };

    /**
     * @brief 将两个多项式相乘
     * @param [in] multiplier1 乘数1
     * @param [in] multiplier2 乘数2
     * @return Polynomial 相乘结果
     */
    Polynomial mulPoly(const Polynomial &multiplier1, const Polynomial &multiplier2) {
        Polynomial ret;
        ret.reserve(multiplier1.size() + multiplier2.size());
        std::ranges::generate_n(std::back_inserter(ret),
                                static_cast<long long>(multiplier1.size() + multiplier2.size()),
                                createZeroNode);

        for (std::size_t i = 0; i < multiplier1.size(); ++i) {
            for (std::size_t j = 0; j < multiplier2.size(); ++j) {
                ret[i + j] = makeNode<Addition>(
                    std::move(ret[i + j]),
                    makeNode<Multiplication>(std::make_unique<Node>(*multiplier1[i]),
                                             std::make_unique<Node>(*multiplier2[j])));
            }
        }

        for (NodePtr &child : ret) { simplify(child); }
        removeRedundantPoly(ret);
        return ret;
    };

    /**
     * @brief 将两个多项式相除
     * @param [in] dividend 被除数
     * @param [in] divisor 除数
     * @return Polynomial 相除结果
     * @warning 要求多项式必须整除
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    Polynomial divPoly(Polynomial dividend, const Polynomial &divisor) {
        NodePtr    lc_divisor = getLeadingCoefficientPoly(divisor);
        Polynomial quotient;
        while (!isZeroPoly(dividend)) {
            assert_msg(getDegreePoly(dividend) >= getDegreePoly(divisor),
                       "dividend 不能被 divisor 整除");
            NodePtr lc_dividend = getLeadingCoefficientPoly(dividend);
            NodePtr lc_div_node =
                makeNode<Division>(std::move(lc_dividend), std::make_unique<Node>(*lc_divisor));
            simplify(lc_div_node);
            Polynomial lc_div;
            lc_div.push_back(std::move(lc_div_node));
            // multiplier = lc(dividend) / lc(divisor) * (x ^ (deg(dividend) - deg(divisor)))
            Polynomial multiplier =
                shiftPoly(std::move(lc_div), getDegreePoly(dividend) - getDegreePoly(divisor));
            Polynomial subtrahend = mulPoly(divisor, multiplier);
            // quotient += multiplier
            quotient = addPoly(std::move(quotient), std::move(multiplier));
            // dividend = dividend - divisor * multiplier
            dividend = subPoly(std::move(dividend), std::move(subtrahend));
        }
        removeRedundantPoly(quotient);
        return quotient;
    };

    /**
     * @brief 运用伪余数定理，计算伪余式
     * @param [in] dividend 被除数
     * @param [in] divisor 除数
     * @return Polynomial 伪余式
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    Polynomial getPseudoRemainderPoly(Polynomial dividend, const Polynomial &divisor) {
        if (getDegreePoly(dividend) < getDegreePoly(divisor)) {
            return dividend;
        }
        Polynomial lc_divisor;
        lc_divisor.push_back(getLeadingCoefficientPoly(divisor));
        Polynomial remainder = std::move(dividend);
        // Polynomial quotient;
        while (getDegreePoly(remainder) >= getDegreePoly(divisor)) {
            // 算法原理：保持 lc_divisor^i(循环次数) * dividend(初始) = quotient(注释部分) * divisor + remainder
            auto       exp = getDegreePoly(remainder) - getDegreePoly(divisor);
            Polynomial lc_dividend;
            lc_dividend.push_back(getLeadingCoefficientPoly(remainder));
            // term1 = lc(divisor) * dividend
            Polynomial term1 = mulPoly(remainder, lc_divisor);
            // term2 = lc(dividend) * x^exp * divisor
            Polynomial q_term = shiftPoly(std::move(lc_dividend), exp);
            Polynomial term2 = mulPoly(divisor, q_term);
            // quotient = addPoly(poly_mul(divisor, quotient), std::move(q_term));
            remainder = subPoly(std::move(term1), std::move(term2));
        }
        return remainder;
    };

    NodePtr polynomialGcd(NodePtr root1, NodePtr root2);

    /**
     * @brief 获取多项式的内容部分
     * @param [in] poly 多项式
     * @return NodePtr 多项式的内容部分
     * @warning 需要提前调用 removeRedundantPoly 函数
     */
    NodePtr getContentPoly(Polynomial poly) {
        std::optional<NodePtr> gcd_result;
        for (auto &child : poly) {
            gcd_result = gcd_result
                             .transform([&](NodePtr &result) {
                                 return polynomialGcd(std::move(result), std::move(child));
                             })
                             .or_else([&]() -> std::optional<NodePtr> { return std::move(child); });
        }
        return std::move(gcd_result).value_or(createZeroNode());
    };

    /**
     * @brief 计算多项式的最大公因式
     * @param [in] poly1 多项式1
     * @param [in] poly2 多项式2
     * @return Polynomial 最大公因式
     * @warning 需要提前调用 removeRedundantPoly 函数
     * @details
     *  - 采用内容-本原部分分解法
     */
    Polynomial polynomialGcd(Polynomial poly1, Polynomial poly2) {
        // 特殊情况
        if (isZeroPoly(poly1)) {
            return poly2;
        }
        if (isZeroPoly(poly2)) {
            return poly1;
        }

        // 本原部分的 GCD（运用辗转相除法），需要提前调用 removeRedundantPoly 函数
        auto gcd_primitive = [](this auto &&gcd_primitive, Polynomial poly1,
                                Polynomial poly2) -> Polynomial {
            // 保证 degree(poly1) >= degree(poly2)
            if (getDegreePoly(poly1) < getDegreePoly(poly2)) {
                std::swap(poly1, poly2);
            }

            // 递归终止条件：poly2 为0
            if (isZeroPoly(poly2)) {
                return poly1;
            }

            // 递归终止条件：poly2 为常数
            if (getDegreePoly(poly2) == 0) {
                // 此时 poly1 不可能为常数
                assert_msg(getDegreePoly(poly1) > 0, "猜想错误：poly1 实际上可能为常数");
                // 返回 1
                Polynomial ret;
                ret.push_back(makeNode<Monomial>(1));
                return ret;
            }

            // 计算伪余数
            Polynomial remainder = getPseudoRemainderPoly(std::move(poly1), poly2);

            // 计算 remiander 的本原部分
            Polynomial r_content;
            r_content.push_back(getContentPoly(clonePoly(remainder)));
            Polynomial r_prim = divPoly(std::move(remainder), r_content);

            return gcd_primitive(std::move(poly2), std::move(r_prim));
        };

        // 计算内容和本原部分
        Polynomial content1;
        content1.push_back(getContentPoly(clonePoly(poly1)));
        Polynomial prim1 = divPoly(std::move(poly1), content1);
        Polynomial content2;
        content2.push_back(getContentPoly(clonePoly(poly2)));
        Polynomial prim2 = divPoly(std::move(poly2), content2);
        // std::println("gcd: divided");

        // 计算内容部分的 GCD
        Polynomial gcd_content;
        gcd_content.push_back(polynomialGcd(std::move(content1[0]), std::move(content2[0])));
        // std::println("gcd: computed content");

        // 计算本原部分的 GCD
        Polynomial gcd_prim = gcd_primitive(std::move(prim1), std::move(prim2));
        // std::println("gcd: computed primitive");

        // 合并
        Polynomial result = mulPoly(gcd_prim, gcd_content);
        // std::println("gcd: merged");

        return result;
    }

    /**
     * @brief 获取两个代数式公共的变量
     * @param [in] root1 代数式节点1
     * @param [in] root2 代数式节点2
     * @return std::optional<VariableView> 公共的变量，若没有则为0
     * @warning 这要求 root 节点被化简过
     */
    std::optional<VariableView> getCommonVar(const NodePtr &root1, const NodePtr &root2) {
        std::optional<VariableView> main_var; //< 主元
        std::set<VariableView>      vars;     //< 变量集
        std::visit(makeOverloaded(
                       [&](const Monomial &value) {
                           for (const auto &[var, exp] : value.m_var_exps) { vars.insert(var); }
                       },
                       [&](const Addition &value) {
                           for (const NodePtr &child : value.m_children) {
                               const auto &value = std::get<Monomial>(child->m_value);
                               for (const auto &[var, exp] : value.m_var_exps) { vars.insert(var); }
                           }
                       },
                       [&](const Multiplication & /*unused*/) {
                           assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                       },
                       [&](const Division & /*unused*/) {
                           assert_msg(false, "多项式节点中不应有 Division 节点");
                       }),
                   root1->m_value);

        std::visit(makeOverloaded(
                       [&](const Monomial &value) {
                           for (const auto &[var, exp] : value.m_var_exps) {
                               if (vars.contains(var)) {
                                   main_var = var;
                                   return;
                               }
                           }
                       },
                       [&](const Addition &value) {
                           for (const NodePtr &child : value.m_children) {
                               const auto &value = std::get<Monomial>(child->m_value);
                               for (const auto &[var, exp] : value.m_var_exps) {
                                   main_var = var;
                                   return;
                               }
                           }
                       },
                       [&](const Multiplication & /*unused*/) {
                           assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                       },
                       [&](const Division & /*unused*/) {
                           assert_msg(false, "多项式节点中不应有 Division 节点");
                       }),
                   root2->m_value);
        return main_var;
    }

    /**
     * @brief 提取代数式节点的整数因数
     * @param [in, out] root 代数式节点
     * @return IntegerConstantType 提取的整数因数
     * @warning 这要求 root 节点被化简过
     * @warning 这要求 root 节点是多项式
     * @note 代数式节点将不会除去提取的因数
     */
    IntegerConstantType extractPolynomialIntegerCoefficient(const NodePtr &root) {
        return std::visit(
            makeOverloaded([&](const Monomial &value) { return std::abs(value.m_coeff); },
                           [&](const Addition &value) {
                               std::optional<IntegerConstantType> coeff = std::nullopt;
                               for (const NodePtr &child : value.m_children) {
                                   assert_msg(std::holds_alternative<Monomial>(child->m_value),
                                              "猜想错误：child 实际上不一定为 Monomial");
                                   auto &mono = std::get<Monomial>(child->m_value);
                                   coeff = coeff
                                               .transform([&](IntegerConstantType nested_coeff) {
                                                   return std::gcd(nested_coeff, mono.m_coeff);
                                               })
                                               .or_else([&] { return std::optional{mono.m_coeff}; });
                               }
                               return coeff.value_or(1);
                           },
                           [&](const Multiplication & /*unused*/) -> IntegerConstantType {
                               assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                               return 0;
                           },
                           [&](const Division & /*unused*/) -> IntegerConstantType {
                               assert_msg(false, "多项式节点中不应有 Division 节点");
                               return 0;
                           }),
            root->m_value);
    }

    /**
     * @brief 将代数式节点除掉整数因数
     * @param [in] root 代数式节点
     * @param [in] coeff 整数因数
     * @warning 这要求 root 节点被化简过
     * @warning 这要求 root 节点是多项式
     * @warning 这要求 root 节点能整除 coeff
     */
    void dividePolynomialWithIntegerCoefficient(NodePtr &root, IntegerConstantType coeff) {
        std::visit(makeOverloaded([&](Monomial &value) { value.m_coeff /= coeff; },
                                  [&](Addition &value) {
                                      for (const NodePtr &child : value.m_children) {
                                          assert_msg(std::holds_alternative<Monomial>(child->m_value),
                                                     "猜想错误：child 实际上不一定为 Monomial");
                                          auto &mono = std::get<Monomial>(child->m_value);
                                          mono.m_coeff /= coeff;
                                      }
                                  },
                                  [&](Multiplication & /*unused*/) {
                                      assert_msg(false, "化简后的节点不应为 Multiplication 节点");
                                  },
                                  [&](Division & /*unused*/) {
                                      assert_msg(false, "多项式节点中不应有 Division 节点");
                                  }),
                   root->m_value);
    }

    /**
      @brief 计算代数式节点（需要为多项式）的最大公因式
     * @param [in] root1 代数式节点1
     * @param [in] root2 代数式节点2
     * @return NodePtr 最大公因式
     * @warning 这要求 root 节点被化简过
     */
    NodePtr polynomialGcd(NodePtr root1, NodePtr root2) {
        // 特殊情况
        if (isZero(root1)) {
            return root2;
        }
        if (isZero(root2)) {
            return root1;
        }

        // 选择主元
        std::optional<VariableView> main_var = getCommonVar(root1, root2); //< 主元

        return main_var
            .transform([&](VariableView nested_main_var) {
                // 转换
                auto       poly1 = toPoly(std::move(root1), nested_main_var);
                auto       poly2 = toPoly(std::move(root2), nested_main_var);

                // 计算
                Polynomial result = polynomialGcd(std::move(poly1), std::move(poly2));

                // 展平
                return toNodePtr(std::move(result), nested_main_var);
            })
            .or_else([&] {
                // 没有相同的变量，返回
                return std::optional{
                    makeNode<Monomial>(std::gcd(extractPolynomialIntegerCoefficient(root1),
                                                extractPolynomialIntegerCoefficient(root2)))};
            })
            .value();
    }

    template <>
    void Simplifier<Division>::operator()(Division &root_val, NodePtr &root) noexcept {
        // std::println("the raw division node is {}", nodeToString(root));
        NodePtr &num = root_val.m_children[0];
        NodePtr &den = root_val.m_children[1];
        simplify(num);
        simplify(den);

        // 保证为多项式
        if (std::holds_alternative<Division>(num->m_value)) {
            auto &value = std::get<Division>(num->m_value);
            den = makeNode<Multiplication>(std::move(den), std::move(value.m_children[1]));
            NodePtr tmp = std::move(value.m_children[0]);
            num = std::move(tmp);
            simplify(root);
            return;
        }
        if (std::holds_alternative<Division>(den->m_value)) {
            auto &value = std::get<Division>(den->m_value);
            num = makeNode<Multiplication>(std::move(den), std::move(value.m_children[1]));
            NodePtr tmp = std::move(value.m_children[0]);
            den = std::move(tmp);
            simplify(root);
            return;
        }
        if (std::holds_alternative<Addition>(num->m_value)) {
            auto &value = std::get<Addition>(num->m_value);
            for (auto &child : value.m_children) {
                // 避免子节点非单项式
                if (std::holds_alternative<Division>(child->m_value)) {
                    auto   &child_value = std::get<Division>(child->m_value);
                    NodePtr sub_den = std::move(child_value.m_children[1]);
                    NodePtr sub_num = std::move(child_value.m_children[0]);
                    std::swap(value.m_children.back(), child);
                    value.m_children.pop_back();
                    num = makeNode<Addition>(
                        makeNode<Multiplication>(std::move(num), std::make_unique<Node>(*sub_den)),
                        std::move(sub_num));
                    den = makeNode<Multiplication>(std::move(den), std::move(sub_den));
                    simplify(root);
                    return;
                }
            }
        }
        if (std::holds_alternative<Addition>(den->m_value)) {
            auto &value = std::get<Addition>(den->m_value);
            for (auto &child : value.m_children) {
                // 避免子节点非单项式
                if (std::holds_alternative<Division>(child->m_value)) {
                    auto   &child_value = std::get<Division>(child->m_value);
                    NodePtr sub_den = std::move(child_value.m_children[1]);
                    NodePtr sub_num = std::move(child_value.m_children[0]);
                    std::swap(value.m_children.back(), child);
                    value.m_children.pop_back();
                    den = makeNode<Addition>(
                        makeNode<Multiplication>(std::move(den), std::make_unique<Node>(*sub_den)),
                        std::move(sub_num));
                    num = makeNode<Multiplication>(std::move(num), std::move(sub_den));
                    simplify(root);
                    return;
                }
            }
        }

        // 特殊情况
        if (isZero(num)) {
            root = createZeroNode();
            return;
        }

        // 摘自 polynomialGcd 并修改

        // 选择主元
        std::optional<VariableView> main_var = getCommonVar(num, den); //< 主元

        main_var
            .transform([&](VariableView nested_main_var) {
                // 转换
                auto       poly1 = toPoly(std::move(num), nested_main_var);
                auto       poly2 = toPoly(std::move(den), nested_main_var);
                // std::println("converted");

                // 计算
                Polynomial result = polynomialGcd(clonePoly(poly1), clonePoly(poly2));
                // std::println("computed");

                poly1 = divPoly(std::move(poly1), result);
                poly2 = divPoly(std::move(poly2), result);
                // std::println("divided");

                // 展平
                num = toNodePtr(std::move(poly1), nested_main_var);
                den = toNodePtr(std::move(poly2), nested_main_var);
                // std::println("reconverted");

                return nested_main_var;
            })
            .or_else([&] -> std::optional<VariableView> {
                // 没有相同的变量，除掉共同变量
                IntegerConstantType gcdans = std::gcd(extractPolynomialIntegerCoefficient(num),
                                                      extractPolynomialIntegerCoefficient(den));
                dividePolynomialWithIntegerCoefficient(num, gcdans);
                dividePolynomialWithIntegerCoefficient(den, gcdans);
                return {};
            });

        if (std::holds_alternative<Monomial>(den->m_value)) {
            auto &den_value = std::get<Monomial>(den->m_value);
            // 消除分母的负数
            if (den_value.m_coeff < 0) {
                den_value.m_coeff = -den_value.m_coeff;
                num = makeNode<Multiplication>(makeNode<Monomial>(-1), std::move(num));
                simplify(num);
            }
            // 消除无意义的分母
            if (den_value.m_coeff == 1 && den_value.m_var_exps.empty()) {
                NodePtr tmp = std::move(num);
                root = std::move(tmp);
            }
        }

        // std::println("the simpilfied division node is {}", nodeToString(root));
    }

    void simplify(NodePtr &root) {
        std::visit(
            [&](auto &value) {
                using T = std::decay_t<decltype(value)>;
                Simplifier<T>()(value, root);
            },
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
    FloatT calculateApproximation(const NodePtr                             &root,
                                  const std::function<FloatT(VariableView)> &converter) {
        return std::visit(makeOverloaded(
                              [&](const Monomial &value) {
                                  auto ret = static_cast<FloatT>(value.m_coeff);
                                  for (const auto &[var, exp] : value.m_var_exps) {
                                      ret *= std::pow(converter(var), exp);
                                  }
                                  return ret;
                              },
                              [&](const Addition &value) {
                                  FloatT sum = 0;
                                  for (const auto &child : value.m_children) {
                                      sum += calculateApproximation<FloatT>(child, converter);
                                  }
                                  return sum;
                              },
                              [&](const Multiplication &value) {
                                  FloatT mul = 1;
                                  for (const auto &child : value.m_children) {
                                      mul *= calculateApproximation<FloatT>(child, converter);
                                  }
                                  return mul;
                              },
                              [&](const Division &value) {
                                  return calculateApproximation<FloatT>(value.m_children[0], converter)
                                         / calculateApproximation<FloatT>(value.m_children[1],
                                                                          converter);
                              }),
                          root->m_value);
    }

    /**
     * @brief 代数式加/减法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 常量
     */
    void plus(NodePtr &lhs, IntegerConstantType rhs) {
        lhs = makeNode<Addition>(std::move(lhs), makeNode<Monomial>(rhs));
        simplify(lhs);
    }
    /**
     * @brief 代数式加法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void plus(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Addition>(std::move(lhs), makeNode<Monomial>(1, Monomial::MapVarExp{{rhs, 1}}));
        simplify(lhs);
    }
    /**
     * @brief 代数式减法内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void minus(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Addition>(std::move(lhs), makeNode<Monomial>(-1, Monomial::MapVarExp{{rhs, 1}}));
        simplify(lhs);
    }
    /**
     * @brief 代数式乘法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 常量
     */
    void multiply(NodePtr &lhs, IntegerConstantType rhs) {
        lhs = makeNode<Multiplication>(std::move(lhs), makeNode<Monomial>(rhs));
        simplify(lhs);
    }
    /**
     * @brief 代数式乘法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void multiply(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Multiplication>(std::move(lhs),
                                       makeNode<Monomial>(1, Monomial::MapVarExp{{rhs, 1}}));
        simplify(lhs);
    }
    /**
     * @brief 代数式除法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 常量
     */
    void divide(NodePtr &lhs, IntegerConstantType rhs) {
        lhs = makeNode<Division>(std::move(lhs), makeNode<Monomial>(rhs));
        simplify(lhs);
    }
    /**
     * @brief 代数式除法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void divide(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Division>(std::move(lhs), makeNode<Monomial>(1, Monomial::MapVarExp{{rhs, 1}}));
        simplify(lhs);
    }

} // namespace tnrw::math::details

/// @endcond

namespace tnrw {
    namespace math {

        // AlgebraicExpression类的成员定义
        [[nodiscard]] AlgebraicExpression::AlgebraicExpression() noexcept
            : m_root(details::createZeroNode()) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(IntegerConstantType constant) noexcept
            : m_root(details::makeNode<details::Monomial>(constant)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(VariableView variable) noexcept
            : m_root(details::makeNode<details::Monomial>(
                  1, details::Monomial::MapVarExp{{*m_vars.emplace(variable).first, 1}})) {}
        AlgebraicExpression::AlgebraicExpression(NumericExpression num_expr) noexcept
            : m_root(std::move(num_expr.m_root)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(const AlgebraicExpression &rhs) noexcept
            : m_root(std::make_unique<details::Node>(*rhs.m_root)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(AlgebraicExpression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

        AlgebraicExpression &AlgebraicExpression::operator=(const IntegerConstantType &rhs) & noexcept {
            m_root = details::makeNode<details::Monomial>(rhs);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator=(VariableView rhs) & noexcept {
            VariableView var = *m_vars.emplace(rhs).first;
            m_root = details::makeNode<details::Monomial>(1, details::Monomial::MapVarExp{{var, 1}});
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator=(const AlgebraicExpression &rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::make_unique<details::Node>(*rhs.m_root);
            }
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator=(AlgebraicExpression &&rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::move(rhs.m_root);
            }
            return *this;
        }

        AlgebraicExpression::~AlgebraicExpression() noexcept = default;

        AlgebraicExpression &AlgebraicExpression::operator+=(IntegerConstantType rhs) & noexcept {
            details::plus(m_root, rhs);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator+=(VariableView rhs) & noexcept {
            VariableView var = *m_vars.emplace(rhs).first;
            details::plus(m_root, var);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator+=(AlgebraicExpression rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Addition>(std::move(m_root), std::move(rhs.m_root));
            simplify(m_root);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator-=(IntegerConstantType rhs) & noexcept {
            details::plus(m_root, -rhs);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator-=(VariableView rhs) & noexcept {
            VariableView var = *m_vars.emplace(rhs).first;
            details::minus(m_root, var);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator-=(AlgebraicExpression rhs) & noexcept {
            using namespace details;
            m_root =
                makeNode<Addition>(std::move(m_root), makeNode<Multiplication>(makeNode<Monomial>(-1),
                                                                               std::move(rhs.m_root)));
            simplify(m_root);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator*=(IntegerConstantType rhs) & noexcept {
            details::multiply(m_root, rhs);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator*=(VariableView rhs) & noexcept {
            VariableView var = *m_vars.emplace(rhs).first;
            details::multiply(m_root, var);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator*=(AlgebraicExpression rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Multiplication>(std::move(m_root), std::move(rhs.m_root));
            simplify(m_root);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator/=(IntegerConstantType rhs) & noexcept {
            details::divide(m_root, rhs);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator/=(VariableView rhs) & noexcept {
            VariableView var = *m_vars.emplace(rhs).first;
            details::divide(m_root, var);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator/=(AlgebraicExpression rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Division>(std::move(m_root), std::move(rhs.m_root));
            simplify(m_root);
            return *this;
        }

        [[nodiscard]] AlgebraicExpression AlgebraicExpression::operator+() const noexcept {
            return *this;
        }

        [[nodiscard]] AlgebraicExpression AlgebraicExpression::operator-() const noexcept {
            AlgebraicExpression copy_of_this(*this);
            copy_of_this.changeToOpposite();
            return copy_of_this;
        }

        AlgebraicExpression &AlgebraicExpression::operator++() noexcept {
            details::plus(m_root, 1);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator--() noexcept {
            details::plus(m_root, -1);
            return *this;
        }

        [[nodiscard]] AlgebraicExpression AlgebraicExpression::operator++(int) noexcept {
            AlgebraicExpression copy_of_old(*this);
            details::plus(m_root, 1);
            return copy_of_old;
        }
        [[nodiscard]] AlgebraicExpression AlgebraicExpression::operator--(int) noexcept {
            AlgebraicExpression copy_of_old(*this);
            details::plus(m_root, -1);
            return copy_of_old;
        }

        template <std::floating_point FloatT>
        [[nodiscard]] FloatT AlgebraicExpression::calculateApproximation(
            const std::function<FloatT(VariableView)> &converter) const noexcept {
            return details::calculateApproximation<FloatT>(m_root, converter);
        }

        void AlgebraicExpression::clear() noexcept { m_root = details::createZeroNode(); }

        [[nodiscard]] std::string AlgebraicExpression::toString() const noexcept {
            return details::nodeToString(m_root);
        }

        [[nodiscard]] std::wstring AlgebraicExpression::toWString() const noexcept {
            return std::format(L"{}", *this);
        }

        void AlgebraicExpression::changeToOpposite() noexcept { details::multiply(m_root, -1); }

        [[nodiscard]] bool AlgebraicExpression::hasVariable() const noexcept {
            if (m_vars.empty()) {
                return false;
            }
            return details::hasVariable(m_root);
        }

        [[nodiscard]] bool AlgebraicExpression::hasVariable(VariableView variable) const noexcept {
            if (!m_vars.contains(variable)) {
                return false;
            }
            return details::hasVariable(m_root, variable);
        }
        [[nodiscard]] std::optional<NumericExpression>
        AlgebraicExpression::toNumericExpression() const & noexcept {
            if (hasVariable()) {
                return std::nullopt;
            }
            NumericExpression ret;
            ret.m_root = std::make_unique<details::Node>(*m_root);
            return ret;
        }
        [[nodiscard]] std::optional<NumericExpression>
        AlgebraicExpression::toNumericExpression() && noexcept {
            if (hasVariable()) {
                return std::nullopt;
            }
            NumericExpression ret;
            ret.m_root = std::move(m_root);
            return ret;
        }

        [[nodiscard]] AlgebraicExpression operator+(const AlgebraicExpression &lhs,
                                                    const IntegerConstantType  rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy += rhs;
        }
        [[nodiscard]] AlgebraicExpression operator+(const IntegerConstantType  lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy += lhs;
        }
        [[nodiscard]] AlgebraicExpression operator+(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy += rhs;
        }
        [[nodiscard]] AlgebraicExpression operator+(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy += lhs;
        }
        [[nodiscard]] AlgebraicExpression operator+(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy += rhs;
        }

        [[nodiscard]] AlgebraicExpression operator-(const AlgebraicExpression &lhs,
                                                    const IntegerConstantType  rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator-(const IntegerConstantType  lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(-rhs);
            return cpy += lhs;
        }
        [[nodiscard]] AlgebraicExpression operator-(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator-(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(-rhs);
            return cpy += lhs;
        }
        [[nodiscard]] AlgebraicExpression operator-(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy -= rhs;
        }

        [[nodiscard]] AlgebraicExpression operator*(const AlgebraicExpression &lhs,
                                                    const IntegerConstantType  rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy *= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator*(const IntegerConstantType  lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy *= lhs;
        }
        [[nodiscard]] AlgebraicExpression operator*(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy *= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator*(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy *= lhs;
        }
        [[nodiscard]] AlgebraicExpression operator*(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy *= rhs;
        }

        [[nodiscard]] AlgebraicExpression operator/(const AlgebraicExpression &lhs,
                                                    const IntegerConstantType  rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator/(const IntegerConstantType  lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator/(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator/(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator/(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }

        [[nodiscard]] bool operator==(const AlgebraicExpression &lhs,
                                      const AlgebraicExpression &rhs) noexcept {
            if (&lhs == &rhs) {
                return true;
            }
            return details::isEqual(lhs.m_root, rhs.m_root);
        }
        [[nodiscard]] bool operator!=(const AlgebraicExpression &lhs,
                                      const AlgebraicExpression &rhs) noexcept {
            if (&lhs == &rhs) {
                return false;
            }
            return !details::isEqual(lhs.m_root, rhs.m_root);
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const AlgebraicExpression         &rhs) noexcept {
            if constexpr (std::is_same_v<CharT, char>) {
                std::print(out, "{}", rhs);
            } else if constexpr (std::is_same_v<CharT, wchar_t>) {
                out << std::format(L"{}", rhs);
            } else {
                // 无能为力，自求多福
                out << std::format("{}", rhs);
            }
            return out;
        }

        // NumericExpression类的成员定义
        [[nodiscard]] NumericExpression::NumericExpression() noexcept
            : m_root(details::createZeroNode()) {}

        [[nodiscard]] NumericExpression::NumericExpression(const IntegerConstantType constant) noexcept
            : m_root(details::makeNode<details::Monomial>(constant)) {}

        NumericExpression::~NumericExpression() noexcept = default;

        [[nodiscard]] NumericExpression::NumericExpression(const NumericExpression &rhs) noexcept
            : m_root(std::make_unique<details::Node>(*rhs.m_root)) {}

        [[nodiscard]] NumericExpression::NumericExpression(NumericExpression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

        NumericExpression &NumericExpression::operator=(const NumericExpression &rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::make_unique<details::Node>(*rhs.m_root);
            }
            return *this;
        }

        NumericExpression &NumericExpression::operator=(NumericExpression &&rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::move(rhs.m_root);
            }
            return *this;
        }

        NumericExpression &NumericExpression::operator+=(const IntegerConstantType rhs) & noexcept {
            details::plus(m_root, rhs);
            return *this;
        }
        NumericExpression &NumericExpression::operator+=(const NumericExpression &rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Addition>(std::move(m_root), std::make_unique<Node>(*rhs.m_root));
            simplify(m_root);
            return *this;
        }

        NumericExpression &NumericExpression::operator-=(const IntegerConstantType rhs) & noexcept {
            details::plus(m_root, -rhs);
            return *this;
        }
        NumericExpression &NumericExpression::operator-=(const NumericExpression &rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Addition>(
                std::move(m_root),
                makeNode<Multiplication>(makeNode<Monomial>(-1), std::make_unique<Node>(*rhs.m_root)));
            details::simplify(m_root);
            return *this;
        }

        NumericExpression &NumericExpression::operator*=(const IntegerConstantType rhs) & noexcept {
            details::multiply(m_root, rhs);
            return *this;
        }
        NumericExpression &NumericExpression::operator*=(const NumericExpression &rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Multiplication>(std::move(m_root), std::make_unique<Node>(*rhs.m_root));
            simplify(m_root);
            return *this;
        }

        NumericExpression &NumericExpression::operator/=(const IntegerConstantType rhs) & noexcept {
            details::divide(m_root, rhs);
            return *this;
        }
        NumericExpression &NumericExpression::operator/=(const NumericExpression &rhs) & noexcept {
            using namespace details;
            m_root = makeNode<Division>(std::move(m_root), std::make_unique<Node>(*rhs.m_root));
            simplify(m_root);
            return *this;
        }

        [[nodiscard]] NumericExpression NumericExpression::operator+() const noexcept { return *this; }

        [[nodiscard]] NumericExpression NumericExpression::operator-() const noexcept {
            NumericExpression copy_of_this(*this);
            copy_of_this.changeToOpposite();
            return copy_of_this;
        }

        NumericExpression &NumericExpression::operator++() noexcept {
            details::plus(m_root, 1);
            return *this;
        }
        NumericExpression &NumericExpression::operator--() noexcept {
            details::plus(m_root, -1);
            return *this;
        }

        [[nodiscard]] NumericExpression NumericExpression::operator++(int) noexcept {
            NumericExpression copy_of_old(*this);
            details::plus(m_root, 1);
            return copy_of_old;
        }
        [[nodiscard]] NumericExpression NumericExpression::operator--(int) noexcept {
            NumericExpression copy_of_old(*this);
            details::plus(m_root, -1);
            return copy_of_old;
        }

        template <std::floating_point FloatT>
        [[nodiscard]] FloatT NumericExpression::calculateApproximation() const noexcept {
            return details::calculateApproximation<FloatT>(
                m_root, [](VariableView /*unused*/) -> FloatT {
                    assert_msg(false, "NumericExpression 不应该有 Variable 节点");
                    return 0;
                });
        }

        void NumericExpression::clear() noexcept { m_root = details::createZeroNode(); }

        [[nodiscard]] std::string NumericExpression::toString() const noexcept {
            return details::nodeToString(m_root);
        }

        [[nodiscard]] std::wstring NumericExpression::toWString() const noexcept {
            return std::format(L"{}", *this);
        }

        void NumericExpression::changeToOpposite() noexcept { details::multiply(m_root, -1); }

        [[nodiscard]] NumericExpression operator+(const NumericExpression  &lhs,
                                                  const IntegerConstantType rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy += rhs;
        }
        [[nodiscard]] NumericExpression operator+(const IntegerConstantType lhs,
                                                  const NumericExpression  &rhs) noexcept {
            NumericExpression cpy(rhs);
            return cpy += lhs;
        }
        [[nodiscard]] NumericExpression operator+(const NumericExpression &lhs,
                                                  const NumericExpression &rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy += rhs;
        }

        [[nodiscard]] NumericExpression operator-(const NumericExpression  &lhs,
                                                  const IntegerConstantType rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] NumericExpression operator-(const IntegerConstantType lhs,
                                                  const NumericExpression  &rhs) noexcept {
            NumericExpression cpy(rhs);
            return cpy -= lhs;
        }
        [[nodiscard]] NumericExpression operator-(const NumericExpression &lhs,
                                                  const NumericExpression &rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy -= rhs;
        }

        [[nodiscard]] NumericExpression operator*(const NumericExpression  &lhs,
                                                  const IntegerConstantType rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy *= rhs;
        }
        [[nodiscard]] NumericExpression operator*(const IntegerConstantType lhs,
                                                  const NumericExpression  &rhs) noexcept {
            NumericExpression cpy(rhs);
            return cpy *= lhs;
        }
        [[nodiscard]] NumericExpression operator*(const NumericExpression &lhs,
                                                  const NumericExpression &rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy *= rhs;
        }

        [[nodiscard]] NumericExpression operator/(const NumericExpression  &lhs,
                                                  const IntegerConstantType rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] NumericExpression operator/(const IntegerConstantType lhs,
                                                  const NumericExpression  &rhs) noexcept {
            NumericExpression cpy(rhs);
            return cpy /= lhs;
        }
        [[nodiscard]] NumericExpression operator/(const NumericExpression &lhs,
                                                  const NumericExpression &rhs) noexcept {
            NumericExpression cpy(lhs);
            return cpy /= rhs;
        }

        [[nodiscard]] bool operator==(const NumericExpression &lhs,
                                      const NumericExpression &rhs) noexcept {
            if (&lhs == &rhs) {
                return true;
            }
            return details::isEqual(lhs.m_root, rhs.m_root);
        }
        [[nodiscard]] bool operator!=(const NumericExpression &lhs,
                                      const NumericExpression &rhs) noexcept {
            if (&lhs == &rhs) {
                return false;
            }
            return !details::isEqual(lhs.m_root, rhs.m_root);
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const NumericExpression           &rhs) noexcept {
            if constexpr (std::is_same_v<CharT, char>) {
                std::print(out, "{}", rhs);
            } else if constexpr (std::is_same_v<CharT, wchar_t>) {
                out << std::format(L"{}", rhs);
            } else {
                // 无能为力，自求多福
                out << std::format("{}", rhs);
            }
            return out;
        }

        // 模板实例化
        template std::basic_ostream<char> &operator<< <char>(std::basic_ostream<char>  &out,
                                                             const AlgebraicExpression &rhs) noexcept;
        template std::basic_ostream<wchar_t> &
        operator<< <wchar_t>(std::basic_ostream<wchar_t> &out, const AlgebraicExpression &rhs) noexcept;
        template float AlgebraicExpression::calculateApproximation<float>(
            const std::function<float(VariableView)> &converter) const noexcept;
        template double AlgebraicExpression::calculateApproximation<double>(
            const std::function<double(VariableView)> &converter) const noexcept;
        template long double AlgebraicExpression::calculateApproximation<long double>(
            const std::function<long double(VariableView)> &converter) const noexcept;

        template std::basic_ostream<char> &operator<< <char>(std::basic_ostream<char> &out,
                                                             const NumericExpression  &rhs) noexcept;
        template std::basic_ostream<wchar_t> &
        operator<< <wchar_t>(std::basic_ostream<wchar_t> &out, const NumericExpression &rhs) noexcept;
        template float       NumericExpression::calculateApproximation<float>() const noexcept;
        template double      NumericExpression::calculateApproximation<double>() const noexcept;
        template long double NumericExpression::calculateApproximation<long double>() const noexcept;

    } // namespace math

    inline namespace literals {

        inline namespace expressions_base_literals {
            [[nodiscard]] constexpr math::IntegerConstantType
            operator""_c(unsigned long long constant) noexcept {
                return static_cast<math::IntegerConstantType>(constant);
            }
            [[nodiscard]] math::VariableType operator""_v(const char *variable,
                                                          std::size_t len) noexcept {
                return {variable, len};
            }
            [[nodiscard]] math::VariableView operator""_vv(const char *variable,
                                                           std::size_t len) noexcept {
                return {variable, len};
            }
        } // namespace expressions_base_literals

        inline namespace algebraic_expression_literals {

            [[nodiscard]] math::AlgebraicExpression
            operator""_cAlgeExpr(const unsigned long long constant) noexcept {
                return math::AlgebraicExpression{operator""_c(constant)};
            }
            [[nodiscard]] math::AlgebraicExpression operator""_vAlgeExpr(const char *variable,
                                                                         std::size_t len) noexcept {
                return math::AlgebraicExpression{operator""_vv(variable, len)};
            }

        } // namespace algebraic_expression_literals

        inline namespace numeric_expression_literals {

            [[nodiscard]] math::NumericExpression
            operator""_cNumExpr(const unsigned long long constant) noexcept {
                return math::NumericExpression{operator""_c(constant)};
            }

        } // namespace numeric_expression_literals

    } // namespace literals

} // namespace tnrw
