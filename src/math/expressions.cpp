/**
 * @file expressions.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了代数式和无字母的代数式类的细节
 * @version 0.1.0-2
 * @date 2026-01-17
 * 
 * @copyright cpp-love
 * 
 */

#include "base/assert_msg.hpp"
#include "math/AlgebraicExpression.hpp"
#include "math/NumericExpression.hpp"
#include "math/expressions_base.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <entt/core/utility.hpp>
#include <format>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <print>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

/// @cond INTERNAL

/// @brief @ref tnrw::math 命名空间的一些功能的实现细节
namespace tnrw::math::details {

    /**
     * @brief 整型常量节点
     * @details
     *  - 其下没有子节点
     */
    struct IntegerConstant {
        IntegerConstantType m_value; ///< 值
    };

    /**
     * @brief 变量节点
     * @details
     *  - 其下没有子节点
     */
    struct Variable {
        VariableType m_value; ///< 值
    };

    /**
     * @brief 加法节点
     * @details
     *  - 若加的项中有常量（包括非整型的），则必须为 `m_children` 成员的第0项（即 `m_children[0]` 的位置）
     *  - 其下的直接子节点只能为 @ref IntegerConstant , @ref Variable , @ref Negation , @ref Multiplication , @ref Division
     */
    struct Addition {
        std::vector<NodePtr> m_children; ///< 子节点

        /**
         * @brief 从节点列表构造的构造函数
         * @tparam Args 参数类型
         * @param [in] args 节点列表
         */
        template <typename... Args>
            requires(std::is_same_v<Args &&, NodePtr &&> && ...) && (sizeof...(Args) > 1)
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
        /**
         * @brief 析构函数
         */
        ~Addition() noexcept = default;
        /// @brief 禁止复制赋值运算符重载
        Addition &operator=(const Addition &rhs) noexcept = delete;
        /// @brief 禁止移动赋值运算符重载
        Addition &operator=(Addition &&rhs) noexcept = delete;
    };

    /**
     * @brief 相反数节点
     *  - 其下的直接子节点只能为 @ref Variable , @ref Multiplication , @ref Division
     */
    struct Negation {
        NodePtr m_value; ///< 值

        /**
         * @brief 从代数式构造的构造函数
         * @param [in] rhs 代数式
         */
        explicit Negation(NodePtr rhs) noexcept;
        /**
         * @brief 深复制构造函数
         * @param [in] rhs 另一个对象
         */
        Negation(const Negation &rhs) noexcept;
        /**
         * @brief 移动构造函数
         * @param [in] rhs 另一个对象
         */
        Negation(Negation &&rhs) noexcept = default;
        /**
         * @brief 析构函数
         */
        ~Negation() noexcept = default;
        /// @brief 禁止复制赋值运算符重载
        Negation &operator=(const Negation &rhs) noexcept = delete;
        /// @brief 禁止移动赋值运算符重载
        Negation &operator=(Negation &&rhs) noexcept = delete;
    };

    /**
     * @brief 乘法节点
     *  - 若加的项中有整型常量，则必须为 `m_children` 成员的第0项（即 `m_children[0]` 的位置）且其必须为正
     *  - 应提取所有在子节点中可提取的取相反数的操作，若本身为负，则在外面套 @ref Negation 节点
     *  - 此节点下的变量必须按名字排序
     *  - 其下的直接子节点只能为 @ref IntegerConstant , @ref Variable
     */
    struct Multiplication {
        std::vector<NodePtr> m_children; ///< 子节点

        /**
         * @brief 从节点列表构造的构造函数
         * @tparam Args 参数类型
         * @param [in] args 节点列表
         */
        template <typename... Args>
            requires(std::is_same_v<Args &&, NodePtr &&> && ...) && (sizeof...(Args) > 1)
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
        /**
         * @brief 析构函数
         */
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
     *  - 其下的直接子节点只能为 @ref IntegerConstant , @ref Variable , @ref Addition , @ref Multiplication
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
        /**
         * @brief 析构函数
         */
        ~Division() noexcept = default;
        /// @brief 禁止复制赋值运算符重载
        Division &operator=(const Division &rhs) noexcept = delete;
        /// @brief 禁止移动赋值运算符重载
        Division &operator=(Division &&rhs) noexcept = delete;
    };

    /**
     * @brief 基本节点
     */
    struct Node {
        std::variant<IntegerConstant, Variable, Addition, Negation, Multiplication, Division>
                       m_value; ///< 值
        /**
         * @brief 生成空节点（为0）
         * @return NodePtr 空节点
         */
        static NodePtr createZero() noexcept;
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
    concept SubNode = requires(Node node) { requires is_variant_member_v<T, decltype(node.m_value)>; };

    // 为了避免 Node 未定义的问题，延后定义构造函数
    template <typename... Args>
        requires(std::is_same_v<Args &&, NodePtr &&> && ...) && (sizeof...(Args) > 1)
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

    Negation::Negation(NodePtr rhs) noexcept : m_value(std::move(rhs)) {}
    Negation::Negation(const Negation &rhs) noexcept : m_value{std::make_unique<Node>(*rhs.m_value)} {}

    template <typename... Args>
        requires(std::is_same_v<Args &&, NodePtr &&> && ...) && (sizeof...(Args) > 1)
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

    NodePtr Node::createZero() noexcept { return makeNode<IntegerConstant>(0); }

    /**
     * @brief 清空节点
     * @param [in] root 节点
     */
    void    clearNode(NodePtr &root) noexcept {
        assert_msg(root != nullptr, "参数 root 错误地为 nullptr");
        root = Node::createZero();
    }

