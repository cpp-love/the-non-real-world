/**
 * @file algebraic_expression.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了代数式类
 * @version 0.1.0-3
 * @date 2026-02-14
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 使用 **树** 这一数据结构来表示代数式
 * - 声明了代数式与常量和变量的加、减、乘、除法运算符重载及其复合赋值运算符重载
 * - 声明了代数式相关的其他成员方法
 * - 声明了将常量及变量字面量转为代数式的自定义字面量
 * 
 */

#ifndef TNRW_MATH_ALGEGRAIC_EXPRESSION_HPP
#define TNRW_MATH_ALGEGRAIC_EXPRESSION_HPP

#include "math/expressions_base.hpp"
#include <concepts>
#include <format>
#include <functional>
#include <iosfwd>
#include <locale>
#include <optional>
#include <set>
#include <string>

namespace tnrw {

    namespace math {

        class NumericExpression;

        /**
         * @brief 代数式类
         * @details 该类使用树状代数式，可以进行部分代数运算
         * @todo
         * - 加入根式
         */
        class AlgebraicExpression {
            /// @cond INTERNAL
          private: /// @privatesection
            // 数据成员
            inline static std::set<VariableType, std::ranges::less> m_vars; ///< 统一存储变量的地方
            details::NodePtr m_root; ///< 私有实现指针，也是代数式树的根节点

            /// @endcond
          public: /// @publicsection
            // 友元声明
            friend bool operator==(const AlgebraicExpression &lhs,
                                   const AlgebraicExpression &rhs) noexcept;
            friend bool operator!=(const AlgebraicExpression &lhs,
                                   const AlgebraicExpression &rhs) noexcept;

