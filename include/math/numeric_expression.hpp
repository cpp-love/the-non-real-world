/**
 * @file numeric_expression.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了无字母的代数式类
 * @version 0.1.0-4
 * @date 2026-03-14
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 使用 **树** 这一数据结构来表示无字母的代数式
 * - 声明了代数式与常量和变量的加、减、乘、除法运算符重载及其复合赋值运算符重载
 * - 声明了代数式相关的其他成员方法
 * - 声明了将常量字面量转为无字母的代数式的自定义字面量
 * 
 */

#ifndef TNRW_MATH_NUMERIC_EXPRESSION_HPP
#define TNRW_MATH_NUMERIC_EXPRESSION_HPP

#include "math/expressions_base.hpp"
#include <concepts>
#include <format>
#include <iosfwd>
#include <locale>
#include <string>

namespace tnrw {

    namespace math {

        /**
         * @brief 无字母的代数式类
         * @details 该类使用树状代数式，可以进行部分代数运算
         * @todo
         * - 加入根式
         * - 完备代数运算
         */
        class numeric_expression {
          private:
            // 数据成员
            node_ptr m_root; ///< 私有实现指针，也是代数式树的根节点

          public:
            // 友元声明
            friend bool operator==(const numeric_expression &lhs,
                                   const numeric_expression &rhs) noexcept;
            friend bool operator!=(const numeric_expression &lhs,
                                   const numeric_expression &rhs) noexcept;
            friend class algebraic_expression;

            // 构造、赋值、析构
            /**
             * @brief 默认构造函数
             * @details 创建一个空的无字母的代数式
             */
            [[nodiscard]] numeric_expression() noexcept;
            /**
             * @brief 以常量为参数的构造函数
             * @param [in] constant 常量值
             */
            [[nodiscard]] explicit numeric_expression(integer_constant_type constant) noexcept;
            /**
             * @brief 析构函数
             */
            ~numeric_expression() noexcept;
            /**
             * @brief 复制构造函数
             * @param [in] rhs 另一个无字母的代数式对象
             */
            [[nodiscard]] numeric_expression(const numeric_expression &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个无字母的代数式对象
             */
            [[nodiscard]] numeric_expression(numeric_expression &&rhs) noexcept;
            /**
             * @brief 复制赋值运算符
             * @param [in] rhs 另一个无字母的代数式对象
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator=(const numeric_expression &rhs)              &noexcept;
            /**
             * @brief 移动赋值运算符
             * @param [in] rhs 另一个无字母的代数式对象
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator=(numeric_expression &&rhs)              &noexcept;

            // 复合赋值与其他操作符
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 常量
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator+=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 另一个无字母的代数式
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator+=(const numeric_expression &rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 常量
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator-=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 另一个无字母的代数式
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator-=(const numeric_expression &rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 常量
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator*=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 另一个无字母的代数式
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator*=(const numeric_expression &rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 常量
             * @return numeric_expression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            numeric_expression              &operator/=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 另一个无字母的代数式
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator/=(const numeric_expression &rhs)              &noexcept;
            /**
             * @brief 一元加法运算符
             * @return numeric_expression 当前对象的副本( `*this` )
             */
            [[nodiscard]] numeric_expression operator+() const noexcept;
            /**
             * @brief 一元减法运算符
             * @return numeric_expression 当前对象的副本取相反数( `*this` )
             */
            [[nodiscard]] numeric_expression operator-() const noexcept;
            /**
             * @brief 后缀自增运算符
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator++() noexcept;
            /**
             * @brief 后缀自减运算符
             * @return numeric_expression& 当前对象的引用( `*this` )
             */
            numeric_expression              &operator--() noexcept;
            /**
             * @brief 前缀自增运算符
             * @return numeric_expression 自增前的副本
             */
            [[nodiscard]] numeric_expression operator++(int) noexcept;
            /**
             * @brief 前缀自减运算符
             * @return numeric_expression 自减前的副本
             */
            [[nodiscard]] numeric_expression operator--(int) noexcept;

            // 其他成员函数
            /**
             * @brief 计算无字母的代数式的近似值
             * @tparam FloatT 返回类型
             * @return FloatT 无字母的代数式的近似值
             */
            template <std::floating_point FloatT>
            [[nodiscard]] FloatT       calculate_approximation() const noexcept;
            /**
             * @brief 清空无字母的代数式
             * @details 释放原无字母的代数式，重设为0
             */
            void                       clear() noexcept;
            /**
             * @brief 将无字母的代数式转为人类可读的字符串
             * @return std::string 人类可读的字符串
             */
            [[nodiscard]] std::string  to_string() const noexcept;
            /**
             * @brief 将无字母的代数式转为人类可读的字符串
             * @return std::wstring 人类可读的字符串
             */
            [[nodiscard]] std::wstring to_wstring() const noexcept;
            /// @brief 将原无字母的代数式取相反数
            void                       change_to_opposite() noexcept;
        };