    /**
     * @brief 判断两个代数式是否相等
     * @param [in] lhs 代数式1
     * @param [in] rhs 代数式2
     * @return true 相等
     * @return false 不相等
     */
    bool isEqual(const NodePtr &lhs, const NodePtr &rhs) noexcept {
        assert_msg(lhs != nullptr && rhs != nullptr, "参数 lhs 和/或 rhs 错误地为 nullptr");
        return std::visit(
            entt::overloaded{[&](const IntegerConstant &value1, const IntegerConstant &value2) -> bool {
                                 return value1.m_value == value2.m_value;
                             },
                             [&](const Variable &value1, const Variable &value2) -> bool {
                                 return value1.m_value == value2.m_value;
                             },
                             [&](const Negation &value1, const Negation &value2) -> bool {
                                 return isEqual(value1.m_value, value2.m_value);
                             },
                             [&]<typename T>(const T &value1, const T &value2) -> bool {
                                 if (value1.m_children.size() != value2.m_children.size()) {
                                     return false;
                                 }
                                 for (std::size_t i = 0; i < value1.m_children.size(); ++i) {
                                     if (!isEqual(value1.m_children[i], value2.m_children[i])) {
                                         return false;
                                     }
                                 }
                                 return true;
                             },
                             [&](const auto & /*unused*/, const auto & /*unused*/) { return false; }},
            lhs->m_value, rhs->m_value);
    }

