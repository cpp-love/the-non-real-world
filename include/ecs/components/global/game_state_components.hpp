/**
 * @file game_state_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏状态的组件
 * @version 0.1.0-2
 * @date 2026-02-20
 * 
 * @copyright cpp-love
 * 
 * @details 状态采用多态的方式实现
 * 
 */

#ifndef TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP

#include "base/assert_msg.hpp"
#include "ecs/components/global/game_base.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/signal/fwd.hpp>
#include <memory>
#include <string_view>
#include <utility>

namespace tnrw::ecs {

    /// @brief 游戏的状态枚举
    enum class game_state_enum : std::uint8_t {
        settings_menu = 0, ///< 设置
        main_menu,         ///< 主页
        game_screen,       ///< 游戏界面
        pause_menu,        ///< 暂停界面
        state_count        ///< 状态总数
    };

    /**
     * @brief 将游戏状态转换为字符串视图
     * @param [in] state 状态
     * @return std::string_view 转换后的字符串视图
     */
    constexpr std::string_view to_string_view(game_state_enum state) {
        static_assert(std::to_underlying(game_state_enum::state_count) == 4,
                      "此函数未完备所有 `game_state_enum` 的枚举的处理");
        switch (state) {
            case game_state_enum::settings_menu: return "settings_menu";
            case game_state_enum::game_screen: return "game_screen";
            case game_state_enum::main_menu: return "main_menu";
            case game_state_enum::pause_menu: return "pause_menu";
            default:
                assert_msg(false, "参数 `state` （整数形式为：{}）有非法的状态",
                           std::to_underlying(state));
                return {};
        }
    }

    /// @brief 压入状态的事件组件
    struct game_state_push_event final {
        game_state_enum state;
    };

    /// @brief 弹出状态的事件组件
    struct game_state_pop_event final {};

    /// @brief 游戏状态基类
    class game_state_base {
      public:
        /**
         * @brief 从分配器构造的构造函数
         * @param [in] dispatcher 分配器
         */
        explicit game_state_base(entt::dispatcher &dispatcher) : m_dispather(dispatcher) {}
        /// @brief 删除复制构造函数
        game_state_base(const game_state_base &rhs) noexcept = delete;
        /**
         * @brief 移动构造函数
         * @param [in] rhs 另一个游戏状态
         */
        game_state_base(game_state_base &&rhs) noexcept = default;
        /// @brief 删除复制赋值运算符重载
        game_state_base &operator=(const game_state_base &rhs) noexcept = delete;
        /// @brief 删除移动赋值运算符重载
        game_state_base &operator=(game_state_base &&rhs) noexcept = delete;
        /// @brief 虚析构函数
        virtual ~game_state_base() noexcept = default;
        /// @brief 将此游戏状态暂停
        virtual void on_pause() noexcept = 0;
        /// @brief 将此游戏状态从暂停中恢复
        virtual void on_resume() noexcept = 0;
        /**
         * @brief 处理事件
         * @param [in] event 事件
         * @return true 事件已处理
         * @return false 事件未处理
         */
        virtual bool handle_event(const sf::Event &event) noexcept = 0;
        /**
         * @brief 更新游戏状态
         * @param [in] delta_time 时间间隔
         */
        virtual void update(milliseconds_f delta_time) noexcept = 0;
        /**
         * @brief 绘制当前状态
         * @param [in] render 需要渲染的地方
         */
        virtual void draw(sf::RenderTarget &render) noexcept = 0;
        /**
         * @brief 判断是否要阻隔绘制、更新、事件往下传递
         * @return true 要阻隔
         * @return false 不要阻隔
         * @details 默认为 true
         */
        virtual bool should_block_passing_down() noexcept { return true; }

        /// @cond INTERNAL
      protected:
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes, cppcoreguidelines-avoid-const-or-ref-data-members)
        entt::dispatcher &m_dispather; ///< 分配器
        /// @endcond
    };

    /// @brief 设置状态类
    class settings_menu : public game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base(entt::dispatcher &dispatcher)
        explicit settings_menu(entt::dispatcher &dispatcher) noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        settings_menu(const settings_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        settings_menu(settings_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        settings_menu &operator=(const settings_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        settings_menu &operator=(settings_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~settings_menu() noexcept override = default;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void draw(sf::RenderTarget &render) noexcept override;

      private:
        /// @brief 连接分配器
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接分配器
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停
        entt::registry m_registry;          ///< 注册表
    };

    /// @brief 主页状态类
    class main_menu : public game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base(entt::dispatcher &dispatcher)
        explicit main_menu(entt::dispatcher &dispatcher) noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        main_menu(const main_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        main_menu(main_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        main_menu &operator=(const main_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        main_menu &operator=(main_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~main_menu() noexcept override = default;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void draw(sf::RenderTarget &render) noexcept override;

      private:
        /// @brief 连接分配器
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接分配器
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停
        entt::registry m_registry;          ///< 注册表
    };

    /// @brief 游戏界面状态类
    class game_screen : public game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base(entt::dispatcher &dispatcher)
        explicit game_screen(entt::dispatcher &dispatcher) noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        game_screen(const game_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        game_screen(game_screen &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        game_screen &operator=(const game_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        game_screen &operator=(game_screen &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~game_screen() noexcept override = default;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void draw(sf::RenderTarget &render) noexcept override;

      private:
        /// @brief 连接分配器
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接分配器
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停
        entt::registry m_registry;          ///< 注册表
    };

    /// @brief 暂停界面状态类
    class pause_menu : public game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base(entt::dispatcher &dispatcher)
        explicit pause_menu(entt::dispatcher &dispatcher) noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        pause_menu(const pause_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        pause_menu(pause_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        pause_menu &operator=(const pause_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        pause_menu &operator=(pause_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~pause_menu() noexcept override = default;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void draw(sf::RenderTarget &render) noexcept override;
        /// @copybrief game_state_base::should_block_passing_down
        bool should_block_passing_down() noexcept override { return false; }

      private:
        /// @brief 连接分配器
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接分配器
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停
        entt::registry m_registry;          ///< 注册表
    };

    /**
     * @brief 通过状态枚举构造对应的状态类，并返回基类指针
     * @param [in] state 状态枚举
     * @param [in] dispatcher 用于构造的分配器引用
     * @return std::unique_ptr<game_state_base> 基类指针
     */
    inline std::unique_ptr<game_state_base> make_corresponding_state(game_state_enum   state,
                                                                     entt::dispatcher &dispatcher) {
        static_assert(std::to_underlying(game_state_enum::state_count) == 4,
                      "此函数未完备所有 `game_state_enum` 的枚举的处理");
        switch (state) {
            case game_state_enum::settings_menu:
                return std::make_unique<settings_menu>(settings_menu(dispatcher));
                break;
            case game_state_enum::main_menu:
                return std::make_unique<main_menu>(main_menu(dispatcher));
                break;
            case game_state_enum::game_screen:
                return std::make_unique<game_screen>(game_screen(dispatcher));
                break;
            case game_state_enum::pause_menu:
                return std::make_unique<pause_menu>(pause_menu(dispatcher));
                break;
            default:
                assert_msg(false, "参数 `state` （整数形式为：{}）有非法的状态",
                           std::to_underlying(state));
                return {};
        }
    }

} // namespace tnrw::ecs

#endif // TNRW_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP