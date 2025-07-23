/**
 * @file Maths_base.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了一些数学的基本元素
 * @version 0.1.0-1
 * @date 2025-07-23
 * 
 * @copyright Copyright 2025 cpp-love
 * 
 * @details
 * - 声明了 `ConstantType` 和 `VariableType` 来表示常量与变量类型
 * 
 */

#ifndef __MATHS_MATHS_BASE_HPP__
#define __MATHS_MATHS_BASE_HPP__

/**
 * @brief "The Non-real World" 游戏的命名空间
 * @details 此游戏的所有API都在此命名空间内
 */
namespace tnrw {

    /**
     * @brief 数学相关的命名空间
     * @details
     * - 为了方便游戏的血量、攻击系统的开发，先行开发作为血条值的一些相关类
     * - 同时也为了方便游戏的开发，先行定义一些数学相关的函数
     */
    namespace Maths {

        // using别名
        using ConstantType = long long; ///< 常量值类型
        using VariableType = char;      ///< 变量字符类型

    } // namespace Maths

    namespace literals {

        /// @brief Maths的基础内联自定义字面量命名空间
        inline namespace Maths_base_literals {
            /**
             * @brief 创建常量
             * @param [in] constant 常量值字面量
             * @return Maths::ConstantType 创建的常量类型
             */
            Maths::ConstantType operator""_c(const unsigned long long constant) noexcept;
            /**
             * @brief 创建变量
             * @param [in] variable 变量字符字面量
             * @return Maths::VariableType 创建的变量类型
             */
            Maths::VariableType operator""_v(const char variable) noexcept;
        } // namespace Maths_base_literals

    } // namespace literals

} // namespace tnrw

#endif