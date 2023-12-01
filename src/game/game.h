#ifndef GAME_H__
#define GAME_H__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <ranges>

#include "engine/io/mouse.h"
#include "engine/io/window.h"
#include "engine/resource-manager.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/basic-shapes/rectangle.hpp"
#include "logic/board.h"
#include "ui/ui.h"
#include "utilities/rectangle.h"

auto loadResources()  //
    -> void;

enum class GameState {
    Menu,
    Game,
};

class ChessWidget {
public:
    ChessWidget() = delete;
    ChessWidget(chess::board&);
    auto draw()
        -> void;
    auto on_click_action()
        -> std::optional<chess::move>;
    auto on_hover_action() 
        -> void {}
    auto get_square(glm::vec2) const
        -> chess::square;
    auto get_sprite()
        -> engine::sprite&;
    static auto load_resources()
        -> void;
private:
    engine::sprite m_sprite;
    ItemFrame rect;
    std::array<std::array<engine::sprite, 6>, 2> piece_s;
    IntermediateBoardRepresentation i_board;
    std::optional<chess::square> m_selected{};
    chess::board& m_board;
};

class EvalBar {
public:
    EvalBar(float& eval);
    auto draw()
        -> void;
    auto update()
        -> void;
    auto on_click_action()
        -> void;
private:
    float prev_eval{0.0f};
    float& curr_eval;
    engine::rectangle white;
    engine::rectangle black;
};

class Game {
public:
    Game(Window&);

    auto draw()  //
        -> void;
    auto update()  //
        -> void;
    auto proccessInput()  //
        -> void;
private:
    auto undo_move()
        -> void;
    auto proccessClick()  //
        -> void;
private:
    float evaluation;
private:
    chess::board m_board;
    std::vector<chess::move> m_move_history;
    Window& m_window;
    ChessWidget m_gui;
    EvalBar eval_bar;
};



#endif