    /**
     * @brief 将节点转换为 std::string
     * @param node 节点引用
     * @return std::string 节点的字符串表示
     */
    std::string nodeToString(const Node &node) {
        return std::visit(
            entt::overloaded{[](const IntegerConstant &value) { return std::to_string(value.m_value); },
                             [](const Variable &value) { return value.m_value; },
                             [](const Addition &value) {
                                 std::string str = "(";
                                 for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                                     if (i) {
                                         str += ")+(";
                                     }
                                     str += nodeToString(*value.m_children[i]);
                                 }
                                 str += ")";
                                 return str;
                             },
                             [](const Negation &value) {
                                 return std::string("-(") + nodeToString(*value.m_value) + ")";
                             },
                             [](const Multiplication &value) {
                                 std::string str;
                                 for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                                     if (i) {
                                         str += "*";
                                     }
                                     str += nodeToString(*value.m_children[i]);
                                 }
                                 return str;
                             },
                             [](const Division &value) {
                                 return std::string("(") + nodeToString(*value.m_children[0]) + ")/("
                                        + nodeToString(*value.m_children[1]) + ")";
                             }},
            node.m_value);
    }

    /**
     * @brief 简化代数式
     * @param [in] root 代数式
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
         * @param [in] root_val 子节点的引用
         * @param [in] root 节点的引用
         */
        void operator()(SubNodeT &root_val, NodePtr &root) noexcept;
    };

    template <>
    void Simplifier<IntegerConstant>::operator()(IntegerConstant & /*unused*/,
                                                 NodePtr & /*unused*/) noexcept {
        // 无需化简
    }

    template <>
    void Simplifier<Variable>::operator()(Variable & /*unused*/, NodePtr & /*unused*/) noexcept {
        // 无需化简
    }

    template <>
    void Simplifier<Addition>::operator()(Addition &root_val, NodePtr &root) noexcept {
        // std::println("the raw addition node is {}", nodeToString(*root));
        auto &children = root_val.m_children;
        if (children.empty()) {
            clearNode(root);
            return;
        }

        IntegerConstantType integer_augend = 0; //< 整型常量
        IntegerConstantType augend_num = 0;     //< 分数常量
        IntegerConstantType augend_den = 1;     //< 分数常量
        struct PositionAndCoeff {
            std::size_t         pos;
            IntegerConstantType coeff_num;
            IntegerConstantType coeff_den;
        };
        std::map<VariableView, PositionAndCoeff> var_coeffs; //< 单个变量节点的字符串 -> 其位置和系数
        std::map<std::string, PositionAndCoeff>  multi_vars; //< 多变量节点的字符串 -> 其位置和系数

        for (std::size_t i = 0; i < children.size();) {
            // 获取元素
            NodePtr cur = std::move(children[i]);
            std::swap(children[i], children.back());
            children.pop_back();
            assert_msg(cur != nullptr, "Addition 节点的子节点 cur 错误地为 nullptr");
            // 对每个子节点化简
            simplify(cur);

            /// @todo 取消不同的重载的重复
            std::visit(
                entt::overloaded{
                    [&](IntegerConstant &value) {
                        // 整合所有 IntegerConstant 节点
                        integer_augend += value.m_value;
                    },
                    [&](Addition &value) {
                        // 展开 Addition 节点
                        auto &sub_children = value.m_children;
                        for (NodePtr &sub : sub_children) { children.push_back(std::move(sub)); }
                    },
                    [&](Variable &value) {
                        auto [iter, succeeded] = var_coeffs.try_emplace(
                            value.m_value, PositionAndCoeff{.pos = i, .coeff_num = 1, .coeff_den = 1});
                        if (!succeeded) {
                            // 说明已经存在变量，相加
                            iter->second.coeff_num += iter->second.coeff_den;
                        } else {
                            // 复原
                            children.push_back(std::move(cur));
                            std::swap(children[i], children.back());
                            ++i;
                        }
                    },
                    [&](Multiplication &value) {
                        // 获取系数
                        IntegerConstantType coeff = 1;
                        auto               &vchildren = value.m_children;
                        if (std::holds_alternative<IntegerConstant>(vchildren.front()->m_value)) {
                            coeff = std::get<IntegerConstant>(vchildren.front()->m_value).m_value;
                            std::swap(vchildren.front(), vchildren.back());
                            vchildren.pop_back();
                        }
                        if (vchildren.size() == 1) {
                            // 单变量
                            auto [iter, succeeded] = var_coeffs.try_emplace(
                                std::get<Variable>(vchildren.front()->m_value).m_value,
                                PositionAndCoeff{.pos = i, .coeff_num = coeff, .coeff_den = 1});
                            if (!succeeded) {
                                // 说明已经存在变量，相加
                                iter->second.coeff_num += coeff * iter->second.coeff_den;
                            } else {
                                // 复原
                                children.push_back(std::move(cur));
                                std::swap(children[i], children.back());
                                ++i;
                            }
                            return;
                        }
                        // 多变量
                        std::string key = nodeToString(*cur);
                        auto [iter, succeeded] = multi_vars.try_emplace(
                            key, PositionAndCoeff{.pos = i, .coeff_num = coeff, .coeff_den = 1});
                        if (!succeeded) {
                            // 说明已经存在变量，相加
                            iter->second.coeff_num += coeff * iter->second.coeff_den;
                        } else {
                            // 复原
                            children.push_back(std::move(cur));
                            std::swap(children[i], children.back());
                            ++i;
                        }
                    },
                    [&](Division &value) {
                        // 获取系数
                        std::array<IntegerConstantType, 2> coeff = {1, 1};
                        for (std::size_t i = 0; i < value.m_children.size(); ++i) {
                            if (std::holds_alternative<Multiplication>(value.m_children[i]->m_value)) {
                                auto &vchildren =
                                    std::get<Multiplication>(value.m_children[i]->m_value).m_children;
                                if (std::holds_alternative<IntegerConstant>(
                                        vchildren.front()->m_value)) {
                                    coeff[i] =
                                        std::get<IntegerConstant>(vchildren.front()->m_value).m_value;
                                    std::swap(vchildren.front(), vchildren.back());
                                    vchildren.pop_back();
                                }
                                if (vchildren.size() == 1) {
                                    // 单变量
                                    NodePtr tmp = std::move(vchildren.front());
                                    value.m_children[i] = std::move(tmp);
                                }
                            }
                        }
                        if (std::holds_alternative<IntegerConstant>(value.m_children[0]->m_value)) {
                            auto &num = std::get<IntegerConstant>(value.m_children[0]->m_value).m_value;
                            if (std::holds_alternative<IntegerConstant>(value.m_children[1]->m_value)) {
                                // 特殊：是常数
                                auto den =
                                    std::get<IntegerConstant>(value.m_children[1]->m_value).m_value;
                                augend_num = (augend_num * den) + (num * augend_den);
                                augend_den *= den;
                                return;
                            }
                            coeff[0] = num;
                            num = 1;
                        } else if (std::holds_alternative<IntegerConstant>(
                                       value.m_children[1]->m_value)) {
                            coeff[1] = std::get<IntegerConstant>(value.m_children[1]->m_value).m_value;
                            if (std::holds_alternative<Variable>(value.m_children[0]->m_value)) {
                                // 单变量
                                auto &var = std::get<Variable>(value.m_children[0]->m_value).m_value;
                                auto [iter, succeeded] =
                                    var_coeffs.try_emplace(var, PositionAndCoeff{.pos = i,
                                                                                 .coeff_num = coeff[0],
                                                                                 .coeff_den = coeff[1]});
                                if (!succeeded) {
                                    // 说明已经存在变量，相加
                                    iter->second.coeff_num = (iter->second.coeff_num * coeff[1])
                                                             + (iter->second.coeff_den * coeff[0]);
                                    iter->second.coeff_den *= coeff[1];
                                } else {
                                    // 复原
                                    children.push_back(std::move(value.m_children[0]));
                                    std::swap(children[i], children.back());
                                    ++i;
                                }
                                return;
                            }
                            // 多变量
                            std::string key = nodeToString(*value.m_children[0]);
                            auto [iter, succeeded] =
                                multi_vars.try_emplace(key, PositionAndCoeff{.pos = i,
                                                                             .coeff_num = coeff[0],
                                                                             .coeff_den = coeff[1]});
                            if (!succeeded) {
                                // 说明已经存在变量，相加
                                iter->second.coeff_num = (iter->second.coeff_num * coeff[1])
                                                         + (iter->second.coeff_den * coeff[0]);
                                iter->second.coeff_den *= coeff[1];
                            } else {
                                // 复原
                                children.push_back(std::move(value.m_children[0]));
                                std::swap(children[i], children.back());
                                ++i;
                            }
                            return;
                        }
                        // 多变量
                        std::string key = nodeToString(*cur);
                        auto [iter, succeeded] = multi_vars.try_emplace(
                            key,
                            PositionAndCoeff{.pos = i, .coeff_num = coeff[0], .coeff_den = coeff[1]});
                        if (!succeeded) {
                            // 说明已经存在变量，相加
                            iter->second.coeff_num = (iter->second.coeff_num * coeff[1])
                                                     + (iter->second.coeff_den * coeff[0]);
                            iter->second.coeff_den *= coeff[1];
                        } else {
                            // 复原
                            children.push_back(std::move(cur));
                            std::swap(children[i], children.back());
                            ++i;
                        }
                    },
                    [&](Negation &value) {
                        NodePtr &inner = value.m_value;
                        std::visit(
                            entt::overloaded{
                                [&](auto & /*unused*/) {
                                    // 不支持情况
                                    assert_msg(false, "Negation 节点里面只能为 "
                                                      "Variable / Multiplication / Division 节点");
                                },
                                [&](Variable &nested_value) {
                                    auto [iter, succeeded] = var_coeffs.try_emplace(
                                        nested_value.m_value,
                                        PositionAndCoeff{.pos = i, .coeff_num = -1, .coeff_den = 1});
                                    if (!succeeded) {
                                        iter->second.coeff_num -= iter->second.coeff_den;
                                    } else {
                                        children.push_back(std::move(value.m_value));
                                        std::swap(children[i], children.back());
                                        ++i;
                                    }
                                },
                                [&](Multiplication &nested_value) {
                                    IntegerConstantType coeff = 1;
                                    auto               &vchildren = nested_value.m_children;
                                    if (std::holds_alternative<IntegerConstant>(
                                            vchildren.front()->m_value)) {
                                        coeff = std::get<IntegerConstant>(vchildren.front()->m_value)
                                                    .m_value;
                                        std::swap(vchildren.front(), vchildren.back());
                                        vchildren.pop_back();
                                    }
                                    if (vchildren.size() == 1) {
                                        auto [iter, succeeded] = var_coeffs.try_emplace(
                                            std::get<Variable>(vchildren.front()->m_value).m_value,
                                            PositionAndCoeff{.pos = i,
                                                             .coeff_num = -coeff,
                                                             .coeff_den = 1});
                                        if (!succeeded) {
                                            iter->second.coeff_num -= coeff * iter->second.coeff_den;
                                        } else {
                                            children.push_back(std::move(value.m_value));
                                            std::swap(children[i], children.back());
                                            ++i;
                                        }
                                        return;
                                    }
                                    std::string key = nodeToString(*cur);
                                    auto [iter, succeeded] = multi_vars.try_emplace(
                                        key,
                                        PositionAndCoeff{.pos = i, .coeff_num = -coeff, .coeff_den = 1});
                                    if (!succeeded) {
                                        iter->second.coeff_num -= coeff * iter->second.coeff_den;
                                    } else {
                                        children.push_back(std::move(value.m_value));
                                        std::swap(children[i], children.back());
                                        ++i;
                                    }
                                },
                                [&](Division &nested_value) {
                                    std::array<IntegerConstantType, 2> coeff = {1, 1};
                                    for (std::size_t idx = 0; idx < nested_value.m_children.size();
                                         ++idx) {
                                        if (std::holds_alternative<Multiplication>(
                                                nested_value.m_children[idx]->m_value)) {
                                            auto &vchildren = std::get<Multiplication>(
                                                                  nested_value.m_children[idx]->m_value)
                                                                  .m_children;
                                            if (std::holds_alternative<IntegerConstant>(
                                                    vchildren.front()->m_value)) {
                                                coeff[idx] =
                                                    std::get<IntegerConstant>(vchildren.front()->m_value)
                                                        .m_value;
                                                std::swap(vchildren.front(), vchildren.back());
                                                vchildren.pop_back();
                                            }
                                            if (vchildren.size() == 1) {
                                                NodePtr tmp = std::move(vchildren.front());
                                                nested_value.m_children[idx] = std::move(tmp);
                                            }
                                        }
                                    }
                                    if (std::holds_alternative<IntegerConstant>(
                                            nested_value.m_children[0]->m_value)) {
                                        auto &num = std::get<IntegerConstant>(
                                                        nested_value.m_children[0]->m_value)
                                                        .m_value;
                                        if (std::holds_alternative<IntegerConstant>(
                                                nested_value.m_children[1]->m_value)) {
                                            auto den = std::get<IntegerConstant>(
                                                           nested_value.m_children[1]->m_value)
                                                           .m_value;
                                            // 减去分数常数
                                            augend_num = (augend_num * den) - (num * augend_den);
                                            augend_den *= den;
                                            return;
                                        }
                                        coeff[0] = num;
                                        num = 1;
                                    } else if (std::holds_alternative<IntegerConstant>(
                                                   nested_value.m_children[1]->m_value)) {
                                        coeff[1] = std::get<IntegerConstant>(
                                                       nested_value.m_children[1]->m_value)
                                                       .m_value;
                                        if (std::holds_alternative<Variable>(
                                                nested_value.m_children[0]->m_value)) {
                                            auto &var =
                                                std::get<Variable>(nested_value.m_children[0]->m_value)
                                                    .m_value;
                                            auto [iter, succeeded] = var_coeffs.try_emplace(
                                                var, PositionAndCoeff{.pos = i,
                                                                      .coeff_num = -coeff[0],
                                                                      .coeff_den = coeff[1]});
                                            if (!succeeded) {
                                                iter->second.coeff_num =
                                                    (iter->second.coeff_num * coeff[1])
                                                    - (iter->second.coeff_den * coeff[0]);
                                                iter->second.coeff_den *= coeff[1];
                                            } else {
                                                children.push_back(
                                                    std::move(nested_value.m_children[0]));
                                                std::swap(children[i], children.back());
                                                ++i;
                                            }
                                            return;
                                        }
                                        std::string key = nodeToString(*nested_value.m_children[0]);
                                        auto [iter, succeeded] = multi_vars.try_emplace(
                                            key, PositionAndCoeff{.pos = i,
                                                                  .coeff_num = -coeff[0],
                                                                  .coeff_den = coeff[1]});
                                        if (!succeeded) {
                                            iter->second.coeff_num =
                                                (iter->second.coeff_num * coeff[1])
                                                - (iter->second.coeff_den * coeff[0]);
                                            iter->second.coeff_den *= coeff[1];
                                        } else {
                                            children.push_back(std::move(nested_value.m_children[0]));
                                            std::swap(children[i], children.back());
                                            ++i;
                                        }
                                        return;
                                    }
                                    std::string key = nodeToString(*cur);
                                    auto [iter, succeeded] = multi_vars.try_emplace(
                                        key, PositionAndCoeff{.pos = i,
                                                              .coeff_num = -coeff[0],
                                                              .coeff_den = coeff[1]});
                                    if (!succeeded) {
                                        iter->second.coeff_num = (iter->second.coeff_num * coeff[1])
                                                                 - (iter->second.coeff_den * coeff[0]);
                                        iter->second.coeff_den *= coeff[1];
                                    } else {
                                        children.push_back(std::move(value.m_value));
                                        std::swap(children[i], children.back());
                                        ++i;
                                    }
                                }},
                            inner->m_value);
                    }},
                cur->m_value);
        }

        std::vector<size_t> should_delete_pos; //< 应删除的位置

        // 重建单变量的系数
        for (auto &[var, pos_and_coeff] : var_coeffs) {
            NodePtr &cur = children[pos_and_coeff.pos];
            if (pos_and_coeff.coeff_num == 0) {
                should_delete_pos.push_back(pos_and_coeff.pos);
                continue;
            }
            IntegerConstantType gcdnum = std::gcd(pos_and_coeff.coeff_num, pos_and_coeff.coeff_den);
            assert_msg(gcdnum != 0, "gcdnum 错误地为0");
            pos_and_coeff.coeff_num /= gcdnum;
            pos_and_coeff.coeff_den /= gcdnum;
            if (IntegerConstantType num = std::abs(pos_and_coeff.coeff_num); num != 1) {
                cur = makeNode<Multiplication>(makeNode<IntegerConstant>(num), std::move(cur));
            }
            if (pos_and_coeff.coeff_den != 1) {
                cur = makeNode<Division>(std::move(cur),
                                         makeNode<IntegerConstant>(pos_and_coeff.coeff_den));
            }
            if (pos_and_coeff.coeff_num < 0) {
                cur = makeNode<Negation>(std::move(cur));
            }
        }

        // 重建多变量的系数
        for (const auto &[var, pos_and_coeff] : multi_vars) {
            auto &cur = children[pos_and_coeff.pos];
            if (pos_and_coeff.coeff_num == 0) {
                should_delete_pos.push_back(pos_and_coeff.pos);
                continue;
            }
            cur = makeNode<Multiplication>(std::move(cur),
                                           makeNode<IntegerConstant>(pos_and_coeff.coeff_num));
            cur = makeNode<Division>(std::move(cur), makeNode<IntegerConstant>(pos_and_coeff.coeff_den));
            simplify(cur);
        }

        // 删除多余的节点
        std::ranges::sort(should_delete_pos, std::ranges::greater());
        for (size_t pos : should_delete_pos) {
            std::swap(children[pos], children.back());
            children.pop_back();
        }

        // 重建常量
        augend_num += integer_augend * augend_den;
        if (augend_num == 0) {
            // 若只有一个或没有子节点，取消 Addition 节点
            if (children.size() == 1) {
                NodePtr tmp = std::move(children[0]);
                root = std::move(tmp);
            } else if (children.empty()) {
                clearNode(root);
            }
            return;
        }
        IntegerConstantType gcdnum = std::gcd(augend_num, augend_den);
        assert_msg(gcdnum != 0, "gcdnum 错误地为0");
        augend_num /= gcdnum;
        augend_den /= gcdnum;
        NodePtr constant = makeNode<IntegerConstant>(augend_num);
        if (augend_den != 1) {
            constant = makeNode<Division>(std::move(constant), makeNode<IntegerConstant>(augend_den));
        }
        children.push_back(std::move(constant));
        std::swap(children.front(), children.back());
        // 若只有一个子节点，取消 Addition 节点
        if (children.size() == 1) {
            NodePtr tmp = std::move(children.front());
            root = std::move(tmp);
        }
        // std::println("the simpilfied addition node is {}", nodeToString(*root));
    }

    template <>
    void Simplifier<Negation>::operator()(Negation &root_val, NodePtr &root) noexcept {
        assert_msg(root_val.m_value != nullptr, "Negation node 的子节点为空");
        // std::println("the raw negation node is {}", nodeToString(*root));
        simplify(root_val.m_value);
        std::visit(entt::overloaded{[&](IntegerConstant &value) {
                                        // 若子节点为整型常量，直接取反
                                        root = makeNode<IntegerConstant>(-value.m_value);
                                    },
                                    [&](Negation &value) {
                                        // 若子节点为 Negation，去掉双重取反
                                        NodePtr inner = std::move(value.m_value);
                                        root = std::move(inner);
                                    },
                                    [&](Variable & /*unused*/) {
                                        // 无需化简
                                    },
                                    [&](auto & /*unused*/) {
                                        // 将节点转化为乘法节点，交由乘法节点来化简
                                        root = makeNode<Multiplication>(makeNode<IntegerConstant>(-1),
                                                                        std::move(root_val.m_value));
                                        simplify(root);
                                    }},
                   root_val.m_value->m_value);
        // std::println("the simpilfied negation node is {}", nodeToString(*root));
    }

    template <>
    void Simplifier<Multiplication>::operator()(Multiplication &root_val, NodePtr &root) noexcept {
        // std::println("the raw multiplication node is {}", nodeToString(*root));
        auto &children = root_val.m_children;
        if (children.empty()) {
            clearNode(root);
            return;
        }

        IntegerConstantType coeff = 1;
        bool                negative = false;
        for (std::size_t i = 0; i < children.size();) {
            NodePtr cur = std::move(children[i]);
            std::swap(children[i], children.back());
            children.pop_back();
            assert_msg(cur != nullptr, "Multiplication 节点的子节点 cur 错误地为 nullptr");
            simplify(cur);
            bool finished = false; //< 是否结束
            std::visit(
                entt::overloaded{
                    [&](IntegerConstant &value) {
                        // 整合所有 IntegerConstant 节点
                        coeff *= value.m_value;
                    },
                    [&](Multiplication &value) {
                        // 展开 Multiplication 节点
                        children.reserve(children.size() + value.m_children.size());
                        for (NodePtr &child : value.m_children) { children.push_back(std::move(child)); }
                    },
                    [&](Negation &value) {
                        // 收集所有的 Negation 节点
                        negative = !negative;
                        children.push_back(std::move(value.m_value));
                    },
                    [&](Division &value) {
                        // 将 Division 的分母扩大到外面，交由 Division 的简化器来处理
                        children.push_back(std::move(value.m_children[0]));
                        NodePtr den = std::move(value.m_children[1]);
                        root = makeNode<Division>(
                            makeNode<Multiplication>(
                                std::move(root), makeNode<IntegerConstant>(negative ? -coeff : coeff)),
                            std::move(den));
                        simplify(root);
                        finished = true;
                    },
                    [&](Addition &value) {
                        // 将 root 节点除此节点外全部乘到此节点中，交由 Addition 的简化器来处理
                        children.push_back(makeNode<IntegerConstant>(negative ? -coeff : coeff));
                        for (NodePtr &child : value.m_children) {
                            Multiplication copy = root_val;
                            copy.m_children.push_back(std::move(child));
                            child = makeNode<Multiplication>(std::move(copy));
                        }
                        NodePtr new_root = makeNode<Addition>(std::move(value));
                        simplify(new_root);
                        root = std::move(new_root);
                        finished = true;
                    },
                    [&](Variable & /*unused*/) {
                        // 不用改动
                        children.push_back(std::move(cur));
                        std::swap(children[i], children.back());
                        ++i;
                    }},
                cur->m_value);
            if (coeff == 0) {
                // 早发现，早返回
                root = Node::createZero();
                return;
            }
            if (finished) {
                return;
            }
        }

        // 确立正负
        if (coeff < 0) {
            coeff = -coeff;
            negative = !negative;
        }

        // 重建整型常数
        auto var_begin = children.begin();
        if (coeff != 1 || var_begin == children.end()) {
            children.push_back(makeNode<IntegerConstant>(coeff));
            std::swap(children.front(), children.back());
            // 重置变量起始点
            var_begin = std::next(children.begin());
        }

        // 对非整型因子进行规范化：先把变量排到前面并按名字排序
        std::ranges::sort(var_begin, children.end(), [](const NodePtr &var1, const NodePtr &var2) {
            assert_msg(std::holds_alternative<Variable>(var1->m_value)
                           && std::holds_alternative<Variable>(var2->m_value),
                       "参数 var1/var2 的类型错误地不为 Variable");

            VariableView var1_str = std::get<Variable>(var1->m_value).m_value;
            VariableView var2_str = std::get<Variable>(var2->m_value).m_value;
            return var1_str < var2_str;
        });

        // 若只有一个子节点，展开
        if (children.size() == 1) {
            NodePtr tmp = std::move(children[0]);
            root = std::move(tmp);
        }

        // 处理负号
        if (negative) {
            root = makeNode<Negation>(std::move(root));
        }
        // std::println("the simpilfied multiplication node is {}", nodeToString(*root));
    }

    template <>
    void Simplifier<Division>::operator()(Division &root_val, NodePtr &root) noexcept {
        /// @todo 完备多项式提取公因式逻辑
        // std::println("the raw division node is {}", nodeToString(*root));
        NodePtr &num = root_val.m_children[0];
        NodePtr &den = root_val.m_children[1];
        simplify(num);
        simplify(den);
        // 获取分子的常量及变量
        IntegerConstantType *num_coeff = nullptr;
        std::map<VariableView, std::vector<std::function<void()>>>
                              num_vars;              //< var -> std::vector<deleter>
        std::vector<size_t>   should_delete_pos;     //< 应删除的变量子节点位置
        std::function<void()> final_deleter = [] {}; //< 变量的最终删除器
        bool                  finished = false;      //< 是否结束
        std::visit(
            entt::overloaded{
                [&](IntegerConstant &value) {
                    if (value.m_value == 0) {
                        // 特殊：直接返回
                        clearNode(root);
                        finished = true;
                    } else {
                        num_coeff = &value.m_value;
                    }
                },
                [&](Variable &value) {
                    num_vars.emplace(value.m_value, std::vector{std::function{[&num] {
                                         num = makeNode<IntegerConstant>(1);
                                     }}});
                },
                [&](Addition &value) {
                    for (NodePtr &sub : value.m_children) {
                        sub = makeNode<Division>(std::move(sub), std::make_unique<Node>(*den));
                    }
                    root = makeNode<Addition>(std::move(value));
                    simplify(root);
                    finished = true;
                },
                [&](Multiplication &value) {
                    final_deleter = [&] {
                        if (should_delete_pos.empty()) {
                            return;
                        }
                        std::ranges::sort(should_delete_pos);
                        // 删除
                        auto        delete_pos_it = should_delete_pos.begin();
                        std::size_t left_i = 0;
                        for (std::size_t right_i = 0; true; ++left_i, ++right_i) {
                            if (delete_pos_it != should_delete_pos.end() && right_i == *delete_pos_it) {
                                // 这个位置应该被删除，跳过
                                ++right_i;
                            }
                            if (right_i >= value.m_children.size()) {
                                break;
                            }
                            value.m_children[left_i] = std::exchange(value.m_children[right_i], nullptr);
                        }
                        value.m_children.erase(
                            value.m_children.begin()
                                + static_cast<std::vector<NodePtr>::difference_type>(left_i),
                            value.m_children.end());
                        if (value.m_children.empty()) {
                            num = makeNode<IntegerConstant>(1);
                        } else if (value.m_children.size() == 1) {
                            NodePtr tmp = std::move(value.m_children.front());
                            num = std::move(tmp);
                        }
                    };
                    for (size_t i = 0; i < value.m_children.size(); ++i) {
                        NodePtr &sub = value.m_children[i];
                        std::visit(entt::overloaded{
                                       [&](IntegerConstant &value) { num_coeff = &value.m_value; },
                                       [&](Variable &value) {
                                           num_vars[value.m_value].emplace_back([&should_delete_pos, i] {
                                               should_delete_pos.push_back(i);
                                           });
                                       },
                                       [&](auto & /*unused*/) {
                                           assert_msg(false, "Multiplication 节点的子节点只能为 "
                                                             "IntegerConstant/Variable 节点");
                                       }},
                                   sub->m_value);
                    }
                },
                [&](Division &value) {
                    den = makeNode<Multiplication>(std::move(den), std::move(value.m_children[1]));
                    NodePtr tmp = std::move(value.m_children[0]);
                    num = std::move(tmp);
                    simplify(root);
                    finished = true;
                },
                [&](Negation &value) {
                    NodePtr tmp = std::move(value.m_value);
                    num = std::move(tmp);
                    root = makeNode<Negation>(std::move(root));
                    simplify(root);
                    finished = true;
                }},
            num->m_value);

        if (finished) {
            return;
        }

        std::visit(entt::overloaded{
                       [&](IntegerConstant &value) {
                           if (value.m_value == 1) {
                               NodePtr tmp = std::move(num);
                               root = std::move(tmp);
                               return;
                           }
                           if (num_coeff == nullptr) {
                               return;
                           }
                           auto gcdval = std::gcd(*num_coeff, value.m_value);
                           *num_coeff /= gcdval;
                           value.m_value /= gcdval;
                           if (*num_coeff == 1) {
                               simplify(num); //< 防止 1 * ... 的出现
                           }
                           if (value.m_value == 1) {
                               NodePtr tmp = std::move(num);
                               root = std::move(tmp);
                           }
                       },
                       [&](Variable &value) {
                           auto iter = num_vars.find(value.m_value);
                           if (iter == num_vars.end()) {
                               return;
                           }
                           iter->second.back()();
                           final_deleter();
                           NodePtr tmp = std::move(num);
                           root = std::move(tmp);
                       },
                       [&](Addition & /*usused*/) { assert_msg(false, "Not yet completed."); },
                       [&](Multiplication &value) {
                           auto &children = value.m_children;
                           for (auto iter = children.begin(); iter != children.end();) {
                               NodePtr &sub = *iter;
                               std::visit(entt::overloaded{
                                              [&](IntegerConstant &nested_value) {
                                                  if (num_coeff == nullptr) {
                                                      ++iter;
                                                      return;
                                                  }
                                                  auto gcdval =
                                                      std::gcd(*num_coeff, nested_value.m_value);
                                                  *num_coeff /= gcdval;
                                                  nested_value.m_value /= gcdval;
                                                  if (*num_coeff == 1) {
                                                      simplify(num); //< 防止 1 * ... 的出现
                                                  }
                                                  if (nested_value.m_value == 1) {
                                                      iter = children.erase(iter);
                                                  } else {
                                                      ++iter;
                                                  }
                                              },
                                              [&](Variable &nested_value) {
                                                  auto find_it = num_vars.find(nested_value.m_value);
                                                  if (find_it == num_vars.end()) {
                                                      ++iter;
                                                      return;
                                                  }
                                                  find_it->second.back()();
                                                  find_it->second.pop_back();
                                                  if (find_it->second.empty()) {
                                                      num_vars.erase(find_it);
                                                  }
                                                  iter = children.erase(iter);
                                              },
                                              [&](auto & /*usused*/) -> void {
                                                  assert_msg(false, "Multiplication 节点的子节点只能为 "
                                                                    "IntegerConstant/Variable 节点");
                                              }},
                                          sub->m_value);
                           }
                           final_deleter();
                           if (children.empty()) {
                               NodePtr tmp = std::move(num);
                               root = std::move(tmp);
                           } else if (children.size() == 1) {
                               NodePtr tmp = std::move(children.front());
                               den = std::move(tmp);
                           }
                       },
                       [&](Division &value) {
                           num =
                               makeNode<Multiplication>(std::move(num), std::move(value.m_children[1]));
                           NodePtr tmp = std::move(value.m_children[0]);
                           den = std::move(tmp);
                           simplify(root);
                       },
                       [&](Negation &value) {
                           NodePtr tmp = std::move(value.m_value);
                           den = std::move(tmp);
                           root = makeNode<Negation>(std::move(root));
                           simplify(root);
                       }},
                   den->m_value);
        // std::println("the simpilfied division node is {}", nodeToString(*root));
    }

    void simplify(NodePtr &root) {
        std::visit(
            [&](auto &value) {
                using T = std::decay_t<decltype(value)>;
                Simplifier<T>()(value, root);
            },
            root->m_value);
    }

    template <std::floating_point FloatT>
    FloatT calculateApproximation(const NodePtr &root, std::function<FloatT(VariableView)> converter) {
        return std::visit(
            entt::overloaded{
                [&](const IntegerConstant &value) { return static_cast<FloatT>(value.m_value); },
                [&](const Variable &value) { return converter(value.m_value); },
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
                           / calculateApproximation<FloatT>(value.m_children[1], converter);
                },
                [&](const Negation &value) {
                    return -calculateApproximation<FloatT>(value.m_value, converter);
                }},
            root->m_value);
    }

    /**
     * @brief 代数式加/减法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 常量
     */
    void plus(NodePtr &lhs, IntegerConstantType rhs) {
        lhs = makeNode<Addition>(std::move(lhs), makeNode<IntegerConstant>(rhs));
        simplify(lhs);
    }
    /**
     * @brief 代数式加法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void plus(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Addition>(std::move(lhs), makeNode<Variable>(VariableType(rhs)));
        simplify(lhs);
    }
    /**
     * @brief 代数式减法内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void minus(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Addition>(std::move(lhs),
                                 makeNode<Negation>(makeNode<Variable>(VariableType(rhs))));
        simplify(lhs);
    }
    /**
     * @brief 代数式乘法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 常量
     */
    void multiply(NodePtr &lhs, IntegerConstantType rhs) {
        lhs = makeNode<Multiplication>(std::move(lhs), makeNode<IntegerConstant>(rhs));
        simplify(lhs);
    }
    /**
     * @brief 代数式乘法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void multiply(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Multiplication>(std::move(lhs), makeNode<Variable>(VariableType(rhs)));
        simplify(lhs);
    }
    /**
     * @brief 代数式除法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 常量
     */
    void divide(NodePtr &lhs, IntegerConstantType rhs) {
        lhs = makeNode<Division>(std::move(lhs), makeNode<IntegerConstant>(rhs));
        simplify(lhs);
    }
    /**
     * @brief 代数式除法的内部实现
     * @param [in] lhs 代数式
     * @param [in] rhs 变量
     */
    void divide(NodePtr &lhs, VariableView rhs) {
        lhs = makeNode<Division>(std::move(lhs), makeNode<Variable>(VariableType(rhs)));
        simplify(lhs);
    }

} // namespace tnrw::math::details

