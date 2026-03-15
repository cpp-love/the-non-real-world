/**
 * @file algebraic_expression.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了代数式类
 * @version 0.1.0-5
 * @date 2026-03-15
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

        class numeric_expression;

        /**
         * @brief 代数式类
         * @details 该类使用树状代数式，可以进行部分代数运算
         * @todo
         * - 加入根式
         */
        class algebraic_expression {
          private:
            // 数据成员
            inline static std::set<variable_type, std::ranges::less> m_vars; ///< 统一存储变量的地方
            node_ptr m_root; ///< 私有实现指针，也是代数式树的根节点

          public:
            // 友元声明
            friend bool operator==(const algebraic_expression &lhs,
                                   const algebraic_expression &rhs) noexcept;
            friend bool operator!=(const algebraic_expression &lhs,
                                   const algebraic_expression &rhs) noexcept;

            // 构造、赋值、析构
            /**
             * @brief 默认构造函数
             * @details 创建一个空的代数式
             */
            [[nodiscard]] algebraic_expression() noexcept;
            /**
             * @brief 以常量为参数的构造函数
             * @param [in] constant 常量
             */
            [[nodiscard]] explicit algebraic_expression(integer_constant_type constant) noexcept;
            /**
             * @brief 以变量为参数的构造函数
             * @param [in] variable 变量
             */
            [[nodiscard]] explicit algebraic_expression(variable_view variable) noexcept;
            /**
             * @brief 以无字母的代数式为参数的构造函数
             * @param [in] num_expr 无字母的代数式
             */
            [[nodiscard]] explicit algebraic_expression(numeric_expression num_expr) noexcept;
            /**
             * @brief 复制构造函数
             * @param [in] rhs 另一个代数式对象
             */
            [[nodiscard]] algebraic_expression(const algebraic_expression &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个代数式对象
             */
            [[nodiscard]] algebraic_expression(algebraic_expression &&rhs) noexcept;
            /**
             * @brief 以常量为参数的赋值运算符
             * @param [in] rhs 常量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression &operator=(const integer_constant_type &rhs) & noexcept;
            /**
             * @brief 以变量为参数的赋值运算符
             * @param [in] rhs 变量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression &operator=(variable_view rhs) & noexcept;
            /**
             * @brief 复制赋值运算符
             * @param [in] rhs 另一个代数式对象
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression &operator=(const algebraic_expression &rhs) & noexcept;
            /**
             * @brief 移动赋值运算符
             * @param [in] rhs 另一个代数式对象
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression &operator=(algebraic_expression &&rhs) & noexcept;
            /// @brief 析构函数
            ~algebraic_expression() noexcept;

            // 复合赋值与其他操作符
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 常量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator+=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 变量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator+=(variable_view rhs)              &noexcept;
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 另一代数式
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator+=(algebraic_expression rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 常量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator-=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 变量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator-=(variable_view rhs)              &noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 另一代数式
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator-=(algebraic_expression rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 常量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator*=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 变量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator*=(variable_view rhs)              &noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 另一代数式
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator*=(algebraic_expression rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 常量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            algebraic_expression              &operator/=(integer_constant_type rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 变量
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator/=(variable_view rhs)              &noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 另一代数式
             * @return algebraic_expression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            algebraic_expression              &operator/=(algebraic_expression rhs)              &noexcept;
            /**
             * @brief 一元加法运算符
             * @return algebraic_expression 当前对象的副本( `*this` )
             */
            [[nodiscard]] algebraic_expression operator+() const noexcept;
            /**
             * @brief 一元减法运算符
             * @return algebraic_expression 当前对象的副本取相反数( `*this` )
             */
            [[nodiscard]] algebraic_expression operator-() const noexcept;
            /**
             * @brief 后缀自增运算符
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator++() noexcept;
            /**
             * @brief 后缀自减运算符
             * @return algebraic_expression& 当前对象的引用( `*this` )
             */
            algebraic_expression              &operator--() noexcept;
            /**
             * @brief 前缀自增运算符
             * @return algebraic_expression 自增前的副本
             */
            [[nodiscard]] algebraic_expression operator++(int) noexcept;
            /**
             * @brief 前缀自减运算符
             * @return algebraic_expression 自减前的副本
             */
            [[nodiscard]] algebraic_expression operator--(int) noexcept;

            // 其他成员函数
            /**
             * @brief 获取内部指针（左值版）
             * @return const node_ptr& 内部指针
             * @warning 对返回的指针修改可能会引发错误，非必要请不要修改其内容
             */
            [[nodiscard]] const node_ptr      &get_pointer() const      &noexcept;
            /**
             * @brief 获取内部指针（右值版）
             * @return node_ptr&& 内部指针
             * @warning 对返回的指针修改可能会引发错误，非必要请不要修改其内容
             */
            [[nodiscard]] node_ptr           &&get_pointer()           &&noexcept;
            /**
             * @brief 计算代数式的近似值
             * @tparam FloatT 返回类型
             * @param [in] converter 获取变量对应的近似值的函数，参数是变量的视图，返回值是变量对应的近似值
             * @return FloatT 代数式的近似值
             * @warning 若计算中含有除以0，行为未定义
             */
            template <std::floating_point FloatT>
            [[nodiscard]] FloatT calculate_approximation(
                const std::function<FloatT(variable_view)> &converter) const noexcept;
            /**
             * @brief 计算代数式（精确值）
             * @param [in] converter 获取变量对应的精确值的函数，参数是变量的视图，返回值是变量对应的精确值
             * @return numeric_expression 代数式的精确值
             * @warning 若计算中含有除以0，行为未定义
             */
            [[nodiscard]] numeric_expression
            calculate(const std::function<numeric_expression(variable_view)> &converter) const noexcept;
            /**
             * @brief 清空代数式
             * @details 释放原代数式，重设为0
             */
            void                       clear() noexcept;
            /**
             * @brief 将代数式转为人类可读的字符串
             * @return std::string 人类可读的字符串
             */
            [[nodiscard]] std::string  to_string() const noexcept;
            /**
             * @brief 将代数式转为人类可读的字符串
             * @return std::wstring 人类可读的字符串
             */
            [[nodiscard]] std::wstring to_wstring() const noexcept;
            /// @brief 将原代数式取相反数
            void                       change_to_opposite() noexcept;
            /**
             * @brief 判断代数式有无变量
             * @return true 有变量
             * @return false 没有变量
             */
            [[nodiscard]] bool         has_variable() const noexcept;
            /**
             * @brief 判断代数式有无指定变量
             * @param [in] variable 指定变量
             * @return true 有指定变量
             * @return false 没有指定变量
             */
            [[nodiscard]] bool         has_variable(variable_view variable) const noexcept;
            /**
             * @brief 将代数式转换为无字母的代数式（左值版本）
             * @return std::optional<numeric_expression> 转换后的无字母的代数式，若无法转换，则为 `std::nullopt`
             * @note 若 @ref hasVaraible() 的结果为 false，则返回值始终不为 `std::nullopt`；
             *       若 @ref hasVaraible() 的结果为 true，则返回值始终为 `std::nullopt`
             */
            [[nodiscard]] std::optional<numeric_expression> to_numeric_expression() const & noexcept;
            /**
             * @brief 将代数式转换为无字母的代数式（右值版本）
             * @return std::optional<numeric_expression> 转换后的无字母的代数式，若无法转换，则为 `std::nullopt`
             * @note 若 @ref hasVaraible() 的结果为 false，则返回值始终不为 `std::nullopt`；
             *       若 @ref hasVaraible() 的结果为 true，则返回值始终为 `std::nullopt`
             */
            [[nodiscard]] std::optional<numeric_expression> to_numeric_expression() && noexcept;
        };

        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return algebraic_expression 两项相加后的副本
         */
        [[nodiscard]] algebraic_expression               operator+(const algebraic_expression &lhs,
                                                     integer_constant_type       rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相加后的副本
         */
        [[nodiscard]] algebraic_expression               operator+(integer_constant_type       lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return algebraic_expression 两项相加后的副本
         */
        [[nodiscard]] [[nodiscard]] algebraic_expression operator+(const algebraic_expression &lhs,
                                                                   variable_view rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相加后的副本
         */
        [[nodiscard]] algebraic_expression               operator+(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相加后的副本
         */
        [[nodiscard]] algebraic_expression               operator+(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return algebraic_expression 两项相减后的副本
         */
        [[nodiscard]] algebraic_expression               operator-(const algebraic_expression &lhs,
                                                     integer_constant_type       rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相减后的副本
         */
        [[nodiscard]] algebraic_expression               operator-(integer_constant_type       lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return algebraic_expression 两项相减后的副本
         */
        [[nodiscard]] algebraic_expression               operator-(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相减后的副本
         */
        [[nodiscard]] algebraic_expression               operator-(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相减后的副本
         */
        [[nodiscard]] algebraic_expression               operator-(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return algebraic_expression 两项相乘后的副本
         */
        [[nodiscard]] algebraic_expression               operator*(const algebraic_expression &lhs,
                                                     integer_constant_type       rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相乘后的副本
         */
        [[nodiscard]] algebraic_expression               operator*(integer_constant_type       lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return algebraic_expression 两项相乘后的副本
         */
        [[nodiscard]] algebraic_expression               operator*(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相乘后的副本
         */
        [[nodiscard]] algebraic_expression               operator*(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相乘后的副本
         */
        [[nodiscard]] algebraic_expression               operator*(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return algebraic_expression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] algebraic_expression               operator/(const algebraic_expression &lhs,
                                                     integer_constant_type       rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] algebraic_expression               operator/(integer_constant_type       lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return algebraic_expression 两项相除后的副本
         */
        [[nodiscard]] algebraic_expression               operator/(const algebraic_expression &lhs,
                                                     variable_view               rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相除后的副本
         */
        [[nodiscard]] algebraic_expression               operator/(variable_view               lhs,
                                                     const algebraic_expression &rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 代数式对象
         * @return algebraic_expression 两项相除后的副本
         */
        [[nodiscard]] algebraic_expression               operator/(const algebraic_expression &lhs,
                                                     const algebraic_expression &rhs) noexcept;

        /**
         * @brief 比较运算符（等号）
         * @param [in] lhs 代数式对象1
         * @param [in] rhs 代数式对象2
         * @return true 两代数式相等
         * @return false 两代数式不相等
         */
        [[nodiscard]] bool                               operator==(const algebraic_expression &lhs,
                                      const algebraic_expression &rhs) noexcept;
        /**
         * @brief 比较运算符（不等号）
         * @param [in] lhs 代数式对象1
         * @param [in] rhs 代数式对象2
         * @return true 两代数式不相等
         * @return false 两代数式相等
         */
        [[nodiscard]] bool                               operator!=(const algebraic_expression &lhs,
                                      const algebraic_expression &rhs) noexcept;

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
                                                      const algebraic_expression        &rhs) noexcept;

    } // namespace math

    inline namespace literals {

        /// @brief algebraic_expression类的内联自定义字面量命名空间
        inline namespace algebraic_expression_literals {

            /**
             * @brief 以常量字面量创建代数式类
             * @param [in] constant 常量值字面量
             * @return algebraic_expression 创建的代数式类
             * @see @ref math::algebraic_expression::algebraic_expression(math::integer_constant_type constant) "algebraic_expression类的以常量为参数的构造函数"
             */
            [[nodiscard]] math::algebraic_expression
            operator""_c_alge_expr(unsigned long long constant) noexcept;
            /**
             * @brief 以变量字面量创建代数式类
             * @param [in] variable 变量字符串字面量
             * @param [in] len 变量字符串字面量的长度
             * @return algebraic_expression 创建的代数式类
             * @see @ref math::algebraic_expression::algebraic_expression(math::variable_view vairable) "algebraic_expression类的以变量为参数的构造函数"
             */
            [[nodiscard]] math::algebraic_expression operator""_v_alge_expr(const char *variable,
                                                                            std::size_t len) noexcept;

        } // namespace algebraic_expression_literals

    } // namespace literals

} // namespace tnrw

template <typename CharT>
struct std::formatter<tnrw::math::algebraic_expression, CharT> {
    using fmt_type = tnrw::math::algebraic_expression; ///< 格式化参数
    using char_type = CharT;                           ///< 字符类型
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
     * @param [in] alge 要格式化的对象
     * @param [in] ctx 上下文
     * @return FmtCtx::iterator 格式化后的迭代器
     */
    template <typename FmtCtx>
    typename FmtCtx::iterator format(const fmt_type &alge, FmtCtx &ctx) const {
        std::string str = alge.to_string();
        auto        out_it = ctx.out();
        auto       &ctype = std::use_facet<std::ctype<char_type>>(ctx.locale());
        for (char character : str) {
            *out_it = ctype.widen(character);
            ++out_it;
        }
        return out_it;
    }
};

#endif // TNRW_MATH_ALGEGRAIC_EXPRESSION_HPP
