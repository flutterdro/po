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

Knight::Knight(bool color, int row, int col) 
    : Piece(color, row, col) {
    RM& rm = RM::getInstance();
    auto sprite_opt = rm.getSprite("piece");
    if (sprite_opt.has_value()) {
        m_sprite = sprite_opt.value();
    } else {
        LOG(LoggerLvl::WARNING, "Couldn't find a sprite in resource manager\n");
    }
    if (color) {
        auto texture_opt = rm.getTexture("white-pawn");
        if (texture_opt.has_value()) {
            m_texture = texture_opt.value();
        } else {
            LOG(LoggerLvl::WARNING, "Couldn't find a texture in resource manager\n");
        }
    } else {
        auto texture_opt = rm.getTexture("black-pawn");
        if (texture_opt.has_value()) {
            m_texture = texture_opt.value();
        } else {
            LOG(LoggerLvl::WARNING, "Couldn't find a texture in resource manager\n");
        }
    }
}

void Knight::draw(float pos_x, float pos_y) {
    m_sprite.draw(m_texture, {pos_x, pos_y});
}

void Knight::updateLegalMoves(const Board& board) {
    int row, col;
    m_legal_moves.resize(0);
    auto checkMove = [&](int row, int col) {
        if (col < 8 && col >= 0 && 
            row < 8 && row  >= 0) {
            const Piece *const ocupied = board[row][col];
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

Bishop::Bishop(bool color, int row, int col)
    : Piece(color, row, col) {
    RM& rm = RM::getInstance();
    auto sprite_opt = rm.getSprite("piece");
    if (sprite_opt.has_value()) {
        m_sprite = sprite_opt.value();
    } else {
        LOG(LoggerLvl::WARNING, "Couldn't find a sprite in resource manager\n");
    }
    if (color) {
        auto texture_opt = rm.getTexture("white-pawn");
        if (texture_opt.has_value()) {
            m_texture = texture_opt.value();
        } else {
            LOG(LoggerLvl::WARNING, "Couldn't find a texture in resource manager\n");
        }
    } else {
        auto texture_opt = rm.getTexture("black-pawn");
        if (texture_opt.has_value()) {
            m_texture = texture_opt.value();
        } else {
            LOG(LoggerLvl::WARNING, "Couldn't find a texture in resource manager\n");
        }
    }
}

void Bishop::updateLegalMoves(const Board& board) {
    m_legal_moves.resize(0);

    //check all four diagonals
    const int max = std::max(m_row, m_col);
    for (int i = 1; max + i < 8; ++i) {
        const Piece *const ocupied = board[m_row + i][m_col + i]; //JUST SHUTUP ITS LENGTH IS 8 I DON"T NEED 64 BITS
        if (ocupied) {
            if (ocupied->getColor() != color) {
                m_legal_moves.push_back({m_row + i, m_col + i});
            }
            break;
        }
        m_legal_moves.push_back({m_row + i, m_col + i});
    }
    const int min = std::min(m_row, m_col);
    for (int i = 1; min - i > 0; ++i) {
        const Piece *const ocupied = board[m_row - i][m_col - i];
        if (ocupied) {
            if (ocupied->getColor() != color) {
                m_legal_moves.push_back({m_row - i, m_col - i});
            }
            break;
        }
        m_legal_moves.push_back({m_row - i, m_col - i});
    }
    for (int i = 1; (m_row - i > 0) && (m_col + i < 8); ++i) {
        const Piece *const ocupied = board[m_row - i][m_col + i];
        if (ocupied) {
            if (ocupied->getColor() != color) {
                m_legal_moves.push_back({m_row - i, m_col + i});
            }
            break;
        }
        m_legal_moves.push_back({m_row - i, m_col + i});
    }
    for (int i = 1; (m_col - i > 0) && (m_row + i < 8); ++i) {
        const Piece *const ocupied = board[m_row + i][m_col - i];
        if (ocupied) {
            if (ocupied->getColor() != color) {
                m_legal_moves.push_back({m_row + i, m_col - i});
            }
            break;
        }
        m_legal_moves.push_back({m_row + i, m_col - i});
    }
}

void Bishop::draw(float pos_x, float pos_y) {
    m_sprite.draw(m_texture, {pos_x, pos_y});
}