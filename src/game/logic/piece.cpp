#include "piece.h"

Piece::Piece(bool color, int row, int col) 
        : m_row(row), m_col(col), color(color){}

bool Piece::getColor() const noexcept {
    return color;
}
auto Piece::getLegalMoves() const noexcept 
    -> const std::vector<std::pair<int, int>>& {
    return m_legal_moves;
}
auto Piece::getPos() const noexcept 
    -> const std::pair<int, int> {
    return {m_row, m_col};
}

bool Piece::isLegalMove(std::pair<int, int> square) {
    for (auto move : m_legal_moves) {
        if (move == square) return true;
    }
    return false;
}

void update_b(Board& board) {
    for (auto& row : board) {
        for (auto& piece : row) {
            if (piece != nullptr) {
                piece->updateLegalMoves(board);
            }
        }
    }
}

void Piece::move(int row, int col, Board& board) {
    std::pair old_pos = {row, col};
    bool is_legal = false;
    for (const auto& pos : m_legal_moves) {
        if (old_pos == pos) {
            board[m_row][m_col] = nullptr;
            board[row][col] = this;
            m_col = col; m_row = row;
            update_b(board);
            is_legal = true;
        }
    }
    if (!is_legal) {
        fmt::print("Move {}x{} is illegal\n", row, col);
    }
}

void Knight::draw(float pos_x, float pos_y) {
    m_sprite.draw({pos_x, pos_y});
}

void Knight::updateLegalMoves(const Board& board) {
    int row, col;
    m_legal_moves.resize(0);
    auto checkMove = [&](int row, int col) {
        if (col < 8 && col >= 0 && 
            row < 8 && row  >= 0) {
            const Piece const* ocupied = board[row][col];
            if (ocupied) {
                if (ocupied->getColor() != color) {
                    m_legal_moves.push_back({row, col});
                } else {
                    //fmt::print("Move K{}x{} is illegal\n", row, col);
                }
            } else {
                m_legal_moves.push_back({row, col});
                //fmt::print("Move K{}x{} is legal\n", row, col);
            }
        }
    };
    checkMove(m_row + 1, m_col + 2);
    checkMove(m_row + 2, m_col + 1);
    checkMove(m_row - 1, m_col + 2);
    checkMove(m_row - 2, m_col + 1);
    checkMove(m_row + 1, m_col - 2);
    checkMove(m_row + 2, m_col - 1);
    checkMove(m_row - 1, m_col - 2);
    checkMove(m_row - 2, m_col - 1);
}