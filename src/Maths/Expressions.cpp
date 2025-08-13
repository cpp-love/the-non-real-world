/**
 * @file Expressions.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了代数式和无字母的代数式类的细节
 * @version 0.1.0-1
 * @date 2025-07-05
 * 
 * @copyright cpp-love
 * 
 */

#include "Maths/AlgebraicExpression.hpp"
#include "Maths/Expressions_base.hpp"
#include "Maths/NumericExpression.hpp"
#include "Maths/functions.hpp"
#include "base/assert_msg.hpp"
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace tnrw {

    namespace Maths {

        /// @cond INTERNAL

        /// @brief `tnrw::Maths` 命名空间的一些功能的实现细节
        namespace Details {

            /**
             * @brief 代数式树的节点
             * @details
             * - 节点可以是常量、变量或运算符
             * - 此结构体为代数式类( @ref tnrw::Maths::AlgebraicExpression )和无字母的代数式类( @ref tnrw::Maths::NumericExpression )的实现
             * - **维护说明：（原则：尽量化简）**
             *   - **定义：**
             *     - 定义一个 **根节点为常量、变量或乘/除法运算符** 的树状代数式为 **`树状单项式`**
             *     - 定义一个 **根节点为加法** ， 根节点的 **子节点均为树状单项式** 为 **`树状多项式`** ，
             *                                  每个子节点称为 **`树状多项式的项`**
             *       **注意：** `树状单项式` **并非** 数学上的单项式，它还涵盖了数学上的部分 **分式（暂无根式，可能在未来加入）** 
             *            同理：`树状单项式` 也 **并非** 数学上的多项式
             *   - **维护：**
             *     - **`树状单项式` ：**
             *       1. 若有分母，应保持根节点为除法运算符
             *          1. 分子 **不可以** 有加法运算符，而且分子分母均 **不可以** 有除法运算符
             *          2. 应始终保持分数为 **最简形式** ，即应始终 **约分**
             *          3. 分子分母应始终 **将每个乘法运算符节点中的常量孩子节点置于节点列表的末尾**
             *          4. 分子在第0位，分母在第1位，不应有其他位
             *          5. 分子分母内的成员的 `m_type` 应始终 **没有位标志 `TypeIndex::Negative`**
             *       2. 若无分母，应保持根节点为乘法运算符
             *          1. 其内 **不可以** 有加法运算符
             *          2. 其内应始终 **将每个乘法运算符节点中的常量孩子节点置于节点列表的末尾**
             *          3. 其内的成员的 `m_type` 应始终 **没有位标志 `TypeIndex::Negative`**
             *     - **`树状多项式` ：**
             *       1. 应始终 **合并同类项** （部分变量相同、分子或分母相同的不用合并）
             *       2. 应始终 **将其常量孩子节点置于节点列表的末尾**
             *     - **常量应始终没有位标志 `TypeIndex::Negative`**
             *     - **如果为分数常量，则符号应在分母上**
             * @warning 该结构体是私有的，用户不应直接访问
             */
            struct Node {
                // using别名
                using ConstantValue = ConstantType;
                using VariableValue = VariableType;
                using Ptr = std::unique_ptr<Node>;

                /// @brief 节点类型索引枚举
                enum class TypeIndex : std::uint8_t {
                    Negative = 0, ///< 是否有额外的负号
                    Constant = 1, ///< 常量
                    Variable = 2, ///< 变量
                    Operator = 3  ///< 运算符
                };

                [[nodiscard]] static inline std::size_t toSize(const TypeIndex t) noexcept {
                    return static_cast<std::size_t>(t);
                }

                std::bitset<8> m_type; ///< 节点类型

                /// @brief 运算符值
                struct OperatorValue {
                    /// @brief 运算符类型枚举
                    enum class OperatorType : std::uint8_t {
                        Addition, ///< 加法
                        // Subtraction,    ///< 减法
                        //< 它可以被移除
                        Multiplication, ///< 乘法
                        Division,       ///< 除法
                        // Exponentiation, ///< 乘方/幂
                        // Extraction ///< 开方
                        //< 计划在未来加入
                    };
                    OperatorType m_op_type; ///< 运算符

                    // using 别名
                    using ChildrenType = std::vector<Node::Ptr>;
                    ChildrenType m_children; ///< 孩子节点

                    // 构造、赋值、析构
                    /// @brief 禁止默认构造
                    /// @warning 禁止默认构造
                    OperatorValue() = delete;
                    /**
                     * @brief 以运算符类型和孩子节点为参数的构造函数
                     * @param [in] type 运算符类型
                     * @param [in] children 孩子节点
                     */
                    [[nodiscard]] explicit OperatorValue(OperatorType   type,
                                                         ChildrenType &&children) noexcept
                        : m_op_type(type), m_children(std::move(children)) {}
                    /// @brief 析构函数
                    ~OperatorValue() = default;
                    /**
                     * @brief 深复制构造函数
                     * @param [in] rhs 另一个对象
                     */
                    [[nodiscard]] OperatorValue(const OperatorValue &rhs) noexcept
                        : m_op_type(rhs.m_op_type) {
                        m_children.reserve(rhs.m_children.size());
                        for (const auto &child : rhs.m_children) {
                            m_children.push_back(std::make_unique<Node>(*child));
                        }
                    }
                };

                // using别名
                using ValueType = std::variant<ConstantValue, VariableValue, OperatorValue>;

                ValueType m_value; // 节点值

                // 构造、赋值、析构
                /// @brief 禁止默认构造
                /// @warning 禁止默认构造
                Node() = delete;
                /**
                 * @brief 以常量为参数的构造函数
                 * @param [in] value 常量值
                 * @param [in] is_negative 是否为负
                 */
                [[nodiscard]] explicit Node(const ConstantValue value) noexcept
                    : m_type(), m_value(value) {
                    m_type.set(Node::toSize(TypeIndex::Constant));
                }
                /**
                 * @brief 以变量为参数的构造函数
                 * @param [in] value 变量字符
                 * @param [in] is_negative 是否为负
                 */
                [[nodiscard]] explicit Node(const VariableValue value,
                                            const bool          is_negative = false) noexcept
                    : m_type(), m_value(value) {
                    m_type.set(Node::toSize(TypeIndex::Variable));
                    if (is_negative) {
                        m_type.set(Node::toSize(TypeIndex::Negative));
                    }
                }
                /**
                 * @brief 以运算符为参数的构造函数
                 * @param [in] value 运算符
                 * @param [in] is_negative 是否为负
                 */
                [[nodiscard]] explicit Node(const OperatorValue &value,
                                            const bool           is_negative = false) noexcept
                    : m_type(), m_value(value) {
                    m_type.set(Node::toSize(TypeIndex::Operator));
                    if (is_negative) {
                        m_type.set(Node::toSize(TypeIndex::Negative));
                    }
                }
                /**
                 * @brief 带其他参数的复制构造函数
                 * @param [in] value 另一个 `Node` 对象
                 * @param [in] is_negative 是否为负
                 */
                [[nodiscard]] explicit Node(const Node &value, const bool is_negative) noexcept
                    : m_type(value.m_type), m_value(value.m_value) {
                    if (is_negative) {
                        m_type.set(Node::toSize(TypeIndex::Negative));
                    }
                }
                /**
                 * @brief 带其他参数的移动构造函数
                 * @param [in] value 另一个 `Node` 对象
                 * @param [in] is_negative 是否为负
                 */
                [[nodiscard]] explicit Node(const Node &&value, const bool is_negative) noexcept
                    : m_type(std::move(value.m_type)), m_value(std::move(value.m_value)) {
                    if (is_negative) {
                        m_type.set(Node::toSize(TypeIndex::Negative));
                    }
                }
                /**
                 * @brief 析构函数
                 * @details 释放代数式资源
                 */
                ~Node() noexcept = default;
            };

            /**
             * @brief 清空节点
             * @param [in] root 根节点
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            inline void clearNode(Node::Ptr &root) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                using namespace tnrw::literals::Expressions_base_literals;
                root = std::make_unique<Node>(0_c);
            }

            template <typename T>
            concept IsNodePtr = requires(const T &v) { std::is_same_v<std::decay_t<T>, Node::Ptr>; };

            /**
             * @brief 构建加法运算符节点
             * @tparam Args 孩子类型（必须均为 `Node::Ptr`类型的 ）
             * @param [in] is_negative 是否为负
             * @param [in] args 孩子
             * @return Node::Ptr 新建的节点
             * @warning 孩子类型必须均为 `Node::Ptr` ，否则将报错！
             */
            template <IsNodePtr... Args>
            [[nodiscard]] inline Node::Ptr createAdditionNode(bool is_negative, Args... args) noexcept {
                Node::OperatorValue::ChildrenType children;
                children.reserve(sizeof...(args));
                (children.push_back(std::move(args)), ...);
                return std::make_unique<Node>(
                    Node::OperatorValue(Node::OperatorValue::OperatorType::Addition,
                                        std::move(children)),
                    is_negative);
            }
            /**
             * @brief 构建乘法运算符节点
             * @tparam Args 孩子类型（必须均为 `Node::Ptr`类型的 ）
             * @param [in] is_negative 是否为负
             * @param [in] args 孩子
             * @return Node::Ptr 新建的节点
             * @warning 孩子类型必须均为 `Node::Ptr` ，否则将报错！
             */
            template <IsNodePtr... Args>
            [[nodiscard]] inline Node::Ptr createMultiplicationNode(bool is_negative,
                                                                    Args... args) noexcept {
                Node::OperatorValue::ChildrenType children;
                children.reserve(sizeof...(args));
                (children.push_back(std::move(args)), ...);
                return std::make_unique<Node>(
                    Node::OperatorValue(Node::OperatorValue::OperatorType::Multiplication,
                                        std::move(children)),
                    is_negative);
            }
            /**
             * @brief 构建除法运算符节点
             * @tparam Args 孩子类型（必须均为 `Node::Ptr`类型的 ）
             * @param [in] is_negative 是否为负
             * @param [in] args 孩子
             * @return Node::Ptr 新建的节点
             * @warning 孩子类型必须均为 `Node::Ptr` ，否则将报错！
             */
            template <IsNodePtr... Args>
            [[nodiscard]] inline Node::Ptr createDivisionNode(bool is_negative, Args... args) noexcept {
                Node::OperatorValue::ChildrenType children;
                children.reserve(sizeof...(args));
                (children.push_back(std::move(args)), ...);
                return std::make_unique<Node>(
                    Node::OperatorValue(Node::OperatorValue::OperatorType::Division,
                                        std::move(children)),
                    is_negative);
            }
            /**
             * @brief 将孩子节点移动到根节点
             * @param [in] root 根节点
             * @param [in] child 孩子节点
             * @details 
             * - 将孩子节点移动到临时变量，再移动到根节点，避免了移动到根节点时根节点释放掉子节点导致的悬垂指针问题
             * - 此函数保留原来的负属性并叠加到孩子节点
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            inline void moveChildToRoot(Node::Ptr &root, Node::Ptr &child) {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                bool is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                auto new_child = std::move(child);
                root = std::move(new_child);
                // 保留原来的负属性
                root->m_type[Node::toSize(Node::TypeIndex::Negative)] =
                    root->m_type[Node::toSize(Node::TypeIndex::Negative)] ^ is_nega;
            }
            /**
             * @brief 将代数式转为 `std::basic_string`
             * @tparam CharT `std::basic_string` 的模板参数1
             * @tparam Traits `std::basic_string` 的模板参数2
             * @param [in] root 根节点
             * @param [in] loc 可能的 `std::locale` 配置
             * @return std::basic_string<CharT, Traits> 人类可读的字符串
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            template <typename CharT, typename Traits>
            [[nodiscard]] inline std::basic_string<CharT, Traits>
            toBasicStringFromNode(const Node::Ptr &root, const std::locale &loc) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                return std::visit(
                    [&root, &loc](auto &val) -> std::basic_string<CharT, Traits> {
                        using T = std::decay_t<decltype(val)>;

                        const auto &conv = std::use_facet<std::ctype<CharT>>(loc);

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 避免重复赋值
                            if constexpr (std::is_same_v<std::basic_string<CharT, Traits>,
                                                         std::string>) {
                                return std::to_string(val);
                            } else if constexpr (std::is_same_v<std::basic_string<CharT, Traits>,
                                                                std::wstring>) {
                                return std::to_wstring(val);
                            } else {
                                std::string                      tmp = std::to_string(val);
                                std::basic_string<CharT, Traits> res;
                                res.reserve(tmp.size());
                                for (const char ch : tmp) { res += conv.widen(ch); }
                                return res;
                            }
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)])
                                return conv.widen('-') + std::basic_string<CharT, Traits>(1, val);
                            else
                                return std::basic_string<CharT, Traits>(1, val);
                        } else {
                            std::basic_string<CharT, Traits> s;
                            s.reserve(10);
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                s += conv.widen('-');
                                s += conv.widen('(');
                            }
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Division) {
                                bool flag0 =
                                    (val.m_children[0]->m_type[Node::toSize(Node::TypeIndex::Operator)]);
                                bool flag1 =
                                    (val.m_children[1]->m_type[Node::toSize(Node::TypeIndex::Operator)]);
                                if (flag0) {
                                    s += conv.widen('(');
                                }
                                s += toBasicStringFromNode<CharT, Traits>(val.m_children[0], loc);
                                if (flag0) {
                                    s += conv.widen(')');
                                }
                                s += conv.widen(' ');
                                s += conv.widen('/');
                                s += conv.widen(' ');
                                if (flag1) {
                                    s += conv.widen('(');
                                }
                                s += toBasicStringFromNode<CharT, Traits>(val.m_children[1], loc);
                                if (flag1) {
                                    s += conv.widen(')');
                                }
                            } else {
                                std::basic_string<CharT, Traits> op;
                                op.reserve(3);
                                switch (val.m_op_type) {
                                    case Node::OperatorValue::OperatorType::Addition:
                                        op = conv.widen(' ');
                                        op += conv.widen('+');
                                        op += conv.widen(' ');
                                        break;
                                    case Node::OperatorValue::OperatorType::Multiplication:
                                        op = conv.widen(' ');
                                        op += conv.widen('*');
                                        op += conv.widen(' ');
                                        break;
                                    case Node::OperatorValue::OperatorType::Division: break;
                                }
                                for (std::size_t i = val.m_children.size() - 1; true; --i) {
                                    s += toBasicStringFromNode<CharT, Traits>(val.m_children[i], loc);
                                    if (i == 0) {
                                        break;
                                    }
                                    s += op;
                                }
                            }
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                s += conv.widen(')');
                            }
                            return s;
                        }
                    },
                    root->m_value);
            }
            /**
             * @brief 比较两个代数式是否相同
             * @param [in] root1 代数式1
             * @param [in] root2 代数式2
             * @return true 两个代数式相同
             * @return false 两个代数式不相同
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            [[nodiscard]] inline bool isEqual(const Node::Ptr &root1, const Node::Ptr &root2) {
                assert_msg(root1 != nullptr || root2 != nullptr,
                           "tnrw::Maths::Details::Node类指针错误地为 `nullptr`，请将其反馈到项目的 "
                           "`Issue` 中。");
                // 比较类型是否相等
                if (root1->m_type != root2->m_type)
                    return false;
                if (root1->m_type[Node::toSize(Node::TypeIndex::Operator)]) {
                    const auto &children1 = std::get<Node::OperatorValue>(root1->m_value).m_children;
                    const auto &children2 = std::get<Node::OperatorValue>(root2->m_value).m_children;
                    if (children1.size() != children2.size())
                        return false;
                    for (std::size_t i = 0; i < children1.size(); ++i) {
                        if (!isEqual(children1[i], children2[i]))
                            return false;
                    }
                    return true;
                } else if (root1->m_type[Node::toSize(Node::TypeIndex::Constant)]) {
                    return (std::get<Node::ConstantValue>(root1->m_value)
                            == std::get<Node::ConstantValue>(root2->m_value));
                } else {
                    return (std::get<Node::VariableValue>(root1->m_value)
                            == std::get<Node::VariableValue>(root2->m_value));
                }
            }
            /**
             * @brief 判断一个代数式是否为常量（包含分数）
             * @param [in] root 代数式
             * @return true 代数式为常量
             * @return false 代数式不为常量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            [[nodiscard]] inline bool isConstant(const Node::Ptr &root) {
                assert_msg(root != nullptr,
                           "tnrw::Maths::Details::Node类指针错误地为 `nullptr`，请将其反馈到项目的 "
                           "`Issue` 中。");
                if (root->m_type[Node::toSize(Node::TypeIndex::Constant)]) {
                    return true;
                }
                if (root->m_type[Node::toSize(Node::TypeIndex::Operator)]) {
                    const auto &val = std::get<Node::OperatorValue>(root->m_value);
                    if (val.m_op_type != Node::OperatorValue::OperatorType::Division)
                        return false;
                    if (val.m_children[0]->m_type[Node::toSize(Node::TypeIndex::Constant)]
                        && val.m_children[1]->m_type[Node::toSize(Node::TypeIndex::Constant)])
                        return true;
                }
                return false;
            }
            [[nodiscard]] inline bool tryPlusToConstant(Node::Ptr &root, ConstantType rhs) {
                assert_msg(root != nullptr,
                           "tnrw::Maths::Details::Node类指针错误地为 `nullptr`，请将其反馈到项目的 "
                           "`Issue` 中。");
                if (!isConstant(root))
                    return false;
                std::visit(
                    [&root, &rhs](auto &val) -> void {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            val += rhs;
                        } else if constexpr (std::is_same_v<T, Node::OperatorValue>) {
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)])
                                rhs = -rhs;
                            auto       &num = std::get<Node::ConstantValue>(val.m_children[0]->m_value);
                            const auto &den = std::get<Node::ConstantValue>(val.m_children[1]->m_value);
                            num += rhs * den;
                        }
                    },
                    root->m_value);
                return true;
            }
            /**
             * @brief 尝试合并代数式与变量
             * @param [in] root 根节点
             * @param [in] var 变量
             * @param [in] offset 变量系数
             * @return true 合并成功
             * @return false 合并失败
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            [[nodiscard]] inline bool tryToMerge(Node::Ptr &root, const VariableType var,
                                                 const ConstantType offset) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                return std::visit(
                    [&root, &var, &offset](auto &val) -> bool {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，无法合并
                            return false;
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，继续分类处理
                            if (val != var) {
                                // 根与值不相同，无法合并
                                return false;
                            }
                            // 根与值相同，可以合并
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                if (offset == 1) {
                                    // 根与值相加为0，清空
                                    clearNode(root);
                                } else {
                                    // 根与值相加不为0，创建新节点
                                    root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                    root = std::move(createMultiplicationNode(
                                        false, std::move(root), std::make_unique<Node>(offset - 1)));
                                }
                            } else {
                                if (offset == -1) {
                                    // 根与值相加为0，清空
                                    clearNode(root);
                                } else {
                                    // 根与值相加不为0，创建新节点
                                    root = std::move(createMultiplicationNode(
                                        false, std::move(root), std::make_unique<Node>(offset + 1)));
                                }
                            }
                            return true;

                        } else {
                            // 根是运算符，继续分类处理
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Addition) {
                                // 根是加法运算符，无法合并
                                return false;
                            }
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Multiplication) {
                                // 根是乘法运算符，继续分类处理
                                if (val.m_children.size() != 2)
                                    return false;
                                if (!val.m_children[1]->m_type[Node::toSize(Node::TypeIndex::Constant)]
                                    || !val.m_children[0]
                                            ->m_type[Node::toSize(Node::TypeIndex::Variable)])
                                    return false;
                                if (std::get<Node::VariableValue>(val.m_children[0]->m_value) != var)
                                    return false;
                                // 根与值匹配，合并
                                auto &factor = std::get<Node::ConstantValue>(val.m_children[1]->m_value);
                                if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                    factor = -factor;
                                    root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                }
                                if (factor + offset == 0) {
                                    clearNode(root);
                                } else if (factor + offset == 1) {
                                    root = std::make_unique<Node>(var);
                                } else if (factor + offset == -1) {
                                    root = std::make_unique<Node>(var, true);
                                } else {
                                    factor += offset;
                                }
                                return true;
                            }
                            // 根是除法运算符，继续分类处理
                            if (!val.m_children[1]->m_type[Node::toSize(Node::TypeIndex::Constant)])
                                return false;
                            auto &factor = std::get<Node::ConstantValue>(val.m_children[1]->m_value);
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                return tryToMerge(val.m_children[0], var, -factor * offset);
                            } else {
                                return tryToMerge(val.m_children[0], var, factor * offset);
                            }
                        }
                    },
                    root->m_value);
            }

            /**
             * @brief 尽量让根节点被除数除掉
             * @param [in] root 根节点
             * @param [in] rhs 常量除数
             * @return ConstantType 没被根节点除掉的剩余数
             * @warning 根节点为除法运算符时直接返回原数，不判断！
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            [[nodiscard]] inline ConstantType canDivided(Node::Ptr &root, ConstantType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                return std::visit(
                    [&root, &rhs](auto &val) -> ConstantType {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，尽量除掉

                            // 解决负数
                            if (rhs < 0) {
                                val = -val;
                                rhs = -rhs;
                            }
                            // 特殊情况
                            if (val == 0) {
                                return 1;
                            }
                            Node::ConstantValue gcdnum = gcd(val, rhs);
                            // 返回加速
                            if (gcdnum == 1)
                                return rhs;
                            val /= gcdnum;
                            return rhs / gcdnum;
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，返回原数
                            return rhs;
                        } else {
                            // 根是其他运算符，返回原数
                            if (val.m_op_type != Node::OperatorValue::OperatorType::Multiplication)
                                return rhs;
                            // 根是乘法运算符，尽量除掉其常量
                            auto &last_child = val.m_children.back();
                            if (!last_child->m_type[Node::toSize(Node::TypeIndex::Constant)])
                                return rhs;
                            auto &child_val = std::get<Node::ConstantValue>(last_child->m_value);
                            if (rhs < 0) {
                                auto is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                if (child_val < 0 || !is_nega) {
                                    child_val = -child_val;
                                } else {
                                    is_nega = !is_nega;
                                }
                                rhs = -rhs;
                            }
                            Node::ConstantValue gcdnum = gcd(child_val, rhs);
                            // 返回加速
                            if (gcdnum == 1)
                                return rhs;
                            if (child_val == gcdnum) {
                                // 如果孩子值与要与之相除的数相等，删除孩子
                                val.m_children.pop_back();

                                // 如果除后仅有一个孩子，退化类型
                                if (val.m_children.size() == 1) {
                                    moveChildToRoot(root, val.m_children[0]);
                                }
                            } else if (child_val == -gcdnum) {
                                // 如果孩子值与要与之相除的数相反，删除孩子并将值设为相反数
                                val.m_children.pop_back();

                                // 如果除后仅有一个孩子，退化类型
                                if (val.m_children.size() == 1) {
                                    moveChildToRoot(root, val.m_children[0]);
                                }
                                root->m_type[Node::toSize(Node::TypeIndex::Negative)] =
                                    root->m_type[Node::toSize(Node::TypeIndex::Negative)] ^ true;
                            } else {
                                child_val /= gcdnum;
                            }
                            return rhs / gcdnum;
                        }
                    },
                    root->m_value);
            }
            /**
             * @brief 判断根节点是否能除数被整除，能则除掉
             * @param [in] root 根节点
             * @param [in] rhs 变量除数
             * @return true 根节点能被除数整除
             * @return false 根节点不能被除数整除
             * @warning 根节点为除法运算符时直接返回 `false` ，不判断！
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             */
            [[nodiscard]] inline bool canDivided(Node::Ptr &root, VariableType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                return std::visit(
                    [&root, &rhs](auto &val) -> bool {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，如果是值是 `0` 返回 `true` ，返回 `false`
                            if (val == 0) {
                                return true;
                            }
                            return false;
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，如果根与值相同，返回 `true` 并将根设为1，否则返回 `true`
                            if (val != rhs)
                                return false;
                            using namespace tnrw::literals::Expressions_base_literals;
                            bool is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                            root = std::make_unique<Node>(is_nega ? -1_c : 1_c);
                            return true;
                        } else {
                            // 根是其他运算符，返回 `false`
                            if (val.m_op_type != Node::OperatorValue::OperatorType::Multiplication)
                                return false;
                            // 根是乘法运算符，查找子节点是否有与值相同，若有则返回 `true` 并删除此子节点，没有则返回 `false`
                            for (auto &child : val.m_children) {
                                if (child->m_type[Node::toSize(Node::TypeIndex::Variable)]) {
                                    if (std::get<Node::VariableValue>(child->m_value) == rhs) {
                                        swap(child, val.m_children[val.m_children.size() - 2]);
                                        val.m_children.erase(val.m_children.end() - 2);
                                        // 如果除后仅有一个孩子，退化类型
                                        if (val.m_children.size() == 1) {
                                            moveChildToRoot(root, val.m_children[0]);
                                        }
                                        return true;
                                    }
                                }
                            }
                            return false;
                        }
                    },
                    root->m_value);
            }

            /**
             * @brief 进行代数式和常量的加法
             * @param [in] root 根节点
             * @param [in] rhs 常量值
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator+=(Maths::AlgebraicExpression::ConstantType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator+(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::ConstantType rhs)
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator-=(Maths::AlgebraicExpression::ConstantType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator-(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::ConstantType rhs)
             */
            inline void plus(Node::Ptr &root, ConstantType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 处理特殊情况
                if (rhs == 0)
                    return;

                // 分类处理
                std::visit(
                    [&root, &rhs](auto &val) -> void {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，直接加
                            val += rhs;
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，创建新节点
                            root = std::move(
                                createAdditionNode(false, std::move(root), std::make_unique<Node>(rhs)));
                        } else {
                            // 根是运算符，继续分类处理
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Addition) {
                                // 根是+，检索是否有常量子节点，若有则加上，无则添加其为子节点
                                auto &last_child = val.m_children.back();

                                if (tryPlusToConstant(
                                        last_child, root->m_type[Node::toSize(Node::TypeIndex::Negative)]
                                                        ? -rhs
                                                        : rhs)) {
                                    if (last_child->m_type[Node::toSize(Node::TypeIndex::Constant)]
                                        && std::get<Node::ConstantValue>(last_child->m_value) == 0) {
                                        if (val.m_children.size() == 2) {
                                            moveChildToRoot(root, val.m_children[0]);
                                        } else {
                                            val.m_children.pop_back();
                                        }
                                    }
                                    return;
                                }

                                if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                    val.m_children.push_back(std::make_unique<Node>(-rhs));
                                } else {
                                    val.m_children.push_back(std::make_unique<Node>(rhs));
                                }
                            } else if (val.m_op_type == Node::OperatorValue::OperatorType::Division) {
                                // 根是除法运算符，如果是常量则相加，否则创建新节点
                                if (!tryPlusToConstant(
                                        root, root->m_type[Node::toSize(Node::TypeIndex::Negative)]
                                                  ? -rhs
                                                  : rhs)) {
                                    root = std::move(createAdditionNode(false, std::move(root),
                                                                        std::make_unique<Node>(rhs)));
                                }
                            } else {
                                // 根是乘法运算符，创建新节点
                                root = std::move(createAdditionNode(false, std::move(root),
                                                                    std::make_unique<Node>(rhs)));
                            }
                        }
                    },
                    root->m_value);
            }
            /**
             * @brief 进行代数式和变量的加法
             * @param [in] root 根节点
             * @param [in] rhs 变量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator+=(Maths::AlgebraicExpression::VariableType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator+(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::VariableType rhs)
             */
            inline void plus(Node::Ptr &root, VariableType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                if (root->m_type[Node::toSize(Node::TypeIndex::Operator)]) {
                    auto &val = std::get<Node::OperatorValue>(root->m_value);
                    if (val.m_op_type == Node::OperatorValue::OperatorType::Addition) {
                        // 根节点是加法运算符，循环尝试与子节点合并，若成功则退出，全部失败则添加其为子节点
                        for (std::size_t i = 0; i < val.m_children.size(); ++i) {
                            auto &child = val.m_children[i];
                            if (tryToMerge(
                                    child, rhs,
                                    (root->m_type[Node::toSize(Node::TypeIndex::Negative)] ? -1 : 1))) {
                                if (child->m_type[Node::toSize(Node::TypeIndex::Constant)]
                                    && std::get<Node::ConstantValue>(child->m_value) == 0) {
                                    swap(child, val.m_children[val.m_children.size() - 2]);
                                    val.m_children.erase(val.m_children.end() - 2);
                                    // 如果加后仅有一个孩子，退化类型
                                    if (val.m_children.size() == 1) {
                                        moveChildToRoot(root, val.m_children[0]);
                                    }
                                }
                                return;
                            }
                        }
                        val.m_children.push_back(std::make_unique<Node>(
                            rhs, root->m_type[Node::toSize(Node::TypeIndex::Negative)]));
                        swap(val.m_children.back(), val.m_children[val.m_children.size() - 2]);
                        return;
                    }
                }
                // 根为常量，创建新节点或覆盖根
                if (root->m_type[Node::toSize(Node::TypeIndex::Constant)]) {
                    if (std::get<Node::ConstantValue>(root->m_value) == 0) {
                        root = std::make_unique<Node>(rhs);
                    } else {
                        root = std::move(
                            createAdditionNode(false, std::make_unique<Node>(rhs), std::move(root)));
                    }
                    return;
                }
                // 根为其他，尝试与子节点合并，若成功则退出，失败则创建新节点
                if (tryToMerge(root, rhs, 1))
                    return;
                root =
                    std::move(createAdditionNode(false, std::make_unique<Node>(rhs), std::move(root)));
            }
            /**
             * @brief 进行代数式和变量的减法
             * @param [in] root 代数式
             * @param [in] rhs 变量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator-=(Maths::AlgebraicExpression::VariableType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator-(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::VariableType rhs)
             */
            inline void minus(Node::Ptr &root, VariableType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                if (root->m_type[Node::toSize(Node::TypeIndex::Operator)]) {
                    auto &val = std::get<Node::OperatorValue>(root->m_value);
                    if (val.m_op_type == Node::OperatorValue::OperatorType::Addition) {
                        // 根节点是加法运算符，循环尝试与子节点合并，若成功则退出，全部失败则添加其为子节点
                        for (std::size_t i = 0; i < val.m_children.size(); ++i) {
                            auto &child = val.m_children[i];
                            if (tryToMerge(
                                    child, rhs,
                                    (root->m_type[Node::toSize(Node::TypeIndex::Negative)] ? 1 : -1))) {
                                if (child->m_type[Node::toSize(Node::TypeIndex::Constant)]
                                    && std::get<Node::ConstantValue>(child->m_value) == 0) {
                                    swap(child, val.m_children[val.m_children.size() - 2]);
                                    val.m_children.erase(val.m_children.end() - 2);
                                }
                                // 如果减后仅有一个孩子，退化类型
                                if (val.m_children.size() == 1) {
                                    moveChildToRoot(root, val.m_children[0]);
                                }
                                return;
                            }
                        }
                        val.m_children.push_back(std::make_unique<Node>(
                            rhs, !root->m_type[Node::toSize(Node::TypeIndex::Negative)]));
                        swap(val.m_children.back(), val.m_children[val.m_children.size() - 2]);
                        return;
                    }
                }
                // 根为常量，创建新节点或覆盖根
                if (root->m_type[Node::toSize(Node::TypeIndex::Constant)]) {
                    if (std::get<Node::ConstantValue>(root->m_value) == 0) {
                        root = std::make_unique<Node>(rhs, true);
                    } else {
                        root = std::move(createAdditionNode(false, std::make_unique<Node>(rhs, true),
                                                            std::move(root)));
                    }
                    return;
                }
                // 根为其他，尝试与子节点合并，若成功则退出，失败则创建新节点
                if (tryToMerge(root, rhs, -1))
                    return;
                root = std::move(
                    createAdditionNode(false, std::make_unique<Node>(rhs, true), std::move(root)));
            }

            /**
             * @brief 进行代数式和常量的乘法
             * @param [in] root 代数式
             * @param [in] rhs 常量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator*=(Maths::AlgebraicExpression::ConstantType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator*(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::ConstantType rhs)
             */
            inline void multiply(Node::Ptr &root, ConstantType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 处理特殊情况
                if (rhs == 0) {
                    clearNode(root);
                    return;
                }
                if (rhs == 1)
                    return;

                // 分类处理
                std::visit(
                    [&root, &rhs](auto &val) -> void {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，相乘
                            val *= rhs;
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，如果为-1，取相反数，否则创建新节点
                            if (rhs == -1) {
                                auto is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                is_nega = !is_nega;
                                return;
                            }
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                root = std::move(createMultiplicationNode(false, std::move(root),
                                                                          std::make_unique<Node>(-rhs)));
                            } else {
                                root = std::move(createMultiplicationNode(false, std::move(root),
                                                                          std::make_unique<Node>(rhs)));
                            }
                        } else {
                            // 根是运算符，继续分类
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Multiplication) {
                                // 根是乘法运算符，将其加入子节点或乘到最后的常量节点
                                auto &child = val.m_children.back();
                                if (child->m_type[Node::toSize(Node::TypeIndex::Constant)]) {
                                    std::get<Node::ConstantValue>(child->m_value) *= rhs;
                                } else {
                                    if (rhs == -1) {
                                        // 特殊情况，取相反数
                                        auto is_nega =
                                            root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                        is_nega = !is_nega;
                                        return;
                                    }
                                    {
                                        auto is_nega =
                                            root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                        if (is_nega) {
                                            // 消除无用的负号
                                            is_nega = !is_nega;
                                            rhs = -rhs;
                                        }
                                    }
                                    val.m_children.push_back(std::make_unique<Node>(rhs));
                                }
                            } else if (val.m_op_type == Node::OperatorValue::OperatorType::Division) {
                                // 根是除法运算符，尝试与分母相除，剩余的乘到分子
                                bool is_nega = false;
                                if (rhs < 0) {
                                    rhs = -rhs;
                                    is_nega = true;
                                }
                                ConstantType rest = canDivided(val.m_children[1], rhs);
                                if (rest != 1) {
                                    multiply(val.m_children[0], is_nega ? -rest : rest);
                                } else if (is_nega) {
                                    multiply(val.m_children[0], -rest);
                                }
                                if (rest != rhs) {
                                    auto &den = val.m_children[1];
                                    if (den->m_type[Node::toSize(Node::TypeIndex::Constant)]
                                        && std::get<Node::ConstantValue>(den->m_value) == 1) {
                                        moveChildToRoot(root, val.m_children[0]);
                                    }
                                }
                            } else {
                                // 根是加法运算符，将常量分别乘到其子节点里
                                for (auto &child : val.m_children) { multiply(child, rhs); }
                            }
                        }
                    },
                    root->m_value);
            }
            /**
             * @brief 进行代数式和变量的乘法
             * @param [in] root 代数式
             * @param [in] rhs 变量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator*=(Maths::AlgebraicExpression::VariableType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator*(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::VariableType rhs)
             */
            inline void multiply(Node::Ptr &root, VariableType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                std::visit(
                    [&root, &rhs](auto &val) -> void {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，如果值是 `0` 跳过，如果值是 `1` 覆盖原节点，如果值是 `1` 覆盖原节点，否则创建新节点
                            if (val == 0) {
                                return;
                            }
                            if (val == 1) {
                                root = std::make_unique<Node>(rhs);
                                return;
                            }
                            if (val == -1) {
                                root = std::make_unique<Node>(rhs, true);
                                return;
                            }
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                root = std::move(createMultiplicationNode(
                                    true, std::make_unique<Node>(rhs), std::move(root)));
                            } else {
                                root = std::move(createMultiplicationNode(
                                    false, std::make_unique<Node>(rhs), std::move(root)));
                            }
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，创建新节点
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                root = std::move(createMultiplicationNode(true, std::move(root),
                                                                          std::make_unique<Node>(rhs)));
                            } else {
                                root = std::move(createMultiplicationNode(false, std::move(root),
                                                                          std::make_unique<Node>(rhs)));
                            }
                        } else {
                            // 根是运算符，继续分类
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Multiplication) {
                                // 根是乘法运算符，将其加入子节点
                                val.m_children.push_back(std::make_unique<Node>(rhs));
                                auto &child = val.m_children[val.m_children.size() - 2];
                                if (child->m_type[Node::toSize(Node::TypeIndex::Constant)]) {
                                    swap(child, val.m_children.back());
                                }
                            } else if (val.m_op_type == Node::OperatorValue::OperatorType::Division) {
                                // 根是除法运算符，尝试与分母相除，不行则乘到分子
                                if (canDivided(val.m_children[1], rhs)) {
                                    auto &den = val.m_children[1];
                                    if (den->m_type[Node::toSize(Node::TypeIndex::Constant)]
                                        && std::get<Node::ConstantValue>(den->m_value) == 1) {
                                        moveChildToRoot(root, val.m_children[0]);
                                    }
                                    return;
                                }
                                multiply(val.m_children[0], rhs);
                            } else {
                                // 根是加法运算符，将变量分别乘到其子节点里，将乘后的常量移到尾部
                                Node::Ptr *constantChild = nullptr;
                                for (auto &child : val.m_children) {
                                    multiply(child, rhs);
                                    if (isConstant(child)) {
                                        constantChild = &child;
                                    }
                                }
                                if (constantChild != nullptr) {
                                    swap(*constantChild, val.m_children.back());
                                }
                            }
                        }
                    },
                    root->m_value);
            }
            /**
             * @brief 进行代数式和常量的除法
             * @param [in] root 代数式
             * @param [in] rhs 常量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 除以0行为未定义
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator/=(Maths::AlgebraicExpression::ConstantType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator/(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::ConstantType rhs)
             */
            inline void devide(Node::Ptr &root, ConstantType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 特殊情况
                if (rhs == 1)
                    return;
                // 分类处理
                std::visit(
                    [&root, &rhs](auto &val) -> void {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，尝试相除，剩余的创建新节点
                            ConstantType rest = canDivided(root, rhs);
                            if (rest == 1)
                                return;
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                root = std::move(createDivisionNode(true, std::move(root),
                                                                    std::make_unique<Node>(rest)));
                            } else {
                                root = std::move(createDivisionNode(false, std::move(root),
                                                                    std::make_unique<Node>(rest)));
                            }
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，如果为-1，取相反数，否则创建新节点
                            if (rhs == -1) {
                                auto is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                is_nega = !is_nega;
                                return;
                            }
                            if (rhs < 0) {
                                auto is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                is_nega = -is_nega;
                                rhs = -rhs;
                            }
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                if (rhs < 0) {
                                    root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                    root = std::move(createDivisionNode(false, std::move(root),
                                                                        std::make_unique<Node>(-rhs)));
                                } else {
                                    root = std::move(createDivisionNode(false, std::move(root),
                                                                        std::make_unique<Node>(rhs)));
                                }
                            } else {
                                root = std::move(createDivisionNode(false, std::move(root),
                                                                    std::make_unique<Node>(rhs)));
                            }
                        } else {
                            // 根是运算符，继续分类
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Multiplication) {
                                // 根是乘法运算符，尝试相除，剩余的创建新节点
                                ConstantType rest = canDivided(root, rhs);
                                if (rest == 1)
                                    return;
                                auto is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];

                                if (rhs < 0) {
                                    // 消除无用的负号
                                    is_nega = !is_nega;
                                    if (-rest == 1)
                                        return;
                                    root = std::move(createDivisionNode(false, std::move(root),
                                                                        std::make_unique<Node>(-rest)));
                                } else {
                                    root = std::move(createDivisionNode(false, std::move(root),
                                                                        std::make_unique<Node>(rest)));
                                }
                            } else if (val.m_op_type == Node::OperatorValue::OperatorType::Division) {
                                // 根是除法运算符，尝试与分子相除，剩余的乘到分母
                                auto is_nega = root->m_type[Node::toSize(Node::TypeIndex::Negative)];
                                {
                                    if (rhs < 0) {
                                        if (is_nega) {
                                            // 消除无用的负号
                                            is_nega = !is_nega;
                                            rhs = -rhs;
                                        } else {
                                            using namespace tnrw::literals::Expressions_base_literals;
                                            multiply(val.m_children[0], -1_c);
                                            rhs = -rhs;
                                        }
                                    }
                                }
                                ConstantType rest = canDivided(val.m_children[0], rhs);
                                if (rest != 1) {
                                    multiply(val.m_children[1], rest);
                                }
                            } else {
                                // 根是加法运算符，将常量分别除到其子节点里
                                for (auto &child : val.m_children) { devide(child, rhs); }
                            }
                        }
                    },
                    root->m_value);
            }
            /**
             * @brief 进行代数式和变量的除法
             * @param [in] root 代数式
             * @param [in] rhs 变量
             * @warning 此函数仅为一个辅助函数，当根节点为 `nullptr` 时会发生断言错误！
             * @warning 此函数仅为一个辅助函数，完备的函数见
             * @ref Maths::AlgebraicExpression &Maths::AlgebraicExpression::operator/=(Maths::AlgebraicExpression::VariableType rhs)
             * @ref const Maths::AlgebraicExpression Maths::operator/(const Maths::AlgebraicExpression &lhs, Maths::AlgebraicExpression::VariableType rhs)
             */
            inline void devide(Node::Ptr &root, VariableType rhs) noexcept {
                assert_msg(root != nullptr, "tnrw::Maths::Details::Node类指针错误地为 "
                                            "`nullptr`，请将其反馈到项目的 `Issue` 中。");
                // 分类处理
                std::visit(
                    [&root, &rhs](auto &val) -> void {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Node::ConstantValue>) {
                            // 根是常量，如果值是 `0` 跳过，否则创建新节点
                            if (val == 0) {
                                return;
                            }
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                root = std::move(createDivisionNode(true, std::move(root),
                                                                    std::make_unique<Node>(rhs)));
                            } else {
                                root = std::move(createDivisionNode(false, std::move(root),
                                                                    std::make_unique<Node>(rhs)));
                            }
                        } else if constexpr (std::is_same_v<T, Node::VariableValue>) {
                            // 根是变量，尝试相除，不行创建新节点
                            if (canDivided(root, rhs))
                                return;
                            if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                root = std::move(createDivisionNode(true, std::move(root),
                                                                    std::make_unique<Node>(rhs)));
                            } else {
                                root = std::move(createDivisionNode(false, std::move(root),
                                                                    std::make_unique<Node>(rhs)));
                            }
                        } else {
                            // 根是运算符，继续分类
                            if (val.m_op_type == Node::OperatorValue::OperatorType::Multiplication) {
                                // 根是乘法运算符，尝试相除，不行创建新节点
                                if (canDivided(root, rhs))
                                    return;
                                if (root->m_type[Node::toSize(Node::TypeIndex::Negative)]) {
                                    root->m_type.reset(Node::toSize(Node::TypeIndex::Negative));
                                    root = std::move(createDivisionNode(true, std::move(root),
                                                                        std::make_unique<Node>(rhs)));
                                } else {
                                    root = std::move(createDivisionNode(false, std::move(root),
                                                                        std::make_unique<Node>(rhs)));
                                }
                            } else if (val.m_op_type == Node::OperatorValue::OperatorType::Division) {
                                // 根是除法运算符，尝试与分子相除，剩余的乘到分母
                                if (canDivided(val.m_children[0], rhs))
                                    return;
                                multiply(val.m_children[1], rhs);
                            } else {
                                // 根是加法运算符，将变量分别除到其子节点里，将除后的常量移到尾部
                                Node::Ptr *constantChild = nullptr;
                                for (auto &child : val.m_children) {
                                    devide(child, rhs);
                                    if (isConstant(child)) {
                                        constantChild = &child;
                                    }
                                }
                                if (constantChild != nullptr) {
                                    swap(*constantChild, val.m_children.back());
                                }
                            }
                        }
                    },
                    root->m_value);
            }
        }; // namespace Details

        /// @endcond

        // AlgebraicExpression类的成员定义
        [[nodiscard]] AlgebraicExpression::AlgebraicExpression() noexcept
            : m_root(std::make_unique<Details::Node>(static_cast<ConstantType>(0))) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(const ConstantType constant) noexcept
            : m_root(std::make_unique<Details::Node>(constant)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(const VariableType vairable) noexcept
            : m_root(std::make_unique<Details::Node>(vairable)) {}

        AlgebraicExpression::~AlgebraicExpression() noexcept = default;

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(const AlgebraicExpression &rhs) noexcept
            : m_root(std::make_unique<Details::Node>(*rhs.m_root)) {}

        [[nodiscard]] AlgebraicExpression::AlgebraicExpression(AlgebraicExpression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

        AlgebraicExpression &AlgebraicExpression::operator=(const AlgebraicExpression &rhs) noexcept {
            if (this != &rhs) {
                m_root = std::make_unique<Details::Node>(*rhs.m_root);
            }
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator=(AlgebraicExpression &&rhs) noexcept {
            if (this != &rhs) {
                m_root = std::move(rhs.m_root);
            }
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator+=(const ConstantType rhs) noexcept {
            Details::plus(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator+=(const VariableType rhs) noexcept {
            Details::plus(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator-=(const ConstantType rhs) noexcept {
            Details::plus(m_root, -rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator-=(const VariableType rhs) noexcept {
            Details::minus(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator*=(const ConstantType rhs) noexcept {
            Details::multiply(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator*=(const VariableType rhs) noexcept {
            Details::multiply(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator/=(const ConstantType rhs) noexcept {
            Details::devide(m_root, rhs);
            return *this;
        }

        AlgebraicExpression &AlgebraicExpression::operator/=(const VariableType rhs) noexcept {
            Details::devide(m_root, rhs);
            return *this;
        }

        [[nodiscard]] AlgebraicExpression AlgebraicExpression::operator+() noexcept { return *this; }

        [[nodiscard]] AlgebraicExpression AlgebraicExpression::operator-() noexcept {
            AlgebraicExpression copy_of_this(*this);
            copy_of_this.changeToOpposite();
            return copy_of_this;
        }

        AlgebraicExpression &AlgebraicExpression::operator++() noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            Details::plus(m_root, 1_c);
            return *this;
        }
        AlgebraicExpression &AlgebraicExpression::operator--() noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            Details::plus(m_root, -1_c);
            return *this;
        }

        [[nodiscard]] const AlgebraicExpression AlgebraicExpression::operator++(int) noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            AlgebraicExpression copy_of_old(*this);
            Details::plus(m_root, 1_c);
            return copy_of_old;
        }
        [[nodiscard]] const AlgebraicExpression AlgebraicExpression::operator--(int) noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            AlgebraicExpression copy_of_old(*this);
            Details::plus(m_root, -1_c);
            return copy_of_old;
        }

        void AlgebraicExpression::clear() noexcept { Details::clearNode(m_root); }

        [[nodiscard]] std::string AlgebraicExpression::toString(const std::locale &loc) const noexcept {
            return Details::toBasicStringFromNode<char, std::char_traits<char>>(m_root, loc);
        }

        [[nodiscard]] std::wstring
        AlgebraicExpression::toWString(const std::locale &loc) const noexcept {
            return Details::toBasicStringFromNode<wchar_t, std::char_traits<wchar_t>>(m_root, loc);
        }

        template <typename CharT, typename Traits>
        [[nodiscard]] std::basic_string<CharT, Traits>
        AlgebraicExpression::toBasicString(const std::locale &loc) const noexcept {
            return Details::toBasicStringFromNode<CharT, Traits>(m_root, loc);
        }

        void AlgebraicExpression::changeToOpposite() noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            Details::multiply(m_root, -1_c);
        }

        [[nodiscard]] const AlgebraicExpression
        operator+(const AlgebraicExpression &lhs, const AlgebraicExpression::ConstantType rhs) noexcept {
            return (AlgebraicExpression(lhs) += rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator+(const AlgebraicExpression::ConstantType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) += lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator+(const AlgebraicExpression &lhs, const AlgebraicExpression::VariableType rhs) noexcept {
            return (AlgebraicExpression(lhs) += rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator+(const AlgebraicExpression::VariableType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) += lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator-(const AlgebraicExpression &lhs, const AlgebraicExpression::ConstantType rhs) noexcept {
            return (AlgebraicExpression(lhs) -= rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator-(const AlgebraicExpression::ConstantType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) -= lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator-(const AlgebraicExpression &lhs, const AlgebraicExpression::VariableType rhs) noexcept {
            return (AlgebraicExpression(lhs) -= rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator-(const AlgebraicExpression::VariableType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) -= lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator*(const AlgebraicExpression &lhs, const AlgebraicExpression::ConstantType rhs) noexcept {
            return (AlgebraicExpression(lhs) *= rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator*(const AlgebraicExpression::ConstantType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) *= lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator*(const AlgebraicExpression &lhs, const AlgebraicExpression::VariableType rhs) noexcept {
            return (AlgebraicExpression(lhs) *= rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator*(const AlgebraicExpression::VariableType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) *= lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator/(const AlgebraicExpression &lhs, const AlgebraicExpression::ConstantType rhs) noexcept {
            return (AlgebraicExpression(lhs) /= rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator/(const AlgebraicExpression::ConstantType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) /= lhs);
        }

        [[nodiscard]] const AlgebraicExpression
        operator/(const AlgebraicExpression &lhs, const AlgebraicExpression::VariableType rhs) noexcept {
            return (AlgebraicExpression(lhs) /= rhs);
        }
        [[nodiscard]] const AlgebraicExpression operator/(const AlgebraicExpression::VariableType lhs,
                                                          const AlgebraicExpression &rhs) noexcept {
            return (AlgebraicExpression(rhs) /= lhs);
        }

        [[nodiscard]] bool operator==(const AlgebraicExpression &lhs,
                                      const AlgebraicExpression &rhs) noexcept {
            if (&lhs == &rhs)
                return true;
            return Details::isEqual(lhs.m_root, rhs.m_root);
        }
        [[nodiscard]] bool operator!=(const AlgebraicExpression &lhs,
                                      const AlgebraicExpression &rhs) noexcept {
            if (&lhs == &rhs)
                return false;
            return !Details::isEqual(lhs.m_root, rhs.m_root);
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const AlgebraicExpression         &rhs) noexcept {
            return (out << rhs.toBasicString<CharT, Traits>(out.getloc()));
        }

        // NumericExpression类的成员定义
        [[nodiscard]] NumericExpression::NumericExpression() noexcept
            : m_root(std::make_unique<Details::Node>(static_cast<ConstantType>(0))) {}

        [[nodiscard]] NumericExpression::NumericExpression(const ConstantType constant) noexcept
            : m_root(std::make_unique<Details::Node>(constant)) {}

        NumericExpression::~NumericExpression() noexcept = default;

        [[nodiscard]] NumericExpression::NumericExpression(const NumericExpression &rhs) noexcept
            : m_root(std::make_unique<Details::Node>(*rhs.m_root)) {}

        [[nodiscard]] NumericExpression::NumericExpression(NumericExpression &&rhs) noexcept
            : m_root(std::move(rhs.m_root)) {}

        NumericExpression &NumericExpression::operator=(const NumericExpression &rhs) noexcept {
            if (this != &rhs) {
                m_root = std::make_unique<Details::Node>(*rhs.m_root);
            }
            return *this;
        }

        NumericExpression &NumericExpression::operator=(NumericExpression &&rhs) noexcept {
            if (this != &rhs) {
                m_root = std::move(rhs.m_root);
            }
            return *this;
        }

        NumericExpression &NumericExpression::operator+=(const ConstantType rhs) noexcept {
            Details::plus(m_root, rhs);
            return *this;
        }

        NumericExpression &NumericExpression::operator-=(const ConstantType rhs) noexcept {
            Details::plus(m_root, -rhs);
            return *this;
        }

        NumericExpression &NumericExpression::operator*=(const ConstantType rhs) noexcept {
            Details::multiply(m_root, rhs);
            return *this;
        }

        NumericExpression &NumericExpression::operator/=(const ConstantType rhs) noexcept {
            Details::devide(m_root, rhs);
            return *this;
        }

        [[nodiscard]] NumericExpression NumericExpression::operator+() noexcept { return *this; }

        [[nodiscard]] NumericExpression NumericExpression::operator-() noexcept {
            NumericExpression copy_of_this(*this);
            copy_of_this.changeToOpposite();
            return copy_of_this;
        }

        NumericExpression &NumericExpression::operator++() noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            Details::plus(m_root, 1_c);
            return *this;
        }
        NumericExpression &NumericExpression::operator--() noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            Details::plus(m_root, -1_c);
            return *this;
        }

        [[nodiscard]] const NumericExpression NumericExpression::operator++(int) noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            NumericExpression copy_of_old(*this);
            Details::plus(m_root, 1_c);
            return copy_of_old;
        }
        [[nodiscard]] const NumericExpression NumericExpression::operator--(int) noexcept {
            using namespace tnrw::literals::Expressions_base_literals;
            NumericExpression copy_of_old(*this);
            Details::plus(m_root, -1_c);
            return copy_of_old;
        }

        void NumericExpression::clear() noexcept { Details::clearNode(m_root); }

        [[nodiscard]] std::string NumericExpression::toString(const std::locale &loc) const noexcept {
            return Details::toBasicStringFromNode<char, std::char_traits<char>>(m_root, loc);
        }

        [[nodiscard]] std::wstring NumericExpression::toWString(const std::locale &loc) const noexcept {
            return Details::toBasicStringFromNode<wchar_t, std::char_traits<wchar_t>>(m_root, loc);
        }

        template <typename CharT, typename Traits>
        [[nodiscard]] std::basic_string<CharT, Traits>
        NumericExpression::toBasicString(const std::locale &loc) const noexcept {
            return Details::toBasicStringFromNode<CharT, Traits>(m_root, loc);
        }

        void NumericExpression::changeToOpposite() noexcept {
            if (m_root->m_type[Details::Node::toSize(Details::Node::TypeIndex::Constant)]) {
                // 特殊情况1
                auto &val = std::get<Details::Node::ConstantValue>(m_root->m_value);
                val = -val;
            } else {
                // 特殊情况2
                auto &val = std::get<Details::Node::ConstantValue>(
                    std::get<Details::Node::OperatorValue>(m_root->m_value).m_children[0]->m_value);
                val = -val;
            }
        }

        [[nodiscard]] const NumericExpression
        operator+(const NumericExpression &lhs, const NumericExpression::ConstantType rhs) noexcept {
            return (NumericExpression(lhs) += rhs);
        }
        [[nodiscard]] const NumericExpression operator+(const NumericExpression::ConstantType lhs,
                                                        const NumericExpression &rhs) noexcept {
            return (NumericExpression(rhs) += lhs);
        }

        [[nodiscard]] const NumericExpression
        operator-(const NumericExpression &lhs, const NumericExpression::ConstantType rhs) noexcept {
            return (NumericExpression(lhs) -= rhs);
        }
        [[nodiscard]] const NumericExpression operator-(const NumericExpression::ConstantType lhs,
                                                        const NumericExpression &rhs) noexcept {
            return (NumericExpression(rhs) -= lhs);
        }

        [[nodiscard]] const NumericExpression
        operator*(const NumericExpression &lhs, const NumericExpression::ConstantType rhs) noexcept {
            return (NumericExpression(lhs) *= rhs);
        }
        [[nodiscard]] const NumericExpression operator*(const NumericExpression::ConstantType lhs,
                                                        const NumericExpression &rhs) noexcept {
            return (NumericExpression(rhs) *= lhs);
        }

        [[nodiscard]] const NumericExpression
        operator/(const NumericExpression &lhs, const NumericExpression::ConstantType rhs) noexcept {
            return (NumericExpression(lhs) /= rhs);
        }
        [[nodiscard]] const NumericExpression operator/(const NumericExpression::ConstantType lhs,
                                                        const NumericExpression &rhs) noexcept {
            return (NumericExpression(rhs) /= lhs);
        }

        [[nodiscard]] bool operator==(const NumericExpression &lhs,
                                      const NumericExpression &rhs) noexcept {
            if (&lhs == &rhs)
                return true;
            return Details::isEqual(lhs.m_root, rhs.m_root);
        }
        [[nodiscard]] bool operator!=(const NumericExpression &lhs,
                                      const NumericExpression &rhs) noexcept {
            if (&lhs == &rhs)
                return false;
            return !Details::isEqual(lhs.m_root, rhs.m_root);
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const NumericExpression           &rhs) noexcept {
            return (out << rhs.toBasicString<CharT, Traits>(out.getloc()));
        }

        // 模板实例化

        template std::basic_ostream<char> &operator<< <char>(std::basic_ostream<char>  &out,
                                                             const AlgebraicExpression &rhs) noexcept;
        template std::basic_ostream<char> &operator<< <char>(std::basic_ostream<char> &out,
                                                             const NumericExpression  &rhs) noexcept;

    } // namespace Maths

    namespace literals {

        inline namespace Expressions_base_literals {
            [[nodiscard]] constexpr Maths::ConstantType
            operator""_c(const unsigned long long constant) noexcept {
                return static_cast<Maths::ConstantType>(constant);
            }
            [[nodiscard]] constexpr Maths::VariableType operator""_v(const char variable) noexcept {
                return static_cast<Maths::VariableType>(variable);
            }
        } // namespace Expressions_base_literals

        inline namespace AlgebraicExpression_literals {

            [[nodiscard]] Maths::AlgebraicExpression
            operator""_cAlgeExpr(const unsigned long long constant) noexcept {
                return Maths::AlgebraicExpression(
                    static_cast<Maths::AlgebraicExpression::ConstantType>(constant));
            }
            [[nodiscard]] Maths::AlgebraicExpression operator""_vAlgeExpr(const char variable) noexcept {
                return Maths::AlgebraicExpression(
                    static_cast<Maths::AlgebraicExpression::VariableType>(variable));
            }

        } // namespace AlgebraicExpression_literals

        inline namespace NumericExpression_literals {

            [[nodiscard]] Maths::NumericExpression
            operator""_cNumExpr(const unsigned long long constant) noexcept {
                return Maths::NumericExpression(
                    static_cast<Maths::NumericExpression::ConstantType>(constant));
            }

        } // namespace NumericExpression_literals

    } // namespace literals

} // namespace tnrw