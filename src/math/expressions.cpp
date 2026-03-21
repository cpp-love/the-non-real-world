/**
 * @file expressions.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了代数式和无字母的代数式类的细节
 * @version 0.1.0-6
 * @date 2026-03-15
 * 
 * @copyright cpp-love
 * 
 * @details
 *  - 通过3个节点 @ref monomial @ref addition @ref division ,
 *    一个辅助节点 @ref multiplication 和一个别名 @ref polynomial
 *    来实现 @ref node 节点
 *  - 通过 std::visit 的访问来实现各种功能
 */

#include "base/assert_msg.hpp"
#include "base/overload.hpp"
#include "math/algebraic_expression.hpp"
#include "math/expressions_base.hpp"
#include "math/numeric_expression.hpp"
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
namespace tnrw::math {

    // 为了避免 node 未定义的问题，延后定义构造函数
    template <typename... Args>
        requires(std::is_same_v<Args &&, node_ptr &&> && ...)
    addition::addition(Args &&...args) noexcept {
        m_children.reserve(sizeof...(Args));
        (m_children.push_back(std::forward<Args>(args)), ...);
    }
    addition::addition(const addition &rhs) noexcept {
        m_children.reserve(rhs.m_children.size());
        for (const auto &value : rhs.m_children) {
            m_children.push_back(std::make_unique<node>(*value));
        }
    }

    template <typename... Args>
        requires(std::is_same_v<Args &&, node_ptr &&> && ...)
    multiplication::multiplication(Args &&...args) noexcept {
        m_children.reserve(sizeof...(Args));
        (m_children.push_back(std::forward<Args>(args)), ...);
    }
    multiplication::multiplication(const multiplication &rhs) noexcept {
        m_children.reserve(rhs.m_children.size());
        for (const auto &value : rhs.m_children) {
            m_children.push_back(std::make_unique<node>(*value));
        }
    }

    division::division(node_ptr num, node_ptr den) noexcept
        : m_children{std::move(num), std::move(den)} {}
    division::division(const division &rhs) noexcept
        : m_children{std::make_unique<node>(*rhs.m_children[0]),
                     std::make_unique<node>(*rhs.m_children[1])} {}

    namespace {
        /**
         * @brief 生成空节点（为0）
         * @return node_ptr 空节点
         */
        node_ptr create_zero_node() noexcept { return make_node<monomial>(0); }

        /**
         * @brief 简化代数式
         * @param [in, out] root 代数式
         */
        void     simplify(node_ptr &root);

        /**
         * @brief 代数式节点简化器
         * @tparam SubNodeT 子节点
         */
        template <sub_node SubNodeT>
        struct simplifier {
            /**
             * @brief 简化代数式节点
             * @param [in, out] root_val 子节点的引用
             * @param [in, out] root 节点的引用
             */
            void operator()(SubNodeT &root_val, node_ptr &root) noexcept;
        };

        template <>
        void simplifier<monomial>::operator()(monomial                  &root_val,
                                              [[maybe_unused]] node_ptr &root) noexcept {
            if (root_val.m_coeff == 0) {
                // 节点为0，清空
                root_val.m_var_exps.clear();
                return;
            }
            for (auto iter = root_val.m_var_exps.begin(); iter != root_val.m_var_exps.end();) {
                integer_constant_type &exp = iter->second;
                if (exp == 0) {
                    // 删除多余无用变量
                    iter = root_val.m_var_exps.erase(iter);
                } else {
                    ++iter;
                }
            }
        }

