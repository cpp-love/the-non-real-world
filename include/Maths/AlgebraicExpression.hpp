/**
 * @file AlgebraicExpression.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了代数式类
 * @version 0.1.0-1
 * @date 2025-07-05
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

#ifndef __MATHS_ALGEGRAIC_EXPRESSION_HPP__
#define __MATHS_ALGEGRAIC_EXPRESSION_HPP__

#include "Maths/Expressions_base.hpp"
#include <map>
#include <memory>

namespace tnrw {

    namespace Maths {

        namespace Details {
            /**
             * @brief  `Pimpl` 惯用法实现 `AlgebraicExpression` 类和 `NumericExpression` 类的私有封装（前向声明）
             * @details
             * - 所有私有成员都隐藏在此结构体的完整定义中，
             *   具体定义见源文件 @ref `Expressions.cpp`
             * - 此结构体也是代数式树的节点定义
             * @warning 该结构体是私有的，用户不应直接访问
             */
            struct Node;
        } // namespace Details

        /**
         * @brief 代数式类
         * @details 该类使用树状代数式，可以进行部分代数运算
         * @todo
         * - 加入根式
         * - 完备代数运算
         */
        class AlgebraicExpression final {
            /// @cond INTERNAL
          private: /// @privatesection
            // 成员变量
            std::unique_ptr<Details::Node> m_root; ///< 私有实现指针，也是代数式树的根节点

            /// @endcond
          public: /// @publicsection
            // 友元声明
            friend bool operator==(const AlgebraicExpression &lhs,
                                   const AlgebraicExpression &rhs) noexcept;
            friend bool operator!=(const AlgebraicExpression &lhs,
                                   const AlgebraicExpression &rhs) noexcept;

            // using别名
            using ConstantType = ConstantType; ///< 常量值类型
            using VariableType = VariableType; ///< 变量字符类型
            template <typename T>
            using VariableDictType = std::map<VariableType, T>; ///< 变量字典类型

            // 构造、赋值、析构
            /**
             * @brief 默认构造函数
             * @details 创建一个空的代数式
             */
            AlgebraicExpression() noexcept;
            /**
             * @brief 以常量为参数的构造函数
             * @param [in] constant 常量值
             */
            AlgebraicExpression(const ConstantType constant) noexcept;
            /**
             * @brief 以变量为参数的构造函数
             * @param [in] vairable 变量字符
             */
            AlgebraicExpression(const VariableType vairable) noexcept;
            /**
             * @brief 析构函数
             * @details 释放代数式资源
             */
            ~AlgebraicExpression() noexcept;
            /**
             * @brief 复制构造函数
             * @param [in] rhs 另一个代数式对象
             * @details 创建一个代数式的副本
             */
            AlgebraicExpression(const AlgebraicExpression &rhs) noexcept;
            /**
             * @brief 移动构造函数
             * @param [in] rhs 另一个代数式对象
             * @details 转移代数式的所有权
             */
            AlgebraicExpression(AlgebraicExpression &&rhs) noexcept;
            /**
             * @brief 复制赋值运算符
             * @param [in] rhs 另一个代数式对象
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             * @details 用另一个对象的内容覆盖本对象的内容
             */
            AlgebraicExpression &operator=(const AlgebraicExpression &rhs) noexcept;
            /**
             * @brief 移动赋值运算符
             * @param [in] rhs 另一个代数式对象
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             * @details 用另一个对象的内容覆盖本对象的内容，并置空另一个对象的内容
             */
            AlgebraicExpression &operator=(AlgebraicExpression &&rhs) noexcept;

            // 复合赋值与其他操作符
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator+=(const ConstantType rhs) noexcept;
            /**
             * @brief 加法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator+=(const VariableType rhs) noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator-=(const ConstantType rhs) noexcept;
            /**
             * @brief 减法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator-=(const VariableType rhs) noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator*=(const ConstantType rhs) noexcept;
            /**
             * @brief 乘法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator*=(const VariableType rhs) noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 常量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             * @warning 除以0行为未定义
             */
            AlgebraicExpression &operator/=(const ConstantType rhs) noexcept;
            /**
             * @brief 除法赋值运算符
             * @param [in] rhs 变量
             * @return AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator/=(const VariableType rhs) noexcept;
            /**
             * @brief 一元加法运算符
             * @return AlgebraicExpression 当前对象的副本( `*this` )
             */
            AlgebraicExpression operator+() noexcept;
            /**
             * @brief 一元减法运算符
             * @return AlgebraicExpression 当前对象取反的副本( `*this` )
             */
            AlgebraicExpression operator-() noexcept;
            /**
             * @brief 后缀自增运算符
             * @return const AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator++() noexcept;
            /**
             * @brief 后缀自减运算符
             * @return const AlgebraicExpression& 当前对象的引用( `*this` )
             */
            AlgebraicExpression &operator--() noexcept;
            /**
             * @brief 前缀自增运算符
             * @return AlgebraicExpression 自增前的副本
             */
            const AlgebraicExpression operator++(int) noexcept;
            /**
             * @brief 前缀自减运算符
             * @return AlgebraicExpression 自减前的副本
             */
            const AlgebraicExpression operator--(int) noexcept;

            // 其他成员函数
            /**
             * @brief 判断代数式是否为0
             * @tparam T 字典值类型
             * @param [in] dict 变量对应的值的字典
             * @return true 代数式为0
             * @return false 代数式不为0
             * @note 该方法会精准判断代数式是否为0，所以字典类型应完整且精确
             * @warning 对于未在字典中出现的变量 `a`，根据 `std::map::operator[](a)` 返回值带入（使用默认赋值）
             */
            template <typename T>
            bool isZero(const VariableDictType<T> &dict) const noexcept;
            /**
             * @brief 计算并获取代数式的值
             * @tparam T 字典值类型
             * @param [in] dict 变量对应的值的字典
             * @return T 代数式的值
             * @warning 对于未在字典中出现的变量 `a`，根据 `std::map::operator[](a)` 返回值带入（使用默认赋值）
             */
            template <typename T>
            T getValue(const VariableDictType<T> &dict) const noexcept;
            /**
             * @brief 清空代数式
             * @details 释放原代数式，重设为0
             */
            void clear() noexcept;
            /**
             * @brief 将代数式转为人类可读的字符串
             * @return std::string 人类可读的字符串
             */
            std::string toString() const noexcept;
            /**
             * @brief 将代数式转为人类可读的字符串
             * @return std::wstring 人类可读的字符串
             */
            std::wstring toWString() const noexcept;
            /**
             * @brief 将原代数式取相反数
             */
            void changeToOpposite() noexcept;
        };

        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return const AlgebraicExpression 两项相加后的副本
         */
        const AlgebraicExpression
        operator+(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::ConstantType rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相加后的副本
         */
        const AlgebraicExpression operator+(const AlgebraicExpression::ConstantType lhs,
                                            const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 加法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return const AlgebraicExpression 两项相加后的副本
         */
        const AlgebraicExpression
        operator+(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::VariableType rhs) noexcept;
        /**
         * @brief 加法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相加后的副本
         */
        const AlgebraicExpression operator+(const AlgebraicExpression::VariableType lhs,
                                            const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return const AlgebraicExpression 两项相减后的副本
         */
        const AlgebraicExpression
        operator-(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::ConstantType rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相减后的副本
         */
        const AlgebraicExpression operator-(const AlgebraicExpression::ConstantType lhs,
                                            const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 减法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return const AlgebraicExpression 两项相减后的副本
         */
        const AlgebraicExpression
        operator-(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::VariableType rhs) noexcept;
        /**
         * @brief 减法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相减后的副本
         */
        const AlgebraicExpression operator-(const AlgebraicExpression::VariableType lhs,
                                            const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return const AlgebraicExpression 两项相乘后的副本
         */
        const AlgebraicExpression
        operator*(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::ConstantType rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相乘后的副本
         */
        const AlgebraicExpression operator*(const AlgebraicExpression::ConstantType lhs,
                                            const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 乘法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return const AlgebraicExpression 两项相乘后的副本
         */
        const AlgebraicExpression
        operator*(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::VariableType rhs) noexcept;
        /**
         * @brief 乘法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相乘后的副本
         */
        const AlgebraicExpression operator*(const AlgebraicExpression::VariableType lhs,
                                            const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 常量
         * @return const AlgebraicExpression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        const AlgebraicExpression
        operator/(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::ConstantType rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 常量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相除后的副本
         * @warning 除以0行为未定义
         */
        const AlgebraicExpression operator/(const AlgebraicExpression::ConstantType lhs,
                                            const AlgebraicExpression &rhs) noexcept;

        /**
         * @brief 除法运算符
         * @param [in] lhs 代数式对象
         * @param [in] rhs 变量
         * @return const AlgebraicExpression 两项相除后的副本
         */
        const AlgebraicExpression
        operator/(const AlgebraicExpression              &lhs,
                  const AlgebraicExpression::VariableType rhs) noexcept;
        /**
         * @brief 除法运算符
         * @param [in] lhs 变量
         * @param [in] rhs 代数式对象
         * @return const AlgebraicExpression 两项相除后的副本
         */
        const AlgebraicExpression operator/(const AlgebraicExpression::VariableType lhs,
                                            const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 比较运算符（等号）
         * @param [in] lhs 代数式对象1
         * @param [in] rhs 代数式对象2
         * @return true 两运算符相等
         * @return false 两运算符不相等
         */
        bool operator==(const AlgebraicExpression &lhs,
                        const AlgebraicExpression &rhs) noexcept;
        /**
         * @brief 比较运算符（不等号）
         * @param [in] lhs 代数式对象1
         * @param [in] rhs 代数式对象2
         * @return true 两运算符不相等
         * @return false 两运算符相等
         */
        bool operator!=(const AlgebraicExpression &lhs,
                        const AlgebraicExpression &rhs) noexcept;

    } // namespace Maths

    namespace literals {

        /// @brief AlgebraicExpression类的内联自定义字面量命名空间
        inline namespace AlgebraicExpression_literals {

            /**
             * @brief 以常量字面量创建代数式类
             * @param [in] constant 常量值字面量
             * @return AlgebraicExpression 创建的代数式类
             * @see @ref Maths::AlgebraicExpression::AlgebraicExpression(Maths::AlgebraicExpression::ConstantType constant) "AlgebraicExpression类的以常量为参数的构造函数"
             */
            Maths::AlgebraicExpression
            operator""_cAlgeExpr(const unsigned long long constant) noexcept;
            /**
             * @brief 以变量字面量创建代数式类
             * @param [in] variable 变量字符字面量
             * @return AlgebraicExpression 创建的代数式类
             * @see @ref Maths::AlgebraicExpression::AlgebraicExpression(Maths::AlgebraicExpression::VariableType vairable) "AlgebraicExpression类的以变量为参数的构造函数"
             */
            Maths::AlgebraicExpression operator""_vAlgeExpr(const char variable) noexcept;

        } // namespace AlgebraicExpression_literals

    } // namespace literals

} // namespace tnrw

#endif // __MATHS_ALGEGRAIC_EXPRESSION_HPP__
