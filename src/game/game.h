#ifndef GAME_H__
#define GAME_H__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <ranges>
#include <system_error>

#include "engine/io/mouse.h"
#include "engine/resource-manager.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "logic/piece.h"
#include "logic/board.h"

auto loadResources()  //
    -> void;

class Game {
public:
    Game() = default;
    auto init()  //
        -> void;

    auto draw()  //
        -> void;
    auto update()  //
        -> void;
    auto proccessInput()  //
        -> void;
    auto isCheckmate()  //
        -> bool;
    auto validateMove(Board const& board)  //
        -> bool;
    ~Game();
private:
    auto proccessClick()  //
        -> void;
    auto getCoverage(Board const& board) const noexcept  //
        -> std::pair<uint64_t, uint64_t>;
private:
    bool is_piece_selected = false;
    Square selected_square{0, 0};
    Square white_king_pos;
    Square black_king_pos;
    bool is_white_turn = true;
    bool is_in_check   = false;
private:
    Board m_board;
    Sprite m_board_sprite;
    Texture m_board_texture;
    std::vector<Piece*> m_piece_ptr_owner;
};

#endif