        template <>
        void simplifier<addition>::operator()(addition &root_val, node_ptr &root) noexcept {
            // std::println("the raw addition node is {}", node_to_string(root));
            auto &children = root_val.m_children;
            if (children.empty()) {
                root = create_zero_node();
                return;
            }

            integer_constant_type integer_augend = 0; //< 整型常量
            std::unordered_map<std::string, monomial>
                monomials; //< monomial 节点除去系数的字符串 -> 其值
            std::unordered_map<std::string, division>
                divisions; //< division 节点分母的字符串 -> 其分母和分子

            // 暂时不能将其改为range-for，因为在 addition 中有对 children 的更改
            for (std::size_t i = 0; i < children.size(); ++i) {
                node_ptr cur = std::move(children[i]);
                // 获取元素
                TNRW_ASSERT_MSG(cur != nullptr, "addition 节点的子节点 cur 错误地为 nullptr");
                // 对每个子节点化简
                simplify(cur);

                std::visit(make_overloaded(
                               [&](addition &value) {
                                   // 展开 addition 节点
                                   auto &sub_children = value.m_children;
                                   children.append_range(sub_children | std::views::as_rvalue);
                               },
                               [&](monomial &value) {
                                   if (value.m_var_exps.empty()) {
                                       // 是常量
                                       integer_augend += value.m_coeff;
                                       return;
                                   }
                                   // 是变量
                                   integer_constant_type coeff = std::exchange(value.m_coeff, 1);
                                   std::string           key = node_to_string(cur); //< 原始值的字符串
                                   value.m_coeff = coeff;
                                   auto [iter, succeeded] = monomials.try_emplace(key, std::move(value));
                                   if (!succeeded) {
                                       // 已经存在，相加
                                       iter->second.m_coeff += coeff;
                                   }
                               },
                               [&](multiplication & /*unused*/) {
                                   unreachable("化简后的节点不应为 multiplication 节点");
                               },
                               [&](division &value) {
                                   std::string key = node_to_string(value.m_children[1]);
                                   auto        iter =
                                       divisions
                                           .try_emplace(key, division{create_zero_node(),
                                                                      std::move(value.m_children[1])})
                                           .first;
                                   iter->second.m_children[0] =
                                       make_node<addition>(std::move(value.m_children[0]),
                                                           std::move(iter->second.m_children[0]));
                               }),
                           cur->m_value);
            }
            children.clear();

            // 重建 division 节点
            for (auto &[var, div] : divisions) {
                node_ptr new_child = make_node<division>(std::move(div));
                simplify(new_child);

                /// @todo 取消此处与上面重载的重复

                std::visit(
                    make_overloaded(
                        [&](addition &value) {
                            // 展开 addition 节点
                            auto &sub_children = value.m_children;
                            for (node_ptr &sub : sub_children) {
                                // sub一定是 monomial
                                TNRW_ASSERT_MSG(std::holds_alternative<monomial>(sub->m_value),
                                                "猜想错误：sub 实际上不一定为 monomial");
                                auto &sub_value = std::get<monomial>(sub->m_value);
                                if (sub_value.m_var_exps.empty()) {
                                    // 是常量
                                    integer_augend += sub_value.m_coeff;
                                    continue;
                                }
                                // 是变量
                                integer_constant_type coeff = std::exchange(sub_value.m_coeff, 1);
                                std::string           key = node_to_string(sub); //< 原始值的字符串
                                sub_value.m_coeff = coeff;
                                auto [iter, succeeded] =
                                    monomials.try_emplace(key, std::move(sub_value));
                                if (!succeeded) {
                                    // 已经存在，相加
                                    iter->second.m_coeff += coeff;
                                }
                            }
                        },
                        [&](monomial &value) {
                            if (value.m_var_exps.empty()) {
                                // 是常量
                                integer_augend += value.m_coeff;
                                return;
                            }
                            // 是变量
                            integer_constant_type coeff = std::exchange(value.m_coeff, 1);
                            std::string           key = node_to_string(new_child); //< 原始值的字符串
                            value.m_coeff = coeff;
                            auto [iter, succeeded] = monomials.try_emplace(key, std::move(value));
                            if (!succeeded) {
                                // 已经存在，相加
                                iter->second.m_coeff += coeff;
                            }
                        },
                        [&](multiplication & /*unused*/) {
                            unreachable("化简后的节点不应为 multiplication 节点");
                        },
                        [&](division &value) {
                            // 合并到前面的节点或添加
                            auto merge_or_add = [&children, &integer_augend, &monomials](
                                                    this auto &&merge_or_add, division &root_value,
                                                    node_ptr &root) -> void {
                                std::string key = node_to_string(root_value.m_children[1]);
                                for (node_ptr &child : children) {
                                    TNRW_ASSERT_MSG(std::holds_alternative<division>(child->m_value),
                                                    "猜想错误：child 实际上不一定为 division");
                                    auto &child_value = std::get<division>(child->m_value);
                                    if (key != node_to_string(child_value.m_children[1])) {
                                        continue;
                                    }
                                    child_value.m_children[0] =
                                        make_node<addition>(std::move(child_value.m_children[0]),
                                                            std::move(root_value.m_children[0]));
                                    simplify(child);
                                    node_ptr new_child = std::move(child);
                                    std::swap(child, children.back());
                                    children.pop_back();
                                    /// @todo 取消此处与上面重载的重复

                                    // 合并相同分母
                                    std::visit(
                                        make_overloaded(
                                            [&](addition &value) {
                                                // 展开 addition 节点
                                                auto &sub_children = value.m_children;
                                                for (node_ptr &sub : sub_children) {
                                                    // sub一定是 monomial
                                                    TNRW_ASSERT_MSG(
                                                        std::holds_alternative<monomial>(sub->m_value),
                                                        "猜想错误：sub 实际上不一定为 monomial");
                                                    auto &sub_value = std::get<monomial>(sub->m_value);
                                                    if (sub_value.m_var_exps.empty()) {
                                                        // 是常量
                                                        integer_augend += sub_value.m_coeff;
                                                        continue;
                                                    }
                                                    // 是变量
                                                    integer_constant_type coeff =
                                                        std::exchange(sub_value.m_coeff, 1);
                                                    std::string key =
                                                        node_to_string(sub); //< 原始值的字符串
                                                    sub_value.m_coeff = coeff;
                                                    auto [iter, succeeded] =
                                                        monomials.try_emplace(key, std::move(sub_value));
                                                    if (!succeeded) {
                                                        // 已经存在，相加
                                                        iter->second.m_coeff += coeff;
                                                    }
                                                }
                                            },
                                            [&](monomial &value) {
                                                if (value.m_var_exps.empty()) {
                                                    // 是常量
                                                    integer_augend += value.m_coeff;
                                                    return;
                                                }
                                                // 是变量
                                                integer_constant_type coeff =
                                                    std::exchange(value.m_coeff, 1);
                                                std::string key =
                                                    node_to_string(child); //< 原始值的字符串
                                                value.m_coeff = coeff;
                                                auto [iter, succeeded] =
                                                    monomials.try_emplace(key, std::move(value));
                                                if (!succeeded) {
                                                    // 已经存在，相加
                                                    iter->second.m_coeff += coeff;
                                                }
                                            },
                                            [&](multiplication & /*unused*/) {
                                                unreachable("化简后的节点不应为 multiplication 节点");
                                            },
                                            [&](division &value) {
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

            // 重建 monomial 节点
            for (auto &[var, mono] : monomials) {
                if (mono.m_coeff == 0) {
                    continue;
                }
                children.push_back(make_node<monomial>(std::move(mono)));
            }

            // 重建常量
            if (integer_augend == 0) {
                // 若只有一个或没有子节点，取消 addition 节点
                if (children.size() == 1) {
                    node_ptr tmp = std::move(children[0]);
                    root = std::move(tmp);
                } else if (children.empty()) {
                    root = create_zero_node();
                }
                return;
            }
            children.push_back(make_node<monomial>(integer_augend));
            std::swap(children.front(), children.back());
            // 若只有一个子节点，取消 addition 节点
            if (children.size() == 1) {
                node_ptr tmp = std::move(children.front());
                root = std::move(tmp);
            }
            // std::println("the simpilfied addition node is {}", node_to_string(root));
        }

        template <>
        void simplifier<multiplication>::operator()(multiplication &root_val, node_ptr &root) noexcept {
            // std::println("the raw multiplication node is {}", node_to_string(root));
            auto &children = root_val.m_children;
            if (children.empty()) {
                root = create_zero_node();
                return;
            }

            monomial new_node{.m_coeff = 1};
            for (auto iter = children.begin(); iter != children.end(); ++iter) {
                node_ptr cur = std::move(*iter);
                simplify(cur);
                bool finished = false; //< 是否结束
                std::visit(make_overloaded(
                               [&](monomial &value) {
                                   // 合并
                                   new_node.m_coeff *= value.m_coeff;
                                   new_node.m_var_exps.merge(value.m_var_exps);
                                   for (auto &[var, exp] : value.m_var_exps) {
                                       new_node.m_var_exps[var] += exp;
                                   }
                               },
                               [&](multiplication & /*unused*/) {
                                   unreachable("化简后的节点不应为 multiplication 节点");
                               },
                               [&](addition &value) {
                                   // 乘进去
                                   multiplication rest{
                                       make_node<monomial>(std::move(new_node))}; //< 目前剩余的节点
                                   ++iter;
                                   for (; iter != children.end(); ++iter) {
                                       rest.m_children.push_back(std::move(*iter));
                                   }
                                   for (node_ptr &sub_ch : value.m_children) {
                                       multiplication cpy = rest;
                                       cpy.m_children.push_back(std::move(sub_ch));
                                       sub_ch = make_node<multiplication>(std::move(cpy));
                                   }
                                   root = make_node<addition>(std::move(value));
                                   simplify(root);
                                   finished = true;
                               },
                               [&](division &value) {
                                   // 乘进去
                                   multiplication rest{
                                       make_node<monomial>(std::move(new_node)),
                                       std::move(value.m_children[0])}; //< 目前剩余的节点
                                   ++iter;
                                   for (; iter != children.end(); ++iter) {
                                       rest.m_children.push_back(std::move(*iter));
                                   }
                                   value.m_children[0] = make_node<multiplication>(rest);
                                   root = make_node<division>(std::move(value));
                                   simplify(root);
                                   finished = true;
                               }),
                           cur->m_value);
                if (finished) {
                    return;
                }
                if (new_node.m_coeff == 0) {
                    // 为0，退出
                    root = create_zero_node();
                    return;
                }
            }

            root = make_node<monomial>(std::move(new_node));
            simplify(root);
            // std::println("the simpilfied multiplication node is {}", node_to_string(root));
        }

        using polynomial = std::vector<node_ptr>; //< 多项式别名，索引 = 指数

        /**
         * @brief 去除多项式的多余节点
         * @param [in] poly 多项式
         */
        void remove_redundant_poly(polynomial &poly) {
            while (!poly.empty() && is_zero(poly.back())) { poly.pop_back(); }
        };

        /**
         * @brief 将普通代数式节点转换成为按主元指数排列的系数多项式向量
         * @param [in] root 代数式节点
         * @param [in] main_var 主元
         * @return polynomial 多项式向量
         * @warning 这要求 root 节点被化简过
         */
        polynomial to_poly(node_ptr root, variable_view main_var) {
            return std::visit(
                make_overloaded(
                    [&](monomial &value) {
                        auto                  iter = value.m_var_exps.find(main_var);
                        integer_constant_type exp = (iter == value.m_var_exps.end()) ? 0 : iter->second;
                        polynomial            poly;
                        poly.reserve(exp + 1);
                        std::ranges::generate_n(std::back_inserter(poly), exp + 1, create_zero_node);
                        if (iter != value.m_var_exps.end()) {
                            value.m_var_exps.erase(iter);
                        }
                        poly[exp] = std::move(root);
                        return poly;
                    },
                    [&](addition &value) {
                        polynomial poly;
                        for (node_ptr &child : value.m_children) {
                            auto                 &value = std::get<monomial>(child->m_value);
                            auto                  iter = value.m_var_exps.find(main_var);
                            integer_constant_type exp =
                                (iter == value.m_var_exps.end()) ? 0 : iter->second;

                            // 重新调整大小
                            if (std::size_t old_size = poly.size(), new_size = exp + 1;
                                old_size < new_size) {
                                poly.reserve(new_size);
                                std::ranges::generate_n(std::back_inserter(poly),
                                                        static_cast<long long>(new_size - old_size),
                                                        create_zero_node);
                            }

                            if (iter != value.m_var_exps.end()) {
                                value.m_var_exps.erase(iter);
                            }
                            poly[exp] = make_node<addition>(std::move(poly[exp]), std::move(child));
                            simplify(poly[exp]);
                        }
                        remove_redundant_poly(poly);
                        return poly;
                    },
                    [&](multiplication & /*unused*/) -> polynomial {
                        unreachable("化简后的节点不应为 multiplication 节点");
                    },
                    [&](division & /*unused*/) -> polynomial {
                        unreachable("多项式节点中不应有 division 节点");
                    }),
                root->m_value);
        };

        /**
         * @brief 将按主元指数排列的系数多项式向量转换成为普通代数式节点
         * @param [in] poly 多项式向量
         * @param [in] main_var 主元
         * @return node_ptr 代数式节点
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        node_ptr to_node_ptr(polynomial poly, variable_view main_var) {
            node_ptr ret = create_zero_node();
            for (const auto [exp, child] : std::views::enumerate(poly)) {
                ret = make_node<addition>(
                    std::move(ret), make_node<multiplication>(
                                        std::move(child),
                                        make_node<monomial>(1, monomial::map_var_exp{{main_var, exp}})));
            }
            simplify(ret);
            return ret;
        };

        /**
         * @brief 获取多项式的最高次数
         * @param [in] poly 多项式
         * @return std::size_t 最高次数
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        std::size_t get_degree_poly(const polynomial &poly) {
            return poly.empty() ? 0 : poly.size() - 1;
        };

        /**
         * @brief 获取多项式的首项系数的副本
         * @param [in] poly 多项式
         * @return node_ptr 多项式的首项系数的副本
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        node_ptr get_leading_coefficient_poly(const polynomial &poly) {
            return std::make_unique<node>(*poly.back());
        };

        /**
         * @brief 克隆多项式
         * @param [in] poly 多项式
         * @return polynomial 多项式副本
         */
        polynomial clone_poly(const polynomial &poly) {
            polynomial ret;
            ret.reserve(poly.size());
            for (const auto &child : poly) { ret.push_back(std::make_unique<node>(*child)); }
            return ret;
        };

        /**
         * @brief 将多项式乘上主元的 offset 次幂
         * @param [in] poly 多项式
         * @param [in] offset 幂数，或者是向高次的移动次数
         * @return polynomial 更改后的多项式
         */
        polynomial shift_poly(polynomial poly, std::size_t offset) {
            polynomial ret;
            ret.reserve(offset);
            std::ranges::generate_n(std::back_inserter(ret), static_cast<long long>(offset),
                                    create_zero_node);
            ret.append_range(poly | std::views::as_rvalue);
            return ret;
        };

        /**
         * @brief 判断多项式是否为0
         * @param [in] poly 多项式
         * @return true 为0 
         * @return false 不为0
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        bool       is_zero_poly(const polynomial &poly) { return poly.empty(); };

        /**
         * @brief 将两个多项式相加
         * @param [in] addend1 加数1
         * @param [in] addend2 加数2
         * @return polynomial 相加结果
         */
        polynomial add_poly(polynomial addend1, polynomial addend2) {
            std::size_t max_size = std::max(addend1.size(), addend2.size());
            polynomial  ret;
            ret.reserve(max_size);
            for (std::size_t i = 0; i < max_size; ++i) {
                node_ptr addend1_i = i < addend1.size() ? std::move(addend1[i]) : create_zero_node();
                node_ptr addend2_i = i < addend2.size() ? std::move(addend2[i]) : create_zero_node();
                // ret[i] = a + b
                ret.push_back(make_node<addition>(std::move(addend1_i), std::move(addend2_i)));
                simplify(ret[i]);
            }
            remove_redundant_poly(ret);
            return ret;
        };

        /**
         * @brief 将两个多项式相减
         * @param [in] minuend 被减数
         * @param [in] subtrahend 减数
         * @return polynomial 相减结果
         */
        polynomial sub_poly(polynomial minuend, polynomial subtrahend) {
            std::size_t max_size = std::max(minuend.size(), subtrahend.size());
            polynomial  ret;
            ret.reserve(max_size);
            for (std::size_t i = 0; i < max_size; ++i) {
                node_ptr minuend_i = i < minuend.size() ? std::move(minuend[i]) : create_zero_node();
                node_ptr subtrahend_i =
                    i < subtrahend.size() ? std::move(subtrahend[i]) : create_zero_node();
                // ret[i] = a - b  => a + (-1) * b
                ret.push_back(make_node<addition>(
                    std::move(minuend_i),
                    make_node<multiplication>(std::move(subtrahend_i), make_node<monomial>(-1))));
                simplify(ret[i]);
            }
            remove_redundant_poly(ret);
            return ret;
        };

        /**
         * @brief 将两个多项式相乘
         * @param [in] multiplier1 乘数1
         * @param [in] multiplier2 乘数2
         * @return polynomial 相乘结果
         */
        polynomial mul_poly(const polynomial &multiplier1, const polynomial &multiplier2) {
            polynomial ret;
            ret.reserve(multiplier1.size() + multiplier2.size());
            std::ranges::generate_n(std::back_inserter(ret),
                                    static_cast<long long>(multiplier1.size() + multiplier2.size()),
                                    create_zero_node);

            for (std::size_t i = 0; i < multiplier1.size(); ++i) {
                for (std::size_t j = 0; j < multiplier2.size(); ++j) {
                    ret[i + j] = make_node<addition>(
                        std::move(ret[i + j]),
                        make_node<multiplication>(std::make_unique<node>(*multiplier1[i]),
                                                  std::make_unique<node>(*multiplier2[j])));
                }
            }

            for (node_ptr &child : ret) { simplify(child); }
            remove_redundant_poly(ret);
            return ret;
        };

        /**
         * @brief 将两个多项式相除
         * @param [in] dividend 被除数
         * @param [in] divisor 除数
         * @return polynomial 相除结果
         * @warning 要求多项式必须整除
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        polynomial div_poly(polynomial dividend, const polynomial &divisor) {
            node_ptr   lc_divisor = get_leading_coefficient_poly(divisor);
            polynomial quotient;
            while (!is_zero_poly(dividend)) {
                TNRW_ASSERT_MSG(get_degree_poly(dividend) >= get_degree_poly(divisor),
                                "dividend 不能被 divisor 整除");
                node_ptr lc_dividend = get_leading_coefficient_poly(dividend);
                node_ptr lc_div_node =
                    make_node<division>(std::move(lc_dividend), std::make_unique<node>(*lc_divisor));
                simplify(lc_div_node);
                polynomial lc_div;
                lc_div.push_back(std::move(lc_div_node));
                // multiplier = lc(dividend) / lc(divisor) * (x ^ (deg(dividend) - deg(divisor)))
                polynomial multiplier =
                    shift_poly(std::move(lc_div), get_degree_poly(dividend) - get_degree_poly(divisor));
                polynomial subtrahend = mul_poly(divisor, multiplier);
                // quotient += multiplier
                quotient = add_poly(std::move(quotient), std::move(multiplier));
                // dividend = dividend - divisor * multiplier
                dividend = sub_poly(std::move(dividend), std::move(subtrahend));
            }
            remove_redundant_poly(quotient);
            return quotient;
        };

        /**
         * @brief 运用伪余数定理，计算伪余式
         * @param [in] dividend 被除数
         * @param [in] divisor 除数
         * @return polynomial 伪余式
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        polynomial get_pseudo_remainder_poly(polynomial dividend, const polynomial &divisor) {
            if (get_degree_poly(dividend) < get_degree_poly(divisor)) {
                return dividend;
            }
            polynomial lc_divisor;
            lc_divisor.push_back(get_leading_coefficient_poly(divisor));
            polynomial remainder = std::move(dividend);
            // polynomial quotient;
            while (get_degree_poly(remainder) >= get_degree_poly(divisor)) {
                // 算法原理：保持 lc_divisor^i(循环次数) * dividend(初始) = quotient(注释部分) * divisor + remainder
                auto       exp = get_degree_poly(remainder) - get_degree_poly(divisor);
                polynomial lc_dividend;
                lc_dividend.push_back(get_leading_coefficient_poly(remainder));
                // term1 = lc(divisor) * dividend
                polynomial term1 = mul_poly(remainder, lc_divisor);
                // term2 = lc(dividend) * x^exp * divisor
                polynomial q_term = shift_poly(std::move(lc_dividend), exp);
                polynomial term2 = mul_poly(divisor, q_term);
                // quotient = add_poly(poly_mul(divisor, quotient), std::move(q_term));
                remainder = sub_poly(std::move(term1), std::move(term2));
            }
            return remainder;
        };

        node_ptr polynomial_gcd(node_ptr root1, node_ptr root2);

        /**
         * @brief 获取多项式的内容部分
         * @param [in] poly 多项式
         * @return node_ptr 多项式的内容部分
         * @warning 需要提前调用 removeRedundant_poly 函数
         */
        node_ptr get_content_poly(polynomial poly) {
            std::optional<node_ptr> gcd_result;
            for (auto &child : poly) {
                gcd_result = gcd_result
                                 .transform([&](node_ptr &result) {
                                     return polynomial_gcd(std::move(result), std::move(child));
                                 })
                                 .or_else([&]() -> std::optional<node_ptr> { return std::move(child); });
            }
            return std::move(gcd_result).value_or(create_zero_node());
        };

        /**
         * @brief 计算多项式的最大公因式
         * @param [in] poly1 多项式1
         * @param [in] poly2 多项式2
         * @return polynomial 最大公因式
         * @warning 需要提前调用 removeRedundant_poly 函数
         * @details
         *  - 采用内容-本原部分分解法
         */
        polynomial polynomial_gcd(polynomial poly1, polynomial poly2) {
            // 特殊情况
            if (is_zero_poly(poly1)) {
                return poly2;
            }
            if (is_zero_poly(poly2)) {
                return poly1;
            }

            // 本原部分的 GCD（运用辗转相除法），需要提前调用 removeRedundant_poly 函数
            auto gcd_primitive = [](this auto &&gcd_primitive, polynomial poly1,
                                    polynomial poly2) -> polynomial {
                // 保证 degree(poly1) >= degree(poly2)
                if (get_degree_poly(poly1) < get_degree_poly(poly2)) {
                    std::swap(poly1, poly2);
                }

                // 递归终止条件：poly2 为0
                if (is_zero_poly(poly2)) {
                    return poly1;
                }

                // 递归终止条件：poly2 为常数
                if (get_degree_poly(poly2) == 0) {
                    // 此时 poly1 不可能为常数
                    TNRW_ASSERT_MSG(get_degree_poly(poly1) > 0, "猜想错误：poly1 实际上可能为常数");
                    // 返回 1
                    polynomial ret;
                    ret.push_back(make_node<monomial>(1));
                    return ret;
                }

                // 计算伪余数
                polynomial remainder = get_pseudo_remainder_poly(std::move(poly1), poly2);

                // 计算 remiander 的本原部分
                polynomial r_content;
                r_content.push_back(get_content_poly(clone_poly(remainder)));
                polynomial r_prim = div_poly(std::move(remainder), r_content);

                return gcd_primitive(std::move(poly2), std::move(r_prim));
            };

            // 计算内容和本原部分
            polynomial content1;
            content1.push_back(get_content_poly(clone_poly(poly1)));
            polynomial prim1 = div_poly(std::move(poly1), content1);
            polynomial content2;
            content2.push_back(get_content_poly(clone_poly(poly2)));
            polynomial prim2 = div_poly(std::move(poly2), content2);
            // std::println("gcd: divided");

            // 计算内容部分的 GCD
            polynomial gcd_content;
            gcd_content.push_back(polynomial_gcd(std::move(content1[0]), std::move(content2[0])));
            // std::println("gcd: computed content");

            // 计算本原部分的 GCD
            polynomial gcd_prim = gcd_primitive(std::move(prim1), std::move(prim2));
            // std::println("gcd: computed primitive");

            // 合并
            polynomial result = mul_poly(gcd_prim, gcd_content);
            // std::println("gcd: merged");

            return result;
        }

        /**
         * @brief 获取两个代数式公共的变量
         * @param [in] root1 代数式节点1
         * @param [in] root2 代数式节点2
         * @return std::optional<variable_view> 公共的变量，若没有则为0
         * @warning 这要求 root 节点被化简过
         */
        std::optional<variable_view> get_common_var(const node_ptr &root1, const node_ptr &root2) {
            std::optional<variable_view> main_var; //< 主元
            std::set<variable_view>      vars;     //< 变量集
            std::visit(make_overloaded(
                           [&](const monomial &value) {
                               for (const auto &[var, exp] : value.m_var_exps) { vars.insert(var); }
                           },
                           [&](const addition &value) {
                               for (const node_ptr &child : value.m_children) {
                                   const auto &value = std::get<monomial>(child->m_value);
                                   for (const auto &[var, exp] : value.m_var_exps) { vars.insert(var); }
                               }
                           },
                           [&](const multiplication & /*unused*/) {
                               unreachable("化简后的节点不应为 multiplication 节点");
                           },
                           [&](const division & /*unused*/) {
                               unreachable("多项式节点中不应有 division 节点");
                           }),
                       root1->m_value);

            std::visit(make_overloaded(
                           [&](const monomial &value) {
                               for (const auto &[var, exp] : value.m_var_exps) {
                                   if (vars.contains(var)) {
                                       main_var = var;
                                       return;
                                   }
                               }
                           },
                           [&](const addition &value) {
                               for (const node_ptr &child : value.m_children) {
                                   const auto &value = std::get<monomial>(child->m_value);
                                   for (const auto &[var, exp] : value.m_var_exps) {
                                       main_var = var;
                                       return;
                                   }
                               }
                           },
                           [&](const multiplication & /*unused*/) {
                               unreachable("化简后的节点不应为 multiplication 节点");
                           },
                           [&](const division & /*unused*/) {
                               unreachable("多项式节点中不应有 division 节点");
                           }),
                       root2->m_value);
            return main_var;
        }

        /**
         * @brief 提取代数式节点的整数因数
         * @param [in, out] root 代数式节点
         * @return integer_constant_type 提取的整数因数
         * @warning 这要求 root 节点被化简过
         * @warning 这要求 root 节点是多项式
         * @note 代数式节点将不会除去提取的因数
         */
        integer_constant_type extract_polynomial_integer_coefficient(const node_ptr &root) {
            return std::visit(
                make_overloaded([&](const monomial &value) { return std::abs(value.m_coeff); },
                                [&](const addition &value) {
                                    std::optional<integer_constant_type> coeff = std::nullopt;
                                    for (const node_ptr &child : value.m_children) {
                                        TNRW_ASSERT_MSG(std::holds_alternative<monomial>(child->m_value),
                                                        "猜想错误：child 实际上不一定为 monomial");
                                        auto &mono = std::get<monomial>(child->m_value);
                                        coeff =
                                            coeff
                                                .transform([&](integer_constant_type nested_coeff) {
                                                    return std::gcd(nested_coeff, mono.m_coeff);
                                                })
                                                .or_else([&] { return std::optional{mono.m_coeff}; });
                                    }
                                    return coeff.value_or(1);
                                },
                                [&](const multiplication & /*unused*/) -> integer_constant_type {
                                    unreachable("化简后的节点不应为 multiplication 节点");
                                    return 0;
                                },
                                [&](const division & /*unused*/) -> integer_constant_type {
                                    unreachable("多项式节点中不应有 division 节点");
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
        void divide_polynomial_with_integer_coefficient(node_ptr &root, integer_constant_type coeff) {
            std::visit(
                make_overloaded(
                    [&](monomial &value) { value.m_coeff /= coeff; },
                    [&](addition &value) {
                        for (const node_ptr &child : value.m_children) {
                            TNRW_ASSERT_MSG(std::holds_alternative<monomial>(child->m_value),
                                            "猜想错误：child 实际上不一定为 monomial");
                            auto &mono = std::get<monomial>(child->m_value);
                            mono.m_coeff /= coeff;
                        }
                    },
                    [&](multiplication & /*unused*/) {
                        unreachable("化简后的节点不应为 multiplication 节点");
                    },
                    [&](division & /*unused*/) { unreachable("多项式节点中不应有 division 节点"); }),
                root->m_value);
        }

        /**
         * @brief 计算代数式节点（需要为多项式）的最大公因式
         * @param [in] root1 代数式节点1
         * @param [in] root2 代数式节点2
         * @return node_ptr 最大公因式
         * @warning 这要求 root 节点被化简过
         */
        node_ptr polynomial_gcd(node_ptr root1, node_ptr root2) {
            // 特殊情况
            if (is_zero(root1)) {
                return root2;
            }
            if (is_zero(root2)) {
                return root1;
            }

            // 选择主元
            std::optional<variable_view> main_var = get_common_var(root1, root2); //< 主元

            return main_var
                .transform([&](variable_view nested_main_var) {
                    // 转换
                    auto       poly1 = to_poly(std::move(root1), nested_main_var);
                    auto       poly2 = to_poly(std::move(root2), nested_main_var);

                    // 计算
                    polynomial result = polynomial_gcd(std::move(poly1), std::move(poly2));

                    // 展平
                    return to_node_ptr(std::move(result), nested_main_var);
                })
                .or_else([&] {
                    // 没有相同的变量，返回
                    return std::optional{
                        make_node<monomial>(std::gcd(extract_polynomial_integer_coefficient(root1),
                                                     extract_polynomial_integer_coefficient(root2)))};
                })
                .value();
        }

        template <>
        void simplifier<division>::operator()(division &root_val, node_ptr &root) noexcept {
            // std::println("the raw division node is {}", node_to_string(root));
            node_ptr &num = root_val.m_children[0];
            node_ptr &den = root_val.m_children[1];
            simplify(num);
            simplify(den);

            // 保证为多项式
            if (std::holds_alternative<division>(num->m_value)) {
                auto &value = std::get<division>(num->m_value);
                den = make_node<multiplication>(std::move(den), std::move(value.m_children[1]));
                node_ptr tmp = std::move(value.m_children[0]);
                num = std::move(tmp);
                simplify(root);
                return;
            }
            if (std::holds_alternative<division>(den->m_value)) {
                auto &value = std::get<division>(den->m_value);
                num = make_node<multiplication>(std::move(den), std::move(value.m_children[1]));
                node_ptr tmp = std::move(value.m_children[0]);
                den = std::move(tmp);
                simplify(root);
                return;
            }
            if (std::holds_alternative<addition>(num->m_value)) {
                auto &value = std::get<addition>(num->m_value);
                for (auto &child : value.m_children) {
                    // 避免子节点非单项式
                    if (std::holds_alternative<division>(child->m_value)) {
                        auto    &child_value = std::get<division>(child->m_value);
                        node_ptr sub_den = std::move(child_value.m_children[1]);
                        node_ptr sub_num = std::move(child_value.m_children[0]);
                        std::swap(value.m_children.back(), child);
                        value.m_children.pop_back();
                        num = make_node<addition>(
                            make_node<multiplication>(std::move(num), std::make_unique<node>(*sub_den)),
                            std::move(sub_num));
                        den = make_node<multiplication>(std::move(den), std::move(sub_den));
                        simplify(root);
                        return;
                    }
                }
            }
            if (std::holds_alternative<addition>(den->m_value)) {
                auto &value = std::get<addition>(den->m_value);
                for (auto &child : value.m_children) {
                    // 避免子节点非单项式
                    if (std::holds_alternative<division>(child->m_value)) {
                        auto    &child_value = std::get<division>(child->m_value);
                        node_ptr sub_den = std::move(child_value.m_children[1]);
                        node_ptr sub_num = std::move(child_value.m_children[0]);
                        std::swap(value.m_children.back(), child);
                        value.m_children.pop_back();
                        den = make_node<addition>(
                            make_node<multiplication>(std::move(den), std::make_unique<node>(*sub_den)),
                            std::move(sub_num));
                        num = make_node<multiplication>(std::move(num), std::move(sub_den));
                        simplify(root);
                        return;
                    }
                }
            }

            // 特殊情况
            if (is_zero(num)) {
                root = create_zero_node();
                return;
            }

            // 摘自 polynomial_gcd 并修改

            // 选择主元
            std::optional<variable_view> main_var = get_common_var(num, den); //< 主元

            main_var
                .transform([&](variable_view nested_main_var) {
                    // 转换
                    auto       poly1 = to_poly(std::move(num), nested_main_var);
                    auto       poly2 = to_poly(std::move(den), nested_main_var);
                    // std::println("converted");

                    // 计算
                    polynomial result = polynomial_gcd(clone_poly(poly1), clone_poly(poly2));
                    // std::println("computed");

                    poly1 = div_poly(std::move(poly1), result);
                    poly2 = div_poly(std::move(poly2), result);
                    // std::println("divided");

                    // 展平
                    num = to_node_ptr(std::move(poly1), nested_main_var);
                    den = to_node_ptr(std::move(poly2), nested_main_var);
                    // std::println("reconverted");

                    return nested_main_var;
                })
                .or_else([&] -> std::optional<variable_view> {
                    // 没有相同的变量，除掉共同变量
                    integer_constant_type gcdans = std::gcd(extract_polynomial_integer_coefficient(num),
                                                            extract_polynomial_integer_coefficient(den));
                    divide_polynomial_with_integer_coefficient(num, gcdans);
                    divide_polynomial_with_integer_coefficient(den, gcdans);
                    return {};
                });

            if (std::holds_alternative<monomial>(den->m_value)) {
                auto &den_value = std::get<monomial>(den->m_value);
                // 消除分母的负数
                if (den_value.m_coeff < 0) {
                    den_value.m_coeff = -den_value.m_coeff;
                    num = make_node<multiplication>(make_node<monomial>(-1), std::move(num));
                    simplify(num);
                }
                // 消除无意义的分母
                if (den_value.m_coeff == 1 && den_value.m_var_exps.empty()) {
                    node_ptr tmp = std::move(num);
                    root = std::move(tmp);
                }
            }

            // std::println("the simpilfied division node is {}", node_to_string(root));
        }

        void simplify(node_ptr &root) {
            std::visit(
                [&](auto &value) {
                    using type = std::decay_t<decltype(value)>;
                    simplifier<type>()(value, root);
                },
                root->m_value);
        }

        /**
         * @brief 代数式加/减法的内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 常量
         */
        void plus(node_ptr &lhs, integer_constant_type rhs) {
            lhs = make_node<addition>(std::move(lhs), make_node<monomial>(rhs));
            simplify(lhs);
        }
        /**
         * @brief 代数式加法的内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 变量
         */
        void plus(node_ptr &lhs, variable_view rhs) {
            lhs = make_node<addition>(std::move(lhs),
                                      make_node<monomial>(1, monomial::map_var_exp{{rhs, 1}}));
            simplify(lhs);
        }
        /**
         * @brief 代数式减法内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 变量
         */
        void minus(node_ptr &lhs, variable_view rhs) {
            lhs = make_node<addition>(std::move(lhs),
                                      make_node<monomial>(-1, monomial::map_var_exp{{rhs, 1}}));
            simplify(lhs);
        }
        /**
         * @brief 代数式乘法的内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 常量
         */
        void multiply(node_ptr &lhs, integer_constant_type rhs) {
            lhs = make_node<multiplication>(std::move(lhs), make_node<monomial>(rhs));
            simplify(lhs);
        }
        /**
         * @brief 代数式乘法的内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 变量
         */
        void multiply(node_ptr &lhs, variable_view rhs) {
            lhs = make_node<multiplication>(std::move(lhs),
                                            make_node<monomial>(1, monomial::map_var_exp{{rhs, 1}}));
            simplify(lhs);
        }
        /**
         * @brief 代数式除法的内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 常量
         */
        void divide(node_ptr &lhs, integer_constant_type rhs) {
            lhs = make_node<division>(std::move(lhs), make_node<monomial>(rhs));
            simplify(lhs);
        }
        /**
         * @brief 代数式除法的内部实现
         * @param [in] lhs 代数式
         * @param [in] rhs 变量
         */
        void divide(node_ptr &lhs, variable_view rhs) {
            lhs = make_node<division>(std::move(lhs),
                                      make_node<monomial>(1, monomial::map_var_exp{{rhs, 1}}));
            simplify(lhs);
        }
    } // namespace

} // namespace tnrw::math

/// @endcond

namespace tnrw {
    namespace math {

        // algebraic_expression类的成员定义
        [[nodiscard]] algebraic_expression::algebraic_expression() noexcept
            : m_root(create_zero_node()) {}

        [[nodiscard]] algebraic_expression::algebraic_expression(integer_constant_type constant) noexcept
            : m_root(make_node<monomial>(constant)) {}

        [[nodiscard]] algebraic_expression::algebraic_expression(variable_view variable) noexcept
            : m_root(
                  make_node<monomial>(1, monomial::map_var_exp{{*m_vars.emplace(variable).first, 1}})) {}
        algebraic_expression::algebraic_expression(numeric_expression num_expr) noexcept
            : m_root(std::move(num_expr.m_root)) {}

        [[nodiscard]] algebraic_expression::algebraic_expression(
            const algebraic_expression &rhs) noexcept
            : m_root(std::make_unique<node>(*rhs.m_root)) {}

        [[nodiscard]] algebraic_expression::algebraic_expression(algebraic_expression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

        algebraic_expression &
        algebraic_expression::operator=(const integer_constant_type &rhs) & noexcept {
            m_root = make_node<monomial>(rhs);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator=(variable_view rhs) & noexcept {
            variable_view var = *m_vars.emplace(rhs).first;
            m_root = make_node<monomial>(1, monomial::map_var_exp{{var, 1}});
            return *this;
        }

        algebraic_expression &
        algebraic_expression::operator=(const algebraic_expression &rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::make_unique<node>(*rhs.m_root);
            }
            return *this;
        }

        algebraic_expression &algebraic_expression::operator=(algebraic_expression &&rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::move(rhs.m_root);
            }
            return *this;
        }

        algebraic_expression::~algebraic_expression() noexcept = default;

        algebraic_expression &algebraic_expression::operator+=(integer_constant_type rhs) & noexcept {
            plus(m_root, rhs);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator+=(variable_view rhs) & noexcept {
            variable_view var = *m_vars.emplace(rhs).first;
            plus(m_root, var);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator+=(algebraic_expression rhs) & noexcept {
            m_root = make_node<addition>(std::move(m_root), std::move(rhs.m_root));
            simplify(m_root);
            return *this;
        }

        algebraic_expression &algebraic_expression::operator-=(integer_constant_type rhs) & noexcept {
            plus(m_root, -rhs);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator-=(variable_view rhs) & noexcept {
            variable_view var = *m_vars.emplace(rhs).first;
            minus(m_root, var);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator-=(algebraic_expression rhs) & noexcept {
            m_root =
                make_node<addition>(std::move(m_root), make_node<multiplication>(make_node<monomial>(-1),
                                                                                 std::move(rhs.m_root)));
            simplify(m_root);
            return *this;
        }

        algebraic_expression &algebraic_expression::operator*=(integer_constant_type rhs) & noexcept {
            multiply(m_root, rhs);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator*=(variable_view rhs) & noexcept {
            variable_view var = *m_vars.emplace(rhs).first;
            multiply(m_root, var);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator*=(algebraic_expression rhs) & noexcept {
            m_root = make_node<multiplication>(std::move(m_root), std::move(rhs.m_root));
            simplify(m_root);
            return *this;
        }

        algebraic_expression &algebraic_expression::operator/=(integer_constant_type rhs) & noexcept {
            divide(m_root, rhs);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator/=(variable_view rhs) & noexcept {
            variable_view var = *m_vars.emplace(rhs).first;
            divide(m_root, var);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator/=(algebraic_expression rhs) & noexcept {
            m_root = make_node<division>(std::move(m_root), std::move(rhs.m_root));
            simplify(m_root);
            return *this;
        }

        [[nodiscard]] algebraic_expression algebraic_expression::operator+() const noexcept {
            return *this;
        }

        [[nodiscard]] algebraic_expression algebraic_expression::operator-() const noexcept {
            algebraic_expression copy_of_this(*this);
            copy_of_this.change_to_opposite();
            return copy_of_this;
        }

        algebraic_expression &algebraic_expression::operator++() noexcept {
            plus(m_root, 1);
            return *this;
        }
        algebraic_expression &algebraic_expression::operator--() noexcept {
            plus(m_root, -1);
            return *this;
        }

        [[nodiscard]] algebraic_expression algebraic_expression::operator++(int) noexcept {
            algebraic_expression copy_of_old(*this);
            plus(m_root, 1);
            return copy_of_old;
        }
        [[nodiscard]] algebraic_expression algebraic_expression::operator--(int) noexcept {
            algebraic_expression copy_of_old(*this);
            plus(m_root, -1);
            return copy_of_old;
        }

        [[nodiscard]] const node_ptr &algebraic_expression::get_pointer() const & noexcept {
            return m_root;
        }
        [[nodiscard]] node_ptr algebraic_expression::get_pointer() && noexcept {
            return std::move(m_root);
        }

        template <std::floating_point FloatT>
        [[nodiscard]] FloatT algebraic_expression::calculate_approximation(
            const std::function<FloatT(variable_view)> &converter) const noexcept {
            return math::calculate_approximation<FloatT>(m_root, converter);
        }

        [[nodiscard]] numeric_expression algebraic_expression::calculate(
            const std::function<numeric_expression(variable_view)> &converter) const noexcept {
            node_ptr result =
                math::calculate(m_root, [&](variable_view var) { return converter(var).get_pointer(); });
            simplify(result);
            numeric_expression ret;
            ret.m_root = std::move(result);
            return ret;
        }

        void                      algebraic_expression::clear() noexcept { m_root = create_zero_node(); }

        [[nodiscard]] std::string algebraic_expression::to_string() const noexcept {
            return node_to_string(m_root);
        }

        [[nodiscard]] std::wstring algebraic_expression::to_wstring() const noexcept {
            return std::format(L"{}", *this);
        }

        void               algebraic_expression::change_to_opposite() noexcept { multiply(m_root, -1); }

        [[nodiscard]] bool algebraic_expression::has_variable() const noexcept {
            if (m_vars.empty()) {
                return false;
            }
            return math::has_variable(m_root);
        }

        [[nodiscard]] bool algebraic_expression::has_variable(variable_view variable) const noexcept {
            if (!m_vars.contains(variable)) {
                return false;
            }
            return math::has_variable(m_root, variable);
        }
        [[nodiscard]] std::optional<numeric_expression>
        algebraic_expression::to_numeric_expression() const & noexcept {
            if (has_variable()) {
                return std::nullopt;
            }
            numeric_expression ret;
            ret.m_root = std::make_unique<node>(*m_root);
            return ret;
        }
        [[nodiscard]] std::optional<numeric_expression>
        algebraic_expression::to_numeric_expression() && noexcept {
            if (has_variable()) {
                return std::nullopt;
            }
            numeric_expression ret;
            ret.m_root = std::move(m_root);
            return ret;
        }

        [[nodiscard]] algebraic_expression operator+(const algebraic_expression &lhs,
                                                     const integer_constant_type rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy += rhs;
        }
        [[nodiscard]] algebraic_expression operator+(const integer_constant_type lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(rhs);
            return cpy += lhs;
        }
        [[nodiscard]] algebraic_expression operator+(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy += rhs;
        }
        [[nodiscard]] algebraic_expression operator+(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(rhs);
            return cpy += lhs;
        }
        [[nodiscard]] algebraic_expression operator+(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy += rhs;
        }

        [[nodiscard]] algebraic_expression operator-(const algebraic_expression &lhs,
                                                     const integer_constant_type rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] algebraic_expression operator-(const integer_constant_type lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(-rhs);
            return cpy += lhs;
        }
        [[nodiscard]] algebraic_expression operator-(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] algebraic_expression operator-(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(-rhs);
            return cpy += lhs;
        }
        [[nodiscard]] algebraic_expression operator-(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy -= rhs;
        }

        [[nodiscard]] algebraic_expression operator*(const algebraic_expression &lhs,
                                                     const integer_constant_type rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy *= rhs;
        }
        [[nodiscard]] algebraic_expression operator*(const integer_constant_type lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(rhs);
            return cpy *= lhs;
        }
        [[nodiscard]] algebraic_expression operator*(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy *= rhs;
        }
        [[nodiscard]] algebraic_expression operator*(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(rhs);
            return cpy *= lhs;
        }
        [[nodiscard]] algebraic_expression operator*(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy *= rhs;
        }

        [[nodiscard]] algebraic_expression operator/(const algebraic_expression &lhs,
                                                     const integer_constant_type rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] algebraic_expression operator/(const integer_constant_type lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] algebraic_expression operator/(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] algebraic_expression operator/(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] algebraic_expression operator/(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept {
            algebraic_expression cpy(lhs);
            return cpy /= rhs;
        }

        [[nodiscard]] bool operator==(const algebraic_expression &lhs,
                                      const algebraic_expression &rhs) noexcept {
            if (&lhs == &rhs) {
                return true;
            }
            return is_equal(lhs.m_root, rhs.m_root);
        }
        [[nodiscard]] bool operator!=(const algebraic_expression &lhs,
                                      const algebraic_expression &rhs) noexcept {
            if (&lhs == &rhs) {
                return false;
            }
            return !is_equal(lhs.m_root, rhs.m_root);
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const algebraic_expression        &rhs) noexcept {
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

        // numeric_expression类的成员定义
        [[nodiscard]] numeric_expression::numeric_expression() noexcept : m_root(create_zero_node()) {}

        [[nodiscard]] numeric_expression::numeric_expression(
            const integer_constant_type constant) noexcept
            : m_root(make_node<monomial>(constant)) {}

        numeric_expression::~numeric_expression() noexcept = default;

        [[nodiscard]] numeric_expression::numeric_expression(const numeric_expression &rhs) noexcept
            : m_root(std::make_unique<node>(*rhs.m_root)) {}

        [[nodiscard]] numeric_expression::numeric_expression(numeric_expression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

        numeric_expression &numeric_expression::operator=(const numeric_expression &rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::make_unique<node>(*rhs.m_root);
            }
            return *this;
        }

        numeric_expression &numeric_expression::operator=(numeric_expression &&rhs) & noexcept {
            if (this != &rhs) {
                m_root = std::move(rhs.m_root);
            }
            return *this;
        }

        numeric_expression &numeric_expression::operator+=(const integer_constant_type rhs) & noexcept {
            plus(m_root, rhs);
            return *this;
        }
        numeric_expression &numeric_expression::operator+=(const numeric_expression &rhs) & noexcept {
            m_root = make_node<addition>(std::move(m_root), std::make_unique<node>(*rhs.m_root));
            simplify(m_root);
            return *this;
        }

        numeric_expression &numeric_expression::operator-=(const integer_constant_type rhs) & noexcept {
            plus(m_root, -rhs);
            return *this;
        }
        numeric_expression &numeric_expression::operator-=(const numeric_expression &rhs) & noexcept {
            m_root = make_node<addition>(
                std::move(m_root),
                make_node<multiplication>(make_node<monomial>(-1), std::make_unique<node>(*rhs.m_root)));
            simplify(m_root);
            return *this;
        }

        numeric_expression &numeric_expression::operator*=(const integer_constant_type rhs) & noexcept {
            multiply(m_root, rhs);
            return *this;
        }
        numeric_expression &numeric_expression::operator*=(const numeric_expression &rhs) & noexcept {
            m_root = make_node<multiplication>(std::move(m_root), std::make_unique<node>(*rhs.m_root));
            simplify(m_root);
            return *this;
        }

        numeric_expression &numeric_expression::operator/=(const integer_constant_type rhs) & noexcept {
            divide(m_root, rhs);
            return *this;
        }
        numeric_expression &numeric_expression::operator/=(const numeric_expression &rhs) & noexcept {
            m_root = make_node<division>(std::move(m_root), std::make_unique<node>(*rhs.m_root));
            simplify(m_root);
            return *this;
        }

        [[nodiscard]] numeric_expression numeric_expression::operator+() const noexcept { return *this; }

        [[nodiscard]] numeric_expression numeric_expression::operator-() const noexcept {
            numeric_expression copy_of_this(*this);
            copy_of_this.change_to_opposite();
            return copy_of_this;
        }

        numeric_expression &numeric_expression::operator++() noexcept {
            plus(m_root, 1);
            return *this;
        }
        numeric_expression &numeric_expression::operator--() noexcept {
            plus(m_root, -1);
            return *this;
        }

        [[nodiscard]] numeric_expression numeric_expression::operator++(int) noexcept {
            numeric_expression copy_of_old(*this);
            plus(m_root, 1);
            return copy_of_old;
        }
        [[nodiscard]] numeric_expression numeric_expression::operator--(int) noexcept {
            numeric_expression copy_of_old(*this);
            plus(m_root, -1);
            return copy_of_old;
        }

        [[nodiscard]] const node_ptr &numeric_expression::get_pointer() const & noexcept {
            return m_root;
        }
        [[nodiscard]] node_ptr numeric_expression::get_pointer() && noexcept {
            return std::move(m_root);
        }
        template <std::floating_point FloatT>
        [[nodiscard]] FloatT numeric_expression::calculate_approximation() const noexcept {
            return math::calculate_approximation<FloatT>(m_root, [](variable_view /*unused*/) -> FloatT {
                unreachable("numeric_expression 不应该有 Variable 节点");
            });
        }

        void                      numeric_expression::clear() noexcept { m_root = create_zero_node(); }

        [[nodiscard]] std::string numeric_expression::to_string() const noexcept {
            return node_to_string(m_root);
        }

        [[nodiscard]] std::wstring numeric_expression::to_wstring() const noexcept {
            return std::format(L"{}", *this);
        }

        void numeric_expression::change_to_opposite() noexcept { multiply(m_root, -1); }

        [[nodiscard]] numeric_expression operator+(const numeric_expression   &lhs,
                                                   const integer_constant_type rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy += rhs;
        }
        [[nodiscard]] numeric_expression operator+(const integer_constant_type lhs,
                                                   const numeric_expression   &rhs) noexcept {
            numeric_expression cpy(rhs);
            return cpy += lhs;
        }
        [[nodiscard]] numeric_expression operator+(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy += rhs;
        }

        [[nodiscard]] numeric_expression operator-(const numeric_expression   &lhs,
                                                   const integer_constant_type rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy -= rhs;
        }
        [[nodiscard]] numeric_expression operator-(const integer_constant_type lhs,
                                                   const numeric_expression   &rhs) noexcept {
            numeric_expression cpy(rhs);
            return cpy -= lhs;
        }
        [[nodiscard]] numeric_expression operator-(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy -= rhs;
        }

        [[nodiscard]] numeric_expression operator*(const numeric_expression   &lhs,
                                                   const integer_constant_type rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy *= rhs;
        }
        [[nodiscard]] numeric_expression operator*(const integer_constant_type lhs,
                                                   const numeric_expression   &rhs) noexcept {
            numeric_expression cpy(rhs);
            return cpy *= lhs;
        }
        [[nodiscard]] numeric_expression operator*(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy *= rhs;
        }

        [[nodiscard]] numeric_expression operator/(const numeric_expression   &lhs,
                                                   const integer_constant_type rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy /= rhs;
        }
        [[nodiscard]] numeric_expression operator/(const integer_constant_type lhs,
                                                   const numeric_expression   &rhs) noexcept {
            numeric_expression cpy(rhs);
            return cpy /= lhs;
        }
        [[nodiscard]] numeric_expression operator/(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept {
            numeric_expression cpy(lhs);
            return cpy /= rhs;
        }

        [[nodiscard]] bool operator==(const numeric_expression &lhs,
                                      const numeric_expression &rhs) noexcept {
            if (&lhs == &rhs) {
                return true;
            }
            return is_equal(lhs.m_root, rhs.m_root);
        }
        [[nodiscard]] bool operator!=(const numeric_expression &lhs,
                                      const numeric_expression &rhs) noexcept {
            if (&lhs == &rhs) {
                return false;
            }
            return !is_equal(lhs.m_root, rhs.m_root);
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const numeric_expression          &rhs) noexcept {
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
        template std::basic_ostream<char> &operator<< <char>(std::basic_ostream<char>   &out,
                                                             const algebraic_expression &rhs) noexcept;
        template std::basic_ostream<wchar_t> &
        operator<< <wchar_t>(std::basic_ostream<wchar_t> &out, const algebraic_expression &rhs) noexcept;
        template float algebraic_expression::calculate_approximation<float>(
            const std::function<float(variable_view)> &converter) const noexcept;
        template double algebraic_expression::calculate_approximation<double>(
            const std::function<double(variable_view)> &converter) const noexcept;
        template long double algebraic_expression::calculate_approximation<long double>(
            const std::function<long double(variable_view)> &converter) const noexcept;

        template std::basic_ostream<char> &operator<< <char>(std::basic_ostream<char> &out,
                                                             const numeric_expression &rhs) noexcept;
        template std::basic_ostream<wchar_t> &
        operator<< <wchar_t>(std::basic_ostream<wchar_t> &out, const numeric_expression &rhs) noexcept;
        template float       numeric_expression::calculate_approximation<float>() const noexcept;
        template double      numeric_expression::calculate_approximation<double>() const noexcept;
        template long double numeric_expression::calculate_approximation<long double>() const noexcept;

    } // namespace math

    inline namespace literals {

        inline namespace expressions_base_literals {
            [[nodiscard]] constexpr math::integer_constant_type
            operator""_c(unsigned long long constant) noexcept {
                return static_cast<math::integer_constant_type>(constant);
            }
            [[nodiscard]] math::variable_type operator""_v(const char *variable,
                                                           std::size_t len) noexcept {
                return {variable, len};
            }
            [[nodiscard]] math::variable_view operator""_vv(const char *variable,
                                                            std::size_t len) noexcept {
                return {variable, len};
            }
        } // namespace expressions_base_literals

        inline namespace algebraic_expression_literals {

            [[nodiscard]] math::algebraic_expression
            operator""_cAlgeExpr(const unsigned long long constant) noexcept {
                return math::algebraic_expression{operator""_c(constant)};
            }
            [[nodiscard]] math::algebraic_expression operator""_vAlgeExpr(const char *variable,
                                                                          std::size_t len) noexcept {
                return math::algebraic_expression{operator""_vv(variable, len)};
            }

        } // namespace algebraic_expression_literals

        inline namespace numeric_expression_literals {

            [[nodiscard]] math::numeric_expression
            operator""_cNumExpr(const unsigned long long constant) noexcept {
                return math::numeric_expression{operator""_c(constant)};
            }

        } // namespace numeric_expression_literals

    } // namespace literals

} // namespace tnrw
