#ifndef GAME_H__
#define GAME_H__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <ranges>
#include <system_error>

#include "engine/io/mouse.h"
#include "engine/io/window.h"
#include "engine/resource-manager.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/basic-shapes/rectangle.hpp"
#include "ai/search.h"
#include "logic/piece.h"
#include "logic/board.h"
#include "ui/ui.h"
#include "utilities/rectangle.h"
#include "utilities/zobrist-hash.h"

auto loadResources()  //
    -> void;

enum class GameState {
    Menu,
    Game,
};

class ChessWidget {
public:
    ChessWidget() = delete;
    ChessWidget(Board&);
    auto draw()
        -> void;
    auto on_click_action()
        -> void;
    auto on_hover_action() 
        -> void {}
    auto get_square(glm::vec2) const
        -> Square;
    auto get_sprite()
        -> Sprite&;
    static auto load_resources()
        -> void;
private:
    Sprite m_sprite;
    ItemFrame rect;
    std::array<std::array<Sprite, 6>, 2> piece_s;
    IntermediateBoardRepresentation i_board;
    std::optional<Square> m_selected{};
    Board& m_board;
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
    rectangle white;
    rectangle black;
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
    auto proccessClick()  //
        -> void;
private:
    float evaluation;
private:
    Board m_board;
    Window& m_window;
    ChessWidget m_gui;
    EvalBar eval_bar;
};



#endif