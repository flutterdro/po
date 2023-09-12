#ifndef GAME_H__
#define GAME_H__

#include <system_error>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "logic/piece.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/resource-manager.h"
#include "engine/io/mouse.h"

struct Rectangle {
    float x;
    float y;
    float width;
    float height;
    bool contains();
};

void loadResources();

class Game {
public:
    Game() = default;
    void init();
    void draw();
    void update();
    ~Game();
private: 
    bool thinking = false;
    int thinking_row;
    int thinking_col;
private:
    Board m_board;
    Sprite m_board_sprite;
    Texture m_board_texture;
    std::vector<Piece*> m_piece_ptr_owner;
};

#endif