        /**
         * @brief 加法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return numeric_expression 两项相加后的副本
         */
        [[nodiscard]] numeric_expression operator+(const numeric_expression &lhs,
                                                   integer_constant_type     rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相加后的副本
         */
        [[nodiscard]] numeric_expression operator+(integer_constant_type     lhs,
                                                   const numeric_expression &rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相加后的副本
         */
        [[nodiscard]] numeric_expression operator+(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return numeric_expression 两项相减后的副本
         */
        [[nodiscard]] numeric_expression operator-(const numeric_expression &lhs,
                                                   integer_constant_type     rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相减后的副本
         */
        [[nodiscard]] numeric_expression operator-(integer_constant_type     lhs,
                                                   const numeric_expression &rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相减后的副本
         */
        [[nodiscard]] numeric_expression operator-(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return numeric_expression 两项相乘后的副本
         */
        [[nodiscard]] numeric_expression operator*(const numeric_expression &lhs,
                                                   integer_constant_type     rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相乘后的副本
         */
        [[nodiscard]] numeric_expression operator*(integer_constant_type     lhs,
                                                   const numeric_expression &rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相乘后的副本
         */
        [[nodiscard]] numeric_expression operator*(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return numeric_expression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] numeric_expression operator/(const numeric_expression &lhs,
                                                   integer_constant_type     rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] numeric_expression operator/(integer_constant_type     lhs,
                                                   const numeric_expression &rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 无字母的代数式对象
         * @return numeric_expression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] numeric_expression operator/(const numeric_expression &lhs,
                                                   const numeric_expression &rhs) noexcept;

        /**
         * @brief 比较运算符（等号）
         * @param [in] lhs 无字母的代数式对象1
         * @param [in] rhs 无字母的代数式对象2
         * @return true 两无字母的代数式相等
         * @return false 两无字母的代数式不相等
         */
        [[nodiscard]] bool               operator==(const numeric_expression &lhs,
                                      const numeric_expression &rhs) noexcept;
        /**
         * @brief 比较运算符（不等号）
         * @param [in] lhs 无字母的代数式对象1
         * @param [in] rhs 无字母的代数式对象2
         * @return true 两无字母的代数式不相等
         * @return false 两无字母的代数式相等
         */
        [[nodiscard]] bool               operator!=(const numeric_expression &lhs,
                                      const numeric_expression &rhs) noexcept;

        /**
         * @brief 流输出操作符
         * @tparam CharT 输出流模板参数1
         * @tparam Traits 输出流模板参数2
         * @param [in] out 输出流
         * @param [in] rhs 无字母的代数式对象
         * @return std::basic_ostream<CharT, Traits>& 输出后的流，以便链式调用
         */
        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                                      const numeric_expression          &rhs) noexcept;

    } // namespace math

    inline namespace literals {

        /// @brief numeric_expression类的内联自定义字面量命名空间
        inline namespace numeric_expression_literals {

            /**
             * @brief 以常量字面量创建无字母的代数式类
             * @param [in] constant 常量值字面量
             * @return numeric_expression 创建的无字母的代数式类
             * @see @ref math::numeric_expression::numeric_expression(math::integer_constant_type constant) "numeric_expression类的以常量为参数的构造函数"
             */
            [[nodiscard]] math::numeric_expression
            operator""_c_num_expr(unsigned long long constant) noexcept;

        } // namespace numeric_expression_literals

    } // namespace literals

} // namespace tnrw

template <typename CharT>
struct std::formatter<tnrw::math::numeric_expression, CharT> {
    using fmt_type = tnrw::math::numeric_expression; ///< 格式化参数
    using char_type = CharT;                         ///< 字符类型
    /**
     * @brief 解析格式化参数的解析器
     * @tparam ParseCtx 解析的上下文类型
     * @param [in] ctx 上下文
     * @return ParseCtx::iterator 解析后的迭代器
     */
    template <typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
        if (ctx.begin() != ctx.end() && *ctx.begin() != '}') {
            throw std::format_error("Invalid format args for tnrw::math::algebraic_expression!");
        }
        return ctx.begin();
    }
    /**
     * @brief 格式化器
     * @tparam ParseCtx 格式化的上下文类型
     * @param [in] num 要格式化的对象
     * @param [in] ctx 上下文
     * @return FmtCtx::iterator 格式化后的迭代器
     */
    template <typename FmtCtx>
    typename FmtCtx::iterator format(const fmt_type &num, FmtCtx &ctx) const {
        std::string str = num.to_string();
        auto        out_it = ctx.out();
        auto       &ctype = std::use_facet<std::ctype<char_type>>(ctx.locale());
        for (char character : str) {
            *out_it = ctype.widen(character);
            ++out_it;
        }
        return out_it;
    }
};

#endif // TNRW_MATH_NUMERIC_EXPRESSION_HPP