/// @endcond

namespace tnrw {
    namespace math {

        // AlgebraicExpression类的成员定义
        [[nodiscard]] AlgebraicExpression::AlgebraicExpression() noexcept
            : m_root(details::Node::createZero()) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(IntegerConstantType constant) noexcept
            : m_root(details::makeNode<details::IntegerConstant>(constant)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(VariableView variable) noexcept
            : m_root(details::makeNode<details::Variable>(VariableType(variable))) {}

        AlgebraicExpression::~AlgebraicExpression() noexcept = default;

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(const AlgebraicExpression &rhs) noexcept
            : m_root(std::make_unique<details::Node>(*rhs.m_root)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(AlgebraicExpression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

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

        AlgebraicExpression &AlgebraicExpression::operator+=(IntegerConstantType rhs) & noexcept {
            details::plus(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator+=(VariableView rhs) & noexcept {
            details::plus(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator-=(IntegerConstantType rhs) & noexcept {
            details::plus(m_root, -rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator-=(VariableView rhs) & noexcept {
            details::minus(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator*=(IntegerConstantType rhs) & noexcept {
            details::multiply(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator*=(VariableView rhs) & noexcept {
            details::multiply(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator/=(IntegerConstantType rhs) & noexcept {
            details::divide(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator/=(VariableView rhs) & noexcept {
            details::divide(m_root, rhs);
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
            std::function<FloatT(VariableView)> converter) const noexcept {
            return details::calculateApproximation<FloatT>(m_root, std::move(converter));
        }

        void                      AlgebraicExpression::clear() noexcept { details::clearNode(m_root); }

        [[nodiscard]] std::string AlgebraicExpression::toString() const noexcept {
            return details::nodeToString(*m_root);
        }

        [[nodiscard]] std::wstring AlgebraicExpression::toWString() const noexcept {
            return std::format(L"{}", *this);
        }

        void AlgebraicExpression::changeToOpposite() noexcept { details::multiply(m_root, -1); }

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

        [[nodiscard]] AlgebraicExpression operator-(const AlgebraicExpression &lhs,
                                                    const IntegerConstantType  rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator-(const IntegerConstantType  lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy -= lhs;
        }

        [[nodiscard]] AlgebraicExpression operator-(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator-(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy -= lhs;
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

        [[nodiscard]] AlgebraicExpression operator/(const AlgebraicExpression &lhs,
                                                    const IntegerConstantType  rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator/(const IntegerConstantType  lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy /= lhs;
        }

        [[nodiscard]] AlgebraicExpression operator/(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept {
            AlgebraicExpression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] AlgebraicExpression operator/(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept {
            AlgebraicExpression cpy(rhs);
            return cpy /= lhs;
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
                out << std::format("{}", rhs);
            }
            return out;
        }

        // NumericExpression类的成员定义
        [[nodiscard]] NumericExpression::NumericExpression() noexcept
            : m_root(details::Node::createZero()) {}

        [[nodiscard]] NumericExpression::NumericExpression(const IntegerConstantType constant) noexcept
            : m_root(std::make_unique<details::Node>(details::IntegerConstant{constant})) {}

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

        NumericExpression &NumericExpression::operator-=(const IntegerConstantType rhs) & noexcept {
            details::plus(m_root, -rhs);
            return *this;
        }

        NumericExpression &NumericExpression::operator*=(const IntegerConstantType rhs) & noexcept {
            details::multiply(m_root, rhs);
            return *this;
        }

        NumericExpression &NumericExpression::operator/=(const IntegerConstantType rhs) & noexcept {
            details::divide(m_root, rhs);
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
            details::plus(m_root, 1_c);
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

        void                      NumericExpression::clear() noexcept { details::clearNode(m_root); }

        [[nodiscard]] std::string NumericExpression::toString() const noexcept {
            return details::nodeToString(*m_root);
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
            std::function<float(VariableView)> converter) const noexcept;
        template double AlgebraicExpression::calculateApproximation<double>(
            std::function<double(VariableView)> converter) const noexcept;
        template long double AlgebraicExpression::calculateApproximation<long double>(
            std::function<long double(VariableView)> converter) const noexcept;

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
