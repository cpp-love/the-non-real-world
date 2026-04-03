/**
 * @file observable.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了一个相对方便的被观察者模板
 * @version 0.1.0-1
 * @date 2026-04-02
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_OBSERVABLE
#define TNRW_BASE_OBSERVABLE

#include <functional>
#include <type_traits>
#include <vector>

namespace tnrw {

    /**
     * @brief 被观察者模板类
     * @tparam T 被观察者的类型
     */
    template <typename T>
    class observable {
      public:
        using observer_callback =
            std::function<void(const T &)>; ///< 观察者接口，接收参数为被观察者修改后的值，没有返回值
      private:
        T                              m_value;     ///< 被观察者
        std::vector<observer_callback> m_observers; ///< 观察者列表
      public:
        /**
         * @brief 构建一个 observable 对象
         * @param [in] value 被观察者
         * @param [in] observers 观察者列表
         */
        observable(const T &value, const std::vector<observer_callback> &observers)
            : m_value(value), m_observers(observers) {}
        /**
         * @brief 构建一个 observable 对象
         * @param [in] value 被观察者
         * @param [in] observers 观察者列表
         */
        observable(T value, const std::vector<observer_callback> &observers)
            : m_value(std::move(value)), m_observers(observers) {}
        /**
         * @brief 构建一个 observable 对象
         * @param [in] value 被观察者
         * @param [in] observers 观察者列表
         */
        explicit observable(T value = {}, std::vector<observer_callback> observers = {}) noexcept(
            std::is_nothrow_move_constructible_v<T>)
            : m_value(std::move(value)), m_observers(std::move(observers)) {}
        /**
         * @brief 构建一个 observable 对象
         * @param [in] value 被观察者
         * @param [in] observers 观察者列表
         */
        observable(const T &value, std::vector<observer_callback> observers) noexcept(
            std::is_nothrow_copy_constructible_v<T>)
            : m_value(value), m_observers(std::move(observers)) {}
        /**
         * @brief 构建一个 observable 对象
         * @param [in] rhs 另一个对象
         */
        observable(const observable &rhs) = default;
        /**
         * @brief 构建一个 observable 对象
         * @param [in] rhs 另一个对象
         */
        observable(observable &&rhs) = default;
        /// @brief 禁用复制赋值运算符
        observable &operator=(const observable &rhs) = delete;
        /// @brief 禁用移动赋值运算符
        observable &operator=(observable &&rhs) = delete;
        /// @brief 销毁 observable 对象
        ~observable() = default;

        /**
         * @brief 获取观察者列表的引用
         * @return std::vector<observer_handler>& 观察者列表的引用
         */
        [[nodiscard]] std::vector<observer_callback>       &observers() noexcept { return m_observers; }
        /**
         * @brief 获取观察者列表的引用
         * @return const std::vector<observer_handler>& 观察者列表的引用
         */
        [[nodiscard]] const std::vector<observer_callback> &observers() const noexcept {
            return m_observers;
        }

        /**
         * @brief 获取被观察者值
         * @return const T& 被观察者值
         */
        [[nodiscard]] const T &get_value() const noexcept { return m_value; }
        /**
         * @brief 设置被观察者值
         * @param [in] value 被观察者值
         */
        void                   set_value(const T &value) const noexcept {
            m_value = value;
            notify_callback();
        }
        /**
         * @brief 设置被观察者值
         * @param [in] value 被观察者值
         */
        void set_value(T &&value) const noexcept {
            m_value = std::move(value);
            notify_callback();
        }

        /**
         * @brief 为被观察者值调用函数
         * @tparam SelfT 显示对象参数的类型
         * @tparam FuncT 调用的函数类型
         * @tparam Args 调用的其他参数类型
         * @param [in] self 显示对象参数
         * @param [in] function 调用的函数
         * @param [in] args 调用的其他参数
         * @return decltype(auto) 调用的返回值
         * @details 被观察者值是调用的第一个参数，所以调用就像这样
         * `std::forward<FuncT>(function)(std::forward<SelfT>(self).m_value, std::forward<Args>(args)...)`
         */
        template <typename SelfT, typename FuncT, typename... Args>
        decltype(auto) invoke(this SelfT &&self, FuncT &&function, Args &&...args)
            requires std::is_invocable_v<FuncT, decltype(std::forward_like<SelfT>(self.m_value)),
                                         Args...>
        {
            using value_t = decltype(std::forward_like<SelfT>(self.m_value));
            if constexpr (!std::is_lvalue_reference_v<value_t>
                          || std::is_invocable_v<FuncT, const T &, Args...>) {
                return std::forward<FuncT>(function)(std::forward<SelfT>(self).m_value,
                                                     std::forward<Args>(args)...);
            } else {
                std::invoke_result_t<FuncT, value_t, Args...> result = std::forward<FuncT>(function)(
                    std::forward<SelfT>(self).m_value, std::forward<Args>(args)...);
                std::forward<SelfT>(self).notify_callback();
                return result;
            }
        }

      private:
        /**
         * @brief 通知回调函数
         * @tparam SelfT 显示对象参数的类型
         * @param [in] self 显示对象参数
         */
        template <typename SelfT>
        void notify_callback(this SelfT &&self) {
            for (const observer_callback &callback : std::forward<SelfT>(self).observers()) {
                callback(static_cast<const observable &>(self).m_value);
            }
        }
    };

} // namespace tnrw

#endif // TNRW_BASE_OBSERVABLE