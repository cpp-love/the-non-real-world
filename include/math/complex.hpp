/**
 * @file complex.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 通用的复数实现，与 `std::complex` 相似
 * @version 0.1.0-1
 * @date 2026-03-21
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_MATH_COMPLEX_HPP
#define TNRW_MATH_COMPLEX_HPP

#include <concepts>
#include <type_traits>

namespace tnrw::math {

    /**
     * @brief 通用的复数实现
     * @tparam T 复数的实部类型和虚部类型
     */
    template <typename T>
        requires requires(T lhs, T rhs) {
            { std::remove_cvref_t<decltype(lhs + rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs - rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs * rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs / rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs += rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs -= rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs *= rhs)>{} } -> std::same_as<T>;
            { std::remove_cvref_t<decltype(lhs /= rhs)>{} } -> std::same_as<T>;
        }
    class complex {
      private:
        T m_real;
        T m_imag;

      public:
        // 构造、赋值、析构
        // NOLINTBEGIN(bugprone-easily-swappable-parameters, hicpp-explicit-conversions)，为了兼容 `std::complex`
        /**
         * @brief 构建一个 complex 对象
         * @param [in] real 实部
         * @param [in] imag 虚部
         */
        constexpr complex(T real = T{}, T imag = T{}) noexcept(std::is_nothrow_move_constructible_v<T>)
            : m_real(std::move(real)), m_imag(std::move(imag)) {}
        // NOLINTEND(bugprone-easily-swappable-parameters, hicpp-explicit-conversions)，为了兼容 `std::complex`
        /**
         * @brief 构建一个 complex 对象
         * @param [in] rhs 另一个对象
         */
        constexpr complex(const complex &rhs) = default;
        /**
         * @brief 构建一个 complex 对象
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         */
        template <class U>
        constexpr explicit complex(const complex<U> &rhs) noexcept(
            std::is_nothrow_constructible_v<T, const U &>)
            : m_real(rhs.real()), m_imag(rhs.imag()) {}
        /**
         * @brief 构建一个 complex 对象
         * @param [in] rhs 另一个对象
         */
        constexpr complex(complex &&) = default;
        /**
         * @brief 构建一个 complex 对象
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         */
        template <class U>
        constexpr explicit complex(complex<U> &&rhs) noexcept(std::is_nothrow_constructible_v<T, U &&>)
            : m_real(std::move(rhs).real()), m_imag(std::move(rhs).imag()) {}

        /**
         * @brief 复制赋值运算符重载
         * @param [in] real 实部
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator=(T rhs) & noexcept(std::is_nothrow_move_assignable_v<T>) {
            m_real = std::move(rhs);
            m_imag = T{};
            return *this;
        }
        /**
         * @brief 复制赋值运算符重载
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator=(const complex &rhs) & = default;
        /**
         * @brief 复制赋值运算符重载
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        template <typename U>
        constexpr complex &
        operator=(const complex<U> &rhs) & noexcept(std::is_nothrow_assignable_v<T &, const U &>) {
            m_real = rhs.real();
            m_imag = rhs.imag();
            return *this;
        }
        /**
         * @brief 复制赋值运算符重载
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator=(complex &&) & = default;
        /**
         * @brief 复制赋值运算符重载
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        template <typename U>
        constexpr complex &
        operator=(complex<U> &&rhs) & noexcept(std::is_nothrow_assignable_v<T &, U &&>) {
            m_real = std::move(rhs).real();
            m_imag = std::move(rhs).imag();
            return *this;
        }

        /// @brief 销毁 complex 对象
        constexpr ~complex() = default;

        // 复合赋值与其他运算符重载
        /**
         * @brief 一元加号运算符重载
         * @return complex 此对象的引用
         */
        [[nodiscard]] constexpr complex operator+() const noexcept { return *this; }
        /**
         * @brief 一元减号运算符重载
         * @return complex 此对象取相反数的引用
         */
        [[nodiscard]] constexpr complex operator-() const noexcept(noexcept(-std::declval<T>())) {
            return {-m_real, -m_imag};
        }

        /**
         * @brief 加法赋值运算符重载
         * @param [in] rhs 实部
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator+=(const T &rhs) & noexcept(requires(T lhs, T rhs) {
            { lhs += rhs } noexcept;
        }) {
            m_real += rhs;
            return *this;
        }
        /**
         * @brief 减法赋值运算符重载
         * @param [in] rhs 实部
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator-=(const T &rhs) & noexcept(requires(T lhs, T rhs) {
            { lhs -= rhs } noexcept;
        }) {
            m_real -= rhs;
            return *this;
        }
        /**
         * @brief 乘法赋值运算符重载
         * @param [in] rhs 实部
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator*=(const T &rhs) & noexcept(requires(T lhs, T rhs) {
            { lhs *= rhs } noexcept;
        }) {
            m_real *= rhs;
            m_imag *= rhs;
            return *this;
        }
        /**
         * @brief 除法赋值运算符重载
         * @param [in] rhs 实部
         * @return complex& 修改后此对象的引用
         */
        constexpr complex &operator/=(const T &rhs) & noexcept(requires(T lhs, T rhs) {
            { lhs /= rhs } noexcept;
        }) {
            m_real /= rhs;
            m_imag /= rhs;
            return *this;
        }

        /**
         * @brief 加法赋值运算符重载
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        template <typename U>
        constexpr complex &operator+=(const complex<U> &rhs) & noexcept(requires(T lhs, T rhs) {
            { lhs += rhs } noexcept;
        }) {
            m_real += rhs.real();
            m_imag += rhs.imag();
            return *this;
        }
        /**
         * @brief 减法赋值运算符重载
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        template <typename U>
        constexpr complex &operator-=(const complex<U> &rhs) & noexcept(requires(T lhs, T rhs) {
            { lhs -= rhs } noexcept;
        }) {
            m_real -= rhs.real();
            m_imag -= rhs.imag();
            return *this;
        }
        /**
         * @brief 乘法赋值运算符重载
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        template <typename U>
        constexpr complex &operator*=(const complex<U> &rhs) & noexcept(
            std::is_nothrow_move_assignable_v<T> && noexcept(std::declval<T>() * std::declval<U>())
            && noexcept(std::declval<T>() + std::declval<T>())
            && noexcept(std::declval<T>() - std::declval<T>())) {
            real((real() * rhs.real()) - (imag() * rhs.imag()));
            imag((real() * rhs.imag()) + (imag() * rhs.real()));
            return *this;
        }
        /**
         * @brief 除法赋值运算符重载
         * @tparam U 另一个对象的模板参数
         * @param [in] rhs 另一个对象
         * @return complex& 修改后此对象的引用
         */
        template <typename U>
        constexpr complex &operator/=(const complex<U> &rhs) & noexcept(
            std::is_nothrow_move_assignable_v<T> && noexcept(std::declval<T>() * std::declval<U>())
            && noexcept(std::declval<T>() + std::declval<T>())
            && noexcept(std::declval<T>() - std::declval<T>())) {
            auto den = (rhs.real() * rhs.real()) + (rhs.imag() * rhs.imag());
            real(((real() * rhs.real()) + (imag() * rhs.imag())) / den);
            imag(((imag() * rhs.real()) - (real() * rhs.imag())) / den);
            return *this;
        }

        // 其他成员函数
        /**
         * @brief 获取实部
         * @return const T& 实部
         */
        [[nodiscard]] constexpr const T &real() const & noexcept { return m_real; }
        /**
         * @brief 获取实部
         * @return T 实部
         */
        [[nodiscard]] constexpr T        real()        &&noexcept { return m_real; }
        /**
         * @brief 设置实部
         * @param [in] value 实部
         */
        constexpr void                   real(T value) noexcept(std::is_nothrow_move_assignable_v<T>) {
            m_real = std::move(value);
        }

        /**
         * @brief 获取虚部
         * @return const T& 虚部
         */
        [[nodiscard]] constexpr const T &imag() const & noexcept { return m_imag; }
        /**
         * @brief 获取虚部
         * @return T 虚部
         */
        [[nodiscard]] constexpr T        imag()        &&noexcept { return m_imag; }
        /**
         * @brief 设置虚部
         * @param [in] value 虚部
         */
        constexpr void                   imag(T value) noexcept(std::is_nothrow_move_assignable_v<T>) {
            m_imag = std::move(value);
        }
    };

    /**
     * @brief 加法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数1
     * @param [in] rhs 复数2
     * @return complex<T> 相加后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator+(const complex<T> &lhs,
              const complex<T> &rhs) noexcept(requires(complex<T> lhs, complex<T> rhs) {
        { lhs += rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy += rhs;
    }
    /**
     * @brief 减法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数1
     * @param [in] rhs 复数2
     * @return complex<T> 相减后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator-(const complex<T> &lhs,
              const complex<T> &rhs) noexcept(requires(complex<T> lhs, complex<T> rhs) {
        { lhs -= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy -= rhs;
    }
    /**
     * @brief 乘法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数1
     * @param [in] rhs 复数2
     * @return complex<T> 相乘后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator*(const complex<T> &lhs,
              const complex<T> &rhs) noexcept(requires(complex<T> lhs, complex<T> rhs) {
        { lhs *= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy *= rhs;
    }
    /**
     * @brief 除法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数1
     * @param [in] rhs 复数2
     * @return complex<T> 相除后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator/(const complex<T> &lhs,
              const complex<T> &rhs) noexcept(requires(complex<T> lhs, complex<T> rhs) {
        { lhs /= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy /= rhs;
    }

    /**
     * @brief 加法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数的实部
     * @param [in] rhs 复数
     * @return complex<T> 相加后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator+(const T &lhs, const complex<T> &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs += rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy += rhs;
    }
    /**
     * @brief 减法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数的实部
     * @param [in] rhs 复数
     * @return complex<T> 相减后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator-(const T &lhs, const complex<T> &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs -= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy -= rhs;
    }
    /**
     * @brief 乘法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数的实部
     * @param [in] rhs 复数
     * @return complex<T> 相乘后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator*(const T &lhs, const complex<T> &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs *= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy *= rhs;
    }
    /**
     * @brief 除法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数的实部
     * @param [in] rhs 复数
     * @return complex<T> 相除后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T>
    operator/(const T &lhs, const complex<T> &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs /= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy /= rhs;
    }

    /**
     * @brief 加法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数
     * @param [in] rhs 复数的实部
     * @return complex<T> 相加后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T> operator+(const complex<T> &lhs,
                                                 const T &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs += rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy += rhs;
    }
    /**
     * @brief 减法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数
     * @param [in] rhs 复数的实部
     * @return complex<T> 相减后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T> operator-(const complex<T> &lhs,
                                                 const T &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs -= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy -= rhs;
    }
    /**
     * @brief 乘法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数
     * @param [in] rhs 复数的实部
     * @return complex<T> 相乘后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T> operator*(const complex<T> &lhs,
                                                 const T &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs *= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy *= rhs;
    }
    /**
     * @brief 除法运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数
     * @param [in] rhs 复数的实部
     * @return complex<T> 相除后的结果
     */
    template <typename T>
    [[nodiscard]] constexpr complex<T> operator/(const complex<T> &lhs,
                                                 const T &rhs) noexcept(requires(complex<T> lhs, T rhs) {
        { lhs /= rhs } noexcept;
    }) {
        complex<T> cpy = lhs;
        return cpy /= rhs;
    }

    /**
     * @brief 等于运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数1
     * @param [in] rhs 复数2
     * @return true 相等
     * @return false 不相等
     */
    template <typename T>
    constexpr bool operator==(const complex<T> &lhs, const complex<T> &rhs) noexcept {
        if (&lhs == &rhs) {
            return true;
        }
        return lhs.real() == rhs.real() && lhs.imag() == rhs.imag();
    }
    /**
     * @brief 等于运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数的实部
     * @param [in] rhs 复数
     * @return true 相等
     * @return false 不相等
     */
    template <typename T>
    constexpr bool operator==(const T &lhs, const complex<T> &rhs) noexcept {
        return lhs == rhs.real() && T{} == rhs.imag();
    }
    /**
     * @brief 等于运算符重载
     * @tparam T 复数的模板参数
     * @param [in] lhs 复数
     * @param [in] rhs 复数的实部
     * @return true 相等
     * @return false 不相等
     */
    template <typename T>
    constexpr bool operator==(const complex<T> &lhs, const T &rhs) noexcept {
        return lhs.real() == rhs && lhs.imag() == T{};
    }

} // namespace tnrw::math

#endif // TNRW_MATH_COMPLEX_HPP
