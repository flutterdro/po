#ifndef PIECE_H__
#define PIECE_H__

#include <array>
#include <vector>
#include <tuple>
#include <fmt/core.h>

#include "../engine/texture.h"
#include "../engine/shader.h"
#include "../engine/sprite.h"
#include "../engine/resource-manager.h"

class Piece {
public:
    using Board = std::array<std::array<Piece*, 8>, 8>;
    Piece(bool color, int row, int col);
    void move(int row, int col, Board& board);
    virtual void updateLegalMoves(const Board&) {}
    virtual void draw(float, float) {}

    bool getColor() const noexcept;
    bool isLegalMove(std::pair<int, int> square);
    auto getLegalMoves() const noexcept 
        -> const std::vector<std::pair<int, int>>&;
    auto getPos() const noexcept 
        -> const std::pair<int, int>;
    
    virtual ~Piece() {}
protected:
    int m_row;
    int m_col;
    bool color;
    std::vector<std::pair<int, int>> m_legal_moves;
};

using Board = std::array<std::array<Piece*, 8>, 8>;
void update_b(Board&);
class Knight : public Piece {
public:
    Knight(bool color, int row, int col) 
        : Piece(color, row, col), m_sprite(*RM::getInstance().getShader("piece"), 
                                           *RM::getInstance().getTexture("black-pawn")){}
    void updateLegalMoves(const Board& board) override;
    void draw(float pos_x, float pos_y) override;
private: 
    Sprite m_sprite;
};

#endif