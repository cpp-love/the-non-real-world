/**
 * @file overload.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了零开销的对于可调用类型的重载的函数对象的工厂函数
 * @version 0.1.0-1
 * @date 2026-02-16
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_OVERLOAD_HPP
#define TNRW_BASE_OVERLOAD_HPP

#include <functional>
#include <type_traits>
#include <utility>

namespace tnrw {

    /// @cond INTERNAL
    namespace details {

        /**
         * @brief 对于多个函数对象的重载函数对象类模板
         * @tparam Funcs 多个函数对象的类型
         */
        template <typename... Funcs>
        struct overloaded : Funcs... {
            using Funcs::operator()...;
        };

        /**
         * @brief Overloaded的推导指引
         * @tparam Funcs 多个函数对象的类型
         */
        template <typename... Funcs>
        overloaded(Funcs &&...) -> overloaded<Funcs...>;

        /**
         * @brief 提取原始的函数类型(即 `Ret(Args...)`)的 trait
         * @tparam Func 函数类型，需要满足 `std::is_function_v<Func>`
         */
        template <typename Func>
            requires std::is_function_v<Func>
        struct raw_function {
            using type = Func; ///< 提取的原始函数类型，去除 const volatile &/&& noexcept 限定符
        };

        /// @brief 对于原始函数类型的特化
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...)> {
            using type = Ret(Args...);
        };

        /// @brief 对于 C-style 可变参数的原始类型的特化
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...)> {
            using type = Ret(Args..., ...);
        };

        // cv-限定符的特化
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) volatile> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const volatile> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) volatile> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const volatile> {
            using type = Ret(Args..., ...);
        };

        // 引用限定符的特化
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) &> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const &> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) volatile &> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const volatile &> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) &> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const &> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) volatile &> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const volatile &> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) &&> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const &&> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) volatile &&> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const volatile &&> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) &&> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const &&> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) volatile &&> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const volatile &&> {
            using type = Ret(Args..., ...);
        };

        // noexcept版本的特化
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) volatile noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const volatile noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) volatile noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const volatile noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) & noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const & noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) volatile & noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const volatile & noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) & noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const & noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) volatile & noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const volatile & noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) && noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const && noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) volatile && noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args...) const volatile && noexcept> {
            using type = Ret(Args...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) && noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const && noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) volatile && noexcept> {
            using type = Ret(Args..., ...);
        };
        template <typename Ret, typename... Args>
        struct raw_function<Ret(Args..., ...) const volatile && noexcept> {
            using type = Ret(Args..., ...);
        };

        /**
         * @brief 提取原始的函数类型(即 `Ret(Args...)`)的类型别名
         * @tparam Func 函数类型，需要满足 `std::is_function_v<Func>`
         */
        template <typename Func>
        using raw_function_t = typename raw_function<Func>::type;

        /**
         * @brief 提取原始的成员函数指针(即 `Ret(Class::*)(Args...)`)的 trait 的帮助 trait
         * @tparam Func 函数类型，需要满足 `std::is_member_function_pointer_v<Func>`
         */
        template <typename Func>
            requires std::is_member_function_pointer_v<Func>
        struct raw_member_function_pointer_helper {
            using type = Func; ///< 提取的原始成员函数指针，去除 const volatile &/&& noexcept 限定符
        };

        template <typename Member, typename Class>
        struct raw_member_function_pointer_helper<Member Class::*> {
            using type = raw_function_t<Member> Class::*;
        };

        /**
         * @brief 提取原始的成员函数指针(即 `Ret(Class::*)(Args...)`)的 trait
         * @tparam Func 函数类型，需要满足 `std::is_member_function_pointer_v<Func>`
         */
        template <typename Func>
            requires std::is_member_function_pointer_v<Func>
        struct raw_member_function_pointer : raw_member_function_pointer_helper<std::remove_cv_t<Func>> {
        };

        /**
         * @brief 提取原始的成员函数指针(即 `Ret(Class::*)(Args...)`)的类型别名
         * @tparam Func 函数类型，需要满足 `std::is_member_function_pointer_v<Func>`
         */
        template <typename Func>
        using raw_member_function_pointer_t = typename raw_member_function_pointer<Func>::type;

        /**
         * @brief 将可调用对象转为函数对象的 trait 的帮助 trait
         * @tparam Func 可调用对象
         * @tparam RawFunc 可调用对象的原始类型
         * @see tnrw::AsCallable
         */
        template <typename Func, typename RawFunc>
        struct as_callable_helper;

        /**
         * @brief 将可调用对象转为函数对象的 trait 的帮助 trait 对 原始函数类型的特化
         * @tparam Func 函数类型
         * @tparam Ret 原始函数类型的返回值类型
         * @tparam Args 原始函数类型的参数类型
         */
        template <typename Func, typename Ret, typename... Args>
        struct as_callable_helper<Func, Ret(Args...)> {
            /// @brief 转换后的函数对象类型
            class type {
                /// @cond INTERNAL
              private: /// @privatesection
                // 数据成员
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
                Func &m_func; ///< 函数引用

                /// @endcond
              public: /// @publicsection
                /**
                 * @brief 从函数引用构造的构造函数
                 * @param [in] func 函数引用
                 */
                type(Func &func) : m_func(func) {} // NOLINT(hicpp-explicit-conversions)

                /**
                 * @brief 调用运算符重载
                 * @param [in] call_args 调用参数
                 * @return Ret 返回值
                 */
                Ret operator()(Args... call_args) const
                    noexcept(noexcept(std::invoke(m_func, std::move(call_args)...))) {
                    return std::invoke(m_func, std::move(call_args)...);
                }
            };
        };

        /**
         * @brief 将可调用对象转为函数对象的 trait 的帮助 trait 对 原始函数指针的特化
         * @tparam Func 函数指针类型
         * @tparam Ret 原始函数指针的返回值类型
         * @tparam Args 原始函数指针的参数类型
         */
        template <typename Func, typename Ret, typename... Args>
        struct as_callable_helper<Func, Ret (*)(Args...)> {
            /// @brief 转换后的函数对象类型
            class type {
                /// @cond INTERNAL
              private: /// @privatesection
                // 数据成员
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
                Func m_func; ///< 函数指针

                /// @endcond
              public: /// @publicsection
                /**
                 * @brief 从函数指针构造的构造函数
                 * @param [in] func 函数指针
                 */
                type(Func func) : m_func(func) {} // NOLINT(hicpp-explicit-conversions)

                /**
                 * @brief 调用运算符重载
                 * @param [in] call_args 调用参数
                 * @return Ret 返回值
                 */
                Ret operator()(Args... call_args) const
                    noexcept(noexcept(std::invoke(m_func, std::move(call_args)...))) {
                    return std::invoke(m_func, std::move(call_args)...);
                }
            };
        };

        /**
         * @brief 将可调用对象转为函数对象的 trait 的帮助 trait 对 原始成员函数指针的特化
         * @tparam Func 成员函数指针类型
         * @tparam Ret 原始成员函数指针的返回值类型
         * @tparam Class 原始成员函数指针所属的对象类型
         * @tparam Args 原始成员函数指针的参数类型
         */
        template <typename Func, typename Ret, typename Class, typename... Args>
        struct as_callable_helper<Func, Ret (Class::*)(Args...)> {
            /// @brief 转换后的函数对象类型
            class type {
                /// @cond INTERNAL
              private: /// @privatesection
                // 数据成员
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
                Func m_func; ///< 成员函数指针

                /// @endcond
              public: /// @publicsection
                /**
                 * @brief 从成员函数指针构造的构造函数
                 * @param [in] func 成员函数指针
                 */
                type(Func func) : m_func(func) {} // NOLINT(hicpp-explicit-conversions)

                /**
                 * @brief 调用运算符重载
                 * @tparam ObjectT 可以进行函数调用的类型，如 `Class`, `Class *` 等
                 * @param [in] object ObjectT 类型的对象
                 * @param [in] call_args 调用参数
                 * @return Ret 返回值
                 */
                template <typename ObjectT>
                Ret operator()(ObjectT &&object, Args... call_args) const
                    noexcept(noexcept(std::invoke(m_func, std::forward<ObjectT>(object),
                                                  std::move(call_args)...))) {
                    return std::invoke(m_func, std::forward<ObjectT>(object), std::move(call_args)...);
                }
            };
        };

    } // namespace details
    /// @endcond

    /**
     * @brief 将可调用对象转为函数对象的 trait
     * @tparam Func 可调用对象
     */
    template <typename Func>
    struct as_callable {
        using type = Func; ///< 转换后的函数对象类型
    };

    /**
     * @brief 将可调用对象转为函数对象的 trait 对于函数指针的特化
     * @tparam FuncPtr 函数指针类型
     */
    template <typename FuncPtr>
        requires std::is_function_v<std::remove_pointer_t<FuncPtr>> && std::is_pointer_v<FuncPtr>
    struct as_callable<FuncPtr>
        : details::as_callable_helper<
              FuncPtr, std::add_pointer_t<details::raw_function_t<std::remove_pointer_t<FuncPtr>>>> {};

    /**
     * @brief 将可调用对象转为函数对象的 trait 对于函数的特化
     * @tparam Func 函数类型
     */
    template <typename Func>
        requires std::is_function_v<Func>
    struct as_callable<Func> : details::as_callable_helper<Func, details::raw_function_t<Func>> {};

    /**
     * @brief 将可调用对象转为函数对象的 trait 对于成员函数指针的特化
     * @tparam MemFuncPtr 成员函数指针类型
     */
    template <typename MemFuncPtr>
        requires std::is_member_function_pointer_v<MemFuncPtr>
    struct as_callable<MemFuncPtr>
        : details::as_callable_helper<MemFuncPtr, details::raw_member_function_pointer_t<MemFuncPtr>> {};

    /**
     * @brief 将可调用对象转为函数对象的类型别名
     * @tparam Func 可调用对象类型
     */
    template <typename Func>
    using as_callable_t = typename as_callable<Func>::type;

    /**
     * @brief 生成对于可调用类型的重载的函数对象的工厂函数
     * @tparam Funcs 可调用类型
     * @param [in] funcs 可调用对象
     * @return auto 含有可调用对象重载的函数对象
     * @details
     *  - 支持绝大部分可调用类型：
     *    1. 非模板的普通函数（函数引用）
     *    2. 非模板的函数指针
     *    3. 非模板的成员函数指针（但包装成为模板函数对象，调用优先级比非模板的可调用对象低）
     *    4. 函数对象（包括 std::function 等标准库定义的函数对象）
     *    5. lambda 表达式
     * @warning
     *  - 不支持普通模板函数，
     *    若要使其参与重载，请使用 `泛型 lambda 表达式包装` 或 `手动定义函数对象包装`
     *  - 不支持 C-style 可变参数函数
     *    若要使其参与重载，请使用 `泛型 lambda 表达式包装` 或 `手动定义函数对象包装`
     */
    template <typename... Funcs>
    constexpr auto make_overloaded(Funcs &&...funcs) {
        return details::overloaded{
            as_callable_t<std::remove_reference_t<Funcs>>(std::forward<Funcs>(funcs))...};
    }

} // namespace tnrw

#endif // TNRW_BASE_OVERLOAD_HPP