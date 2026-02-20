/**
 * @file game_state_components.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏状态的组件 
 * @version 0.1.0-1
 * @date 2026-02-19
 * 
 * @copyright cpp-love
 * 
 */

#include "ecs/components/global/game_state_components.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace tnrw::ecs {

    // settings_menu
    settings_menu::settings_menu(entt::dispatcher &dispatcher) noexcept : game_state_base(dispatcher) {
        connect_dispatcher();
    }
    void settings_menu::on_pause() noexcept {
        spdlog::trace("settings_menu paused");
        m_is_paused = true;
    }
    void settings_menu::on_resume() noexcept {
        spdlog::trace("settings_menu resumed");
        m_is_paused = false;
    }
    bool settings_menu::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("settings_menu handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_dispather.enqueue<game_state_pop_event>();
                return true;
            }
        }
        return false;
    }
    void settings_menu::update(milliseconds_f delta_time) noexcept {
        spdlog::trace("settings_menu updated, time: {}", delta_time);
    }
    void settings_menu::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("settings_menu drew");
        sf::RectangleShape rect{{200, 30}};
        render.draw(rect);
    }
    void settings_menu::connect_dispatcher() noexcept {}
    void settings_menu::disconnect_dispatcher() noexcept {}

    // main_menu
    main_menu::main_menu(entt::dispatcher &dispatcher) noexcept : game_state_base(dispatcher) {
        connect_dispatcher();
    }
    void main_menu::on_pause() noexcept {
        spdlog::trace("main_menu paused");
        m_is_paused = true;
    }
    void main_menu::on_resume() noexcept {
        spdlog::trace("main_menu resumed");
        m_is_paused = false;
    }
    bool main_menu::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("main_menu handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::S) {
                m_dispather.enqueue<game_state_push_event>(game_state_enum::settings_menu);
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::G) {
                m_dispather.enqueue<game_state_push_event>(game_state_enum::game_screen);
                return true;
            }
        }
        return false;
    }
    void main_menu::update(milliseconds_f delta_time) noexcept {
        spdlog::trace("main_menu updated, time: {}", delta_time);
    }
    void main_menu::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("main_menu drew");
        sf::CircleShape circle{200};
        render.draw(circle);
    }
    void main_menu::connect_dispatcher() noexcept {}
    void main_menu::disconnect_dispatcher() noexcept {}

    // game_screen
    game_screen::game_screen(entt::dispatcher &dispatcher) noexcept : game_state_base(dispatcher) {
        connect_dispatcher();
    }
    void game_screen::on_pause() noexcept {
        spdlog::trace("game_screen paused");
        m_is_paused = true;
    }
    void game_screen::on_resume() noexcept {
        spdlog::trace("game_screen resumed");
        m_is_paused = false;
    }
    bool game_screen::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("game_screen handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::S) {
                m_dispather.enqueue<game_state_push_event>(game_state_enum::settings_menu);
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_dispather.enqueue<game_state_pop_event>();
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::P) {
                m_dispather.enqueue<game_state_push_event>(game_state_enum::pause_menu);
                return true;
            }
        }
        return false;
    }
    void game_screen::update(milliseconds_f delta_time) noexcept {
        spdlog::trace("game_screen updated, time: {}", delta_time);
    }
    void game_screen::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("game_screen drew");
        sf::CircleShape circle{200};
        render.draw(circle);
        sf::RectangleShape rect{{20, 200}};
        render.draw(rect);
    }
    void game_screen::connect_dispatcher() noexcept {}
    void game_screen::disconnect_dispatcher() noexcept {}

    // pause_menu
    pause_menu::pause_menu(entt::dispatcher &dispatcher) noexcept : game_state_base(dispatcher) {
        connect_dispatcher();
    }
    void pause_menu::on_pause() noexcept {
        spdlog::trace("pause_menu paused");
        m_is_paused = true;
    }
    void pause_menu::on_resume() noexcept {
        spdlog::trace("pause_menu resumed");
        m_is_paused = false;
    }
    bool pause_menu::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("pause_menu handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_dispather.enqueue<game_state_pop_event>();
                return true;
            }
        }
        return false;
    }
    void pause_menu::update(milliseconds_f delta_time) noexcept {
        spdlog::trace("pause_menu updated, time: {}", delta_time);
    }
    void pause_menu::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("pause_menu drew");
        sf::CircleShape circle{200};
        render.draw(circle);
        sf::RectangleShape rect{{20, 200}};
        rect.setPosition({114, 514});
        render.draw(rect);
    }
    void pause_menu::connect_dispatcher() noexcept {}
    void pause_menu::disconnect_dispatcher() noexcept {}

} // namespace tnrw::ecs