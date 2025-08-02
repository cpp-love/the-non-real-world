/**
 * @file Complex.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 
 * @version 0.1.0-1
 * @date 2025-08-02
 * 
 * @copyright Copyright cpp-love
 * 
 * @details 
 * - 定义了复数类模板（支持自定义类型）
 * - 定义了复数类模板的运算
 * - 定义了虚数转为复数类模板的自定义字面量
 * 
 */

#ifndef __MATHS_COMPLEX_HPP__
#define __MATHS_COMPLEX_HPP__

namespace tnrw {

    namespace Maths {

        template <typename T>
        class Complex {
          public:                 /// @publicsection
            using value_type = T; ///< 值的类型

            /// @cond INTERNAL
          private:                  /// @privatesection
            value_type m_real;      ///< 实部
            value_type m_imaginary; ///< 虚部

            /// @endcond
          public: /// @publicsection
            // 构造、赋值、析构
            constexpr Complex() : m_real{}, m_imaginary{} {}
            constexpr Complex(const value_type &real) : m_real(real), m_imaginary{} {}
            constexpr Complex(const value_type &real, const value_type &imag)
                : m_real(real), m_imaginary(imag) {}
            constexpr Complex(const Complex &rhs)
                : m_real(rhs.m_real), m_imaginary(rhs.m_imaginary) {}
            Complex(Complex &&rhs)
                : m_real(std::move(rhs.m_real)), m_imaginary(std::move(rhs.m_imaginary)) {
            }
            constexpr operator=(const Complex &rhs) {
                m_real = rhs.m_real;
                m_imaginary = rhs.m_imaginary;
            }
            constexpr operator=(Complex &&rhs) {
                m_real = std::move(rhs.m_real);
                m_imaginary = std::move(rhs.m_imaginary);
            }

            // 复合赋值与其他操作符

            // 其他成员函数
                };

    } // namespace Maths

} // namespace tnrw

#endif