            // 构造、赋值、析构
            /**
             * @brief 默认构造函数
             * @details 创建一个空的代数式
             */
            [[nodiscard]] AlgebraicExpression() noexcept;
            /**
             * @brief 以常量为参数的构造函数
             * @param [in] constant 常量
             */
            [[nodiscard]] explicit AlgebraicExpression(IntegerConstantType constant) noexcept;
            /**
             * @brief 以变量为参数的构造函数
             * @param [in] variable 变量
             */
            [[nodiscard]] explicit AlgebraicExpression(VariableView variable) noexcept;
            /**
             * @brief 以无字母的代数式为参数的构造函数
             * @param [in] num_expr 无字母的代数式
             */
            [[nodiscard]] explicit AlgebraicExpression(NumericExpression num_expr) noexcept;
            /**
             * @brief 复制构造函数
             * @param [in] rhs 另一个代数式对象
             */
            [[nodiscard]] AlgebraicExpression(const AlgebraicExpression &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个代数式对象
             */
            [[nodiscard]] AlgebraicExpression(AlgebraicExpression &&rhs) noexcept;
            /**
             * @brief 以常量为参数的赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator=(const IntegerConstantType &rhs) & noexcept;
            /**
             * @brief 以变量为参数的赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator=(VariableView rhs) & noexcept;
            /**
             * @brief 复制赋值运算符
             * @param [in] rhs 另一个代数式对象
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator=(const AlgebraicExpression &rhs) & noexcept;
            /**
             * @brief 移动赋值运算符
             * @param [in] rhs 另一个代数式对象
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator=(AlgebraicExpression &&rhs) & noexcept;
            /// @brief 析构函数
            ~AlgebraicExpression() noexcept;

            // 复合赋值与其他操作符
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator+=(IntegerConstantType rhs)              &noexcept;
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator+=(VariableView rhs)              &noexcept;
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 另一代数式
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator+=(AlgebraicExpression rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator-=(IntegerConstantType rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator-=(VariableView rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 另一代数式
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator-=(AlgebraicExpression rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator*=(IntegerConstantType rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator*=(VariableView rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 另一代数式
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator*=(AlgebraicExpression rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            AlgebraicExpression              &operator/=(IntegerConstantType rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator/=(VariableView rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 另一代数式
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            AlgebraicExpression              &operator/=(AlgebraicExpression rhs)              &noexcept;
            /**
             * @brief 一元加法运算符
             * @return AlgebraicExpression 当前对象的副本( `*this` )
             */
            [[nodiscard]] AlgebraicExpression operator+() const noexcept;
            /**
             * @brief 一元减法运算符
             * @return AlgebraicExpression 当前对象的副本取相反数( `*this` )
             */
            [[nodiscard]] AlgebraicExpression operator-() const noexcept;
            /**
             * @brief 后缀自增运算符
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator++() noexcept;
            /**
             * @brief 后缀自减运算符
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression              &operator--() noexcept;
            /**
             * @brief 前缀自增运算符
             * @return AlgebraicExpression 自增前的副本
             */
            [[nodiscard]] AlgebraicExpression operator++(int) noexcept;
            /**
             * @brief 前缀自减运算符
             * @return AlgebraicExpression 自减前的副本
             */
            [[nodiscard]] AlgebraicExpression operator--(int) noexcept;

            // 其他成员函数
            /**
             * @brief 计算代数式的近似值
             * @tparam FloatT 返回类型
             * @param [in] converter 获取变量对应的近似值的函数，参数是变量的视图，返回值是变量对应的近似值
             * @return FloatT 无字母的代数式的近似值
             * @warning 若计算中含有除以0，行为未定义
             */
            template <std::floating_point FloatT>
            [[nodiscard]] FloatT
            calculateApproximation(const std::function<FloatT(VariableView)> &converter) const noexcept;
            /**
             * @brief 清空代数式
             * @details 释放原代数式，重设为0
             */
            void                       clear() noexcept;
            /**
             * @brief 将代数式转为人类可读的字符串
             * @return std::string 人类可读的字符串
             */
            [[nodiscard]] std::string  toString() const noexcept;
            /**
             * @brief 将代数式转为人类可读的字符串
             * @return std::wstring 人类可读的字符串
             */
            [[nodiscard]] std::wstring toWString() const noexcept;
            /// @brief 将原代数式取相反数
            void                       changeToOpposite() noexcept;
            /**
             * @brief 判断代数式有无变量
             * @return true 有变量
             * @return false 没有变量
             */
            [[nodiscard]] bool         hasVariable() const noexcept;
            /**
             * @brief 判断代数式有无指定变量
             * @param [in] variable 指定变量
             * @return true 有指定变量
             * @return false 没有指定变量
             */
            [[nodiscard]] bool         hasVariable(VariableView variable) const noexcept;
            /**
             * @brief 将代数式转换为无字母的代数式（左值版本）
             * @return std::optional<NumericExpression> 转换后的无字母的代数式，若无法转换，则为 `std::nullopt`
             * @note 若 @ref hasVaraible() 的结果为 false，则返回值始终不为 `std::nullopt`；
             *       若 @ref hasVaraible() 的结果为 true，则返回值始终为 `std::nullopt`
             */
            [[nodiscard]] std::optional<NumericExpression> toNumericExpression() const & noexcept;
            /**
             * @brief 将代数式转换为无字母的代数式（右值版本）
             * @return std::optional<NumericExpression> 转换后的无字母的代数式，若无法转换，则为 `std::nullopt`
             * @note 若 @ref hasVaraible() 的结果为 false，则返回值始终不为 `std::nullopt`；
             *       若 @ref hasVaraible() 的结果为 true，则返回值始终为 `std::nullopt`
             */
            [[nodiscard]] std::optional<NumericExpression> toNumericExpression() && noexcept;
        };

        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return AlgebraicExpression 两项相加后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator+(const AlgebraicExpression &lhs,
                                                    IntegerConstantType        rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相加后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator+(IntegerConstantType        lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return AlgebraicExpression 两项相加后的副本
         */
        [[nodiscard]] [[nodiscard]] AlgebraicExpression operator+(const AlgebraicExpression &lhs,
                                                                  VariableView rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相加后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator+(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相加后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator+(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return AlgebraicExpression 两项相减后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator-(const AlgebraicExpression &lhs,
                                                    IntegerConstantType        rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相减后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator-(IntegerConstantType        lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return AlgebraicExpression 两项相减后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator-(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相减后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator-(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相减后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator-(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return AlgebraicExpression 两项相乘后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator*(const AlgebraicExpression &lhs,
                                                    IntegerConstantType        rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相乘后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator*(IntegerConstantType        lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return AlgebraicExpression 两项相乘后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator*(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相乘后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator*(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相乘后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator*(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return AlgebraicExpression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] AlgebraicExpression               operator/(const AlgebraicExpression &lhs,
                                                    IntegerConstantType        rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] AlgebraicExpression               operator/(IntegerConstantType        lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return AlgebraicExpression 两项相除后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator/(const AlgebraicExpression &lhs,
                                                    VariableView               rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相除后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator/(VariableView               lhs,
                                                    const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return AlgebraicExpression 两项相除后的副本
         */
        [[nodiscard]] AlgebraicExpression               operator/(const AlgebraicExpression &lhs,
                                                    const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 比较运算符（等号）
         * @param [in] lhs 代数式对象1
         * @param [in] rhs 代数式对象2
         * @return true 两代数式相等
         * @return false 两代数式不相等
         */
        [[nodiscard]] bool                              operator==(const AlgebraicExpression &lhs,
                                      const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 比较运算符（不等号）
         * @param [in] lhs 代数式对象1
         * @param [in] rhs 代数式对象2
         * @return true 两代数式不相等
         * @return false 两代数式相等
         */
        [[nodiscard]] bool                              operator!=(const AlgebraicExpression &lhs,
                                      const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 流输出操作符
         * @tparam CharT 输出流模板参数1
         * @tparam Traits 输出流模板参数2
         * @param [in] out 输出流
         * @param [in] rhs 代数式对象
         * @return std::basic_ostream<CharT, Traits>& 输出后的流，以便链式调用
         */
        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const AlgebraicExpression         &rhs) noexcept;

    } // namespace math

    inline namespace literals {

        /// @brief AlgebraicExpression类的内联自定义字面量命名空间
        inline namespace algebraic_expression_literals {

            /**
             * @brief 以常量字面量创建代数式类
             * @param [in] constant 常量值字面量
             * @return AlgebraicExpression 创建的代数式类
             * @see @ref math::AlgebraicExpression::AlgebraicExpression(math::IntegerConstantType constant) "AlgebraicExpression类的以常量为参数的构造函数"
             */
            [[nodiscard]] math::AlgebraicExpression
            operator""_cAlgeExpr(unsigned long long constant) noexcept;
            /**
             * @brief 以变量字面量创建代数式类
             * @param [in] variable 变量字符串字面量
             * @param [in] len 变量字符串字面量的长度
             * @return AlgebraicExpression 创建的代数式类
             * @see @ref math::AlgebraicExpression::AlgebraicExpression(math::VariableView vairable) "AlgebraicExpression类的以变量为参数的构造函数"
             */
            [[nodiscard]] math::AlgebraicExpression operator""_vAlgeExpr(const char *variable,
                                                                         std::size_t len) noexcept;

        } // namespace algebraic_expression_literals

    } // namespace literals

} // namespace tnrw

template <typename CharT>
struct std::formatter<tnrw::math::AlgebraicExpression, CharT> {
    using FmtType = tnrw::math::AlgebraicExpression; ///< 格式化参数
    using CharType = CharT;                          ///< 字符类型
    /**
     * @brief 解析格式化参数的解析器
     * @tparam ParseCtx 解析的上下文类型
     * @param [in] ctx 上下文
     * @return ParseCtx::iterator 解析后的迭代器
     */
    template <typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
        if (ctx.begin() != ctx.end() && *ctx.begin() != '}') {
            throw std::format_error("Invalid format args for tnrw::math::AlgebraicExpression!");
        }
        return ctx.begin();
    }
    /**
     * @brief 格式化器
     * @tparam ParseCtx 格式化的上下文类型
     * @param [in] alge 要格式化的对象
     * @param [in] ctx 上下文
     * @return FmtCtx::iterator 格式化后的迭代器
     */
    template <typename FmtCtx>
    typename FmtCtx::iterator format(const FmtType &alge, FmtCtx &ctx) const {
        std::string str = alge.toString();
        auto        out_it = ctx.out();
        auto       &ctype = std::use_facet<std::ctype<CharType>>(ctx.locale());
        for (char character : str) {
            *out_it = ctype.widen(character);
            ++out_it;
        }
        return out_it;
    }
};

#endif // TNRW_MATH_ALGEGRAIC_EXPRESSION_HPP
