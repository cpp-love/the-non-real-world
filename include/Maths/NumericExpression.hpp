/**
 * @file NumericExpression.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了无字母的代数式类
 * @version 0.1.0-1
 * @date 2025-07-26
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

#ifndef __TNRW_MATHS_NUMERIC_EXPRESSION_HPP__
#define __TNRW_MATHS_NUMERIC_EXPRESSION_HPP__

#include "Maths/Expressions_base.hpp"
#include <iosfwd>
#include <locale>
#include <memory>
#include <string>

namespace tnrw {

    namespace Maths {

        namespace Details {
            /**
             * @brief  `Pimpl` 惯用法实现 `NumericExpression` 类和 `NumericExpression` 类的私有封装（前向声明）
             * @details
             * - 所有私有成员都隐藏在此结构体的完整定义中，
             *   具体定义见源文件 @ref `Expressions.cpp`
             * - 此结构体也是代数式树的节点定义
             * @warning 该结构体是私有的，用户不应直接访问
             */
            struct Node;
        } // namespace Details

        /**
         * @brief 无字母的代数式类
         * @details 该类使用树状代数式，可以进行部分代数运算
         * @todo
         * - 加入根式
         * - 完备代数运算
         */
        class NumericExpression final {
            /// @cond INTERNAL
          private: /// @privatesection
            // 成员变量
            std::unique_ptr<Details::Node> m_root; ///< 私有实现指针，也是代数式树的根节点

            /// @endcond
          public: /// @publicsection
            // 友元声明
            friend bool operator==(const NumericExpression &lhs, const NumericExpression &rhs) noexcept;
            friend bool operator!=(const NumericExpression &lhs, const NumericExpression &rhs) noexcept;

            // using别名
            using ConstantType = ConstantType; ///< 常量值类型

            // 构造、赋值、析构
            /**
             * @brief 默认构造函数
             * @details 创建一个空的无字母的代数式
             */
            [[nodiscard]] NumericExpression() noexcept;
            /**
             * @brief 以常量为参数的构造函数
             * @param [in] constant 常量值
             */
            [[nodiscard]] explicit NumericExpression(ConstantType constant) noexcept;
            /**
             * @brief 析构函数
             * @details 释放无字母的代数式资源
             */
            ~NumericExpression() noexcept;
            /**
             * @brief 复制构造函数
             * @param [in] rhs 另一个无字母的代数式对象
             * @details 创建一个无字母的代数式的副本
             */
            [[nodiscard]] NumericExpression(const NumericExpression &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个无字母的代数式对象
             * @details 转移无字母的代数式的所有权
             */
            [[nodiscard]] NumericExpression(NumericExpression &&rhs) noexcept;
            /**
             * @brief 复制赋值运算符
             * @param [in] rhs 另一个无字母的代数式对象
             * @return NumericExpression& 当前对象的引用( `*this` )
             * @details 用另一个对象的内容覆盖本对象的内容
             */
            NumericExpression                    &operator=(const NumericExpression &rhs) noexcept;
            /**
             * @brief 移动赋值运算符
             * @param [in] rhs 另一个无字母的代数式对象
             * @return NumericExpression& 当前对象的引用( `*this` )
             * @details 用另一个对象的内容覆盖本对象的内容，并置空另一个对象的内容
             */
            NumericExpression                    &operator=(NumericExpression &&rhs) noexcept;

            // 复合赋值与其他操作符
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 常量
             * @return NumericExpression& 当前对象的引用( `*this` )
             */
            NumericExpression                    &operator+=(ConstantType rhs) noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 常量
             * @return NumericExpression& 当前对象的引用( `*this` )
             */
            NumericExpression                    &operator-=(ConstantType rhs) noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 常量
             * @return NumericExpression& 当前对象的引用( `*this` )
             */
            NumericExpression                    &operator*=(ConstantType rhs) noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 常量
             * @return NumericExpression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            NumericExpression                    &operator/=(ConstantType rhs) noexcept;
            /**
             * @brief 一元加法运算符
             * @return NumericExpression 当前对象的副本( `*this` )
             */
            [[nodiscard]] NumericExpression       operator+() noexcept;
            /**
             * @brief 一元减法运算符
             * @return NumericExpression 当前对象取反的副本( `*this` )
             */
            [[nodiscard]] NumericExpression       operator-() noexcept;
            /**
             * @brief 后缀自增运算符
             * @return const NumericExpression& 当前对象的引用( `*this` )
             */
            NumericExpression                    &operator++() noexcept;
            /**
             * @brief 后缀自减运算符
             * @return const NumericExpression& 当前对象的引用( `*this` )
             */
            NumericExpression                    &operator--() noexcept;
            /**
             * @brief 前缀自增运算符
             * @return NumericExpression 自增前的副本
             */
            [[nodiscard]] const NumericExpression operator++(int) noexcept;
            /**
             * @brief 前缀自减运算符
             * @return NumericExpression 自减前的副本
             */
            [[nodiscard]] const NumericExpression operator--(int) noexcept;

            // 其他成员函数
            /**
             * @brief 判断无字母的代数式是否为0
             * @return true 无字母的代数式为0
             * @return false 无字母的代数式不为0
             * @note 该方法会精准判断无字母的代数式是否为0
             */
            [[nodiscard]] bool                    isZero() const noexcept;
            /**
             * @brief 计算并获取无字母的代数式的值
             * @tparam T 返回类型
             * @return T 无字母的代数式的值
             */
            template <typename T>
            [[nodiscard]] T            getValue() const noexcept;
            /**
             * @brief 清空无字母的代数式
             * @details 释放原无字母的代数式，重设为0
             */
            void                       clear() noexcept;
            /**
             * @brief 将无字母的代数式转为人类可读的字符串
             * @param [in] loc 可能的 `std::locale` 配置，可省略
             * @return std::string 人类可读的字符串
             */
            [[nodiscard]] std::string  toString(const std::locale &loc = std::locale()) const noexcept;
            /**
             * @brief 将无字母的代数式转为人类可读的字符串
             * @param [in] loc 可能的 `std::locale` 配置，可省略
             * @return std::wstring 人类可读的字符串
             */
            [[nodiscard]] std::wstring toWString(const std::locale &loc = std::locale()) const noexcept;
            /**
             * @brief 将无字母的代数式转为人类可读的字符串
             * @tparam CharT 字符串模板参数1
             * @tparam Traits 字符串模板参数2
             * @param [in] loc 可能的 `std::locale` 配置，可省略
             * @return std::basic_string<CharT, Traits> 人类可读的字符串
             */
            template <typename CharT, typename Traits = std::char_traits<CharT>>
            [[nodiscard]] std::basic_string<CharT, Traits>
                 toBasicString(const std::locale &loc = std::locale()) const noexcept;
            /**
             * @brief 将原无字母的代数式取相反数
             */
            void changeToOpposite() noexcept;
        };

        /**
         * @brief 加法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return const NumericExpression 两项相加后的副本
         */
        [[nodiscard]] const NumericExpression operator+(const NumericExpression        &lhs,
                                                        NumericExpression::ConstantType rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return const NumericExpression 两项相加后的副本
         */
        [[nodiscard]] const NumericExpression operator+(NumericExpression::ConstantType lhs,
                                                        const NumericExpression        &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return const NumericExpression 两项相减后的副本
         */
        [[nodiscard]] const NumericExpression operator-(const NumericExpression        &lhs,
                                                        NumericExpression::ConstantType rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return const NumericExpression 两项相减后的副本
         */
        [[nodiscard]] const NumericExpression operator-(NumericExpression::ConstantType lhs,
                                                        const NumericExpression        &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return const NumericExpression 两项相乘后的副本
         */
        [[nodiscard]] const NumericExpression operator*(const NumericExpression        &lhs,
                                                        NumericExpression::ConstantType rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return const NumericExpression 两项相乘后的副本
         */
        [[nodiscard]] const NumericExpression operator*(NumericExpression::ConstantType lhs,
                                                        const NumericExpression        &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 无字母的代数式对象
         * @param [in] rhs 常量
         * @return const NumericExpression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] const NumericExpression operator/(const NumericExpression        &lhs,
                                                        NumericExpression::ConstantType rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 无字母的代数式对象
         * @return const NumericExpression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        [[nodiscard]] const NumericExpression operator/(NumericExpression::ConstantType lhs,
                                                        const NumericExpression        &rhs) noexcept;

        /**
         * @brief 比较运算符（等号）
         * @param [in] lhs 无字母的代数式对象1
         * @param [in] rhs 无字母的代数式对象2
         * @return true 两无字母的代数式相等
         * @return false 两无字母的代数式不相等
         */
        [[nodiscard]] bool                    operator==(const NumericExpression &lhs,
                                      const NumericExpression &rhs) noexcept;
        /**
         * @brief 比较运算符（不等号）
         * @param [in] lhs 无字母的代数式对象1
         * @param [in] rhs 无字母的代数式对象2
         * @return true 两无字母的代数式不相等
         * @return false 两无字母的代数式相等
         */
        [[nodiscard]] bool                    operator!=(const NumericExpression &lhs,
                                      const NumericExpression &rhs) noexcept;
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
                                                      const NumericExpression           &rhs) noexcept;

    } // namespace Maths

    inline namespace literals {

        /// @brief NumericExpression类的内联自定义字面量命名空间
        inline namespace NumericExpression_literals {

            /**
             * @brief 以常量字面量创建无字母的代数式类
             * @param [in] constant 常量值字面量
             * @return NumericExpression 创建的无字母的代数式类
             * @see @ref Maths::NumericExpression::NumericExpression(Maths::NumericExpression::ConstantType constant) "NumericExpression类的以常量为参数的构造函数"
             */
            [[nodiscard]] Maths::NumericExpression
            operator""_cNumExpr(unsigned long long constant) noexcept;

        } // namespace NumericExpression_literals

    } // namespace literals

} // namespace tnrw

#endif // __TNRW_MATHS_NUMERIC_EXPRESSION_HPP__
