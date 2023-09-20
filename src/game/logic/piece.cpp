#include "piece.h"

using namespace std::string_literals;

namespace views = std::views;
Piece::Piece(bool is_white, Square initial_pos, char const* texture_handle)
    : m_is_white(is_white), m_current(initial_pos) {
    RM& rm          = RM::getInstance();
    auto sprite_opt = rm.getSprite("piece");
    if (sprite_opt.has_value()) {
        m_sprite = sprite_opt.value();
    } else {
        LOG(LoggerLvl::WARNING,
            "Couldn't find a piece sprite in resource manager\n");
    }
    std::string handle =
        (this->m_is_white ? "white-"s : "black-"s) + texture_handle;
    auto texture_opt = rm.getTexture(handle);
    if (texture_opt.has_value()) {
        m_texture = texture_opt.value();
    } else {
        LOG(LoggerLvl::WARNING,
            "Couldn't find a texture: {} in resource manager\n", handle);
    }
}

auto Piece::invalidateMove(Square square) -> void {
    m_p_legal_moves.reset(square);
}

auto Piece::isWhite() const noexcept -> bool { return m_is_white; }

auto Piece::getPseudoLegalMoves() const noexcept -> BitBoard {
    return m_p_legal_moves;
}

auto Piece::getPos() const noexcept -> const Square { return m_current; }

auto Piece::getCoverage() const noexcept -> BitBoard {
    return m_attacked_squares;
}

auto Piece::isLegalMove(Square square) const noexcept -> bool {
    return m_p_legal_moves.test(square);
}

void Piece::setPos(Square pos) { m_current = pos; }

// void update_b(Board& board) {
//     auto is_not_nullptr = [](auto const ptr) { return ptr != nullptr; };
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             Piece* piece = board[row][col];
//             if (piece) {
//                 piece->setPos({row, col});
//                 piece->updateLegalMoves(board);
//             }
//         }
//     }
// }

// void Piece::move(Square to_pos, Board& board) {
//     board[m_current.row][m_current.col] = nullptr;
//     board[to_pos.row][to_pos.col]       = this;
//     m_current                           = to_pos;
// }

void Piece::draw(float pos_x, float pos_y) {
    RM& rm      = RM::getInstance();
    auto shader = *rm.getShader("piece");
    if (m_is_white) {
        shader.set("white", 1);
    } else {
        shader.set("white", 0);
    }
    m_sprite.draw(m_texture, {pos_x, pos_y});
}

Knight::Knight(bool is_white, Square initial_pos)
    : Piece(is_white, initial_pos, "knight") {}

[[nodiscard]] auto Knight::type() const noexcept -> PieceType {
    return PieceType::Knight;
}

void Knight::updatePseudoLegalMoves(BitBoard white_pos, BitBoard black_pos) {
    auto const [m_row, m_col] = m_current;
    m_p_legal_moves.reset();
    m_attacked_squares.reset();
    auto checkMove = [&](Square square) {
        if (!square.isOutOfBounds()) {
            if (m_is_white && !white_pos.test(square)) {
                m_p_legal_moves.set(square);
            } else if (!m_is_white && !black_pos.test(square)) {
                m_p_legal_moves.set(square);
            }
            m_attacked_squares.set(square);
        }
    };
    checkMove({m_row + 1, m_col + 2});
    checkMove({m_row + 2, m_col + 1});
    checkMove({m_row - 1, m_col + 2});
    checkMove({m_row - 2, m_col + 1});
    checkMove({m_row + 1, m_col - 2});
    checkMove({m_row + 2, m_col - 1});
    checkMove({m_row - 1, m_col - 2});
    checkMove({m_row - 2, m_col - 1});
}

Bishop::Bishop(bool is_white, Square initial_pos)
    : Piece(is_white, initial_pos, "bishop") {}

[[nodiscard]] auto Bishop::type() const noexcept -> PieceType {
    return PieceType::Bishop;
}

void Bishop::updatePseudoLegalMoves(BitBoard white_pos, BitBoard black_pos) {
    m_p_legal_moves.reset();
    m_attacked_squares.reset();
    auto const [m_row, m_col] = m_current;
    // I hate myself for doing this
    auto check_move = [&](Square square) -> bool {
        if (square.isOutOfBounds()) { return false; }
        m_attacked_squares.set(square);
        if ((white_pos | black_pos).test(square)) {
            if ((!m_is_white && white_pos.test(square)) ||
                (m_is_white && black_pos.test(square))) {
                m_p_legal_moves.set(square);
            }
            return false;
        }
        m_p_legal_moves.set(square);
        return true;
    };
    for (int i = 1; check_move({m_row + i, m_col + i}); ++i) {}
    for (int i = 1; check_move({m_row - i, m_col - i}); ++i) {}
    for (int i = 1; check_move({m_row - i, m_col + i}); ++i) {}
    for (int i = 1; check_move({m_row + i, m_col - i}); ++i) {}
}

Rook::Rook(bool is_white, Square initial_pos)
    : Piece(is_white, initial_pos, "rook") {}

auto Rook::type() const noexcept -> PieceType { return PieceType::Rook; }

void Rook::updatePseudoLegalMoves(BitBoard white_pos, BitBoard black_pos) {
    m_p_legal_moves.reset();
    m_attacked_squares.reset();
    auto const [m_row, m_col] = m_current;
    // this code sucks but at least it is maintenable!
    auto check_move = [&](Square square) -> bool {
        if (square.isOutOfBounds()) { return false; }
        m_attacked_squares.set(square);
        if ((white_pos | black_pos).test(square)) {
            if ((!m_is_white && white_pos.test(square)) ||
                (m_is_white && black_pos.test(square))) {
                m_p_legal_moves.set(square);
            }
            return false;
        }
        m_p_legal_moves.set(square);
        return true;
    };
    for (int i = 1; check_move({m_row + i, m_col}); ++i) {}
    for (int i = 1; check_move({m_row - i, m_col}); ++i) {}
    for (int i = 1; check_move({m_row, m_col + i}); ++i) {}
    for (int i = 1; check_move({m_row, m_col - i}); ++i) {}
}

Queen::Queen(bool is_white, Square initial_pos)
    : Piece(is_white, initial_pos, "queen") {}

auto Queen::type() const noexcept -> PieceType { return PieceType::Queen; }

void Queen::updatePseudoLegalMoves(BitBoard white_pos, BitBoard black_pos) {
    m_p_legal_moves.reset();
    m_attacked_squares.reset();
    auto const [m_row, m_col] = m_current;
    // god save the queen
    auto check_move = [&](Square square) -> bool {
        if (square.isOutOfBounds()) { return false; }
        m_attacked_squares.set(square);
        if ((white_pos | black_pos).test(square)) {
            if ((!m_is_white && white_pos.test(square)) ||
                (m_is_white && black_pos.test(square))) {
                m_p_legal_moves.set(square);
            }
            return false;
        }
        m_p_legal_moves.set(square);
        return true;
    };
    for (int i = 1; check_move({m_row + i, m_col}); ++i) {}
    for (int i = 1; check_move({m_row - i, m_col}); ++i) {}
    for (int i = 1; check_move({m_row, m_col + i}); ++i) {}
    for (int i = 1; check_move({m_row, m_col - i}); ++i) {}
    for (int i = 1; check_move({m_row + i, m_col + i}); ++i) {}
    for (int i = 1; check_move({m_row - i, m_col - i}); ++i) {}
    for (int i = 1; check_move({m_row - i, m_col + i}); ++i) {}
    for (int i = 1; check_move({m_row + i, m_col - i}); ++i) {}
    // print64(m_p_legal_moves);
}

Pawn::Pawn(bool is_white, Square initial_pos)
    : Piece(is_white, initial_pos, "pawn") {}

auto Pawn::type() const noexcept -> PieceType { return PieceType::Pawn; }

void Pawn::updatePseudoLegalMoves(BitBoard white_pos, BitBoard black_pos) {
    m_p_legal_moves.reset();
    m_attacked_squares.reset();
    auto const [m_row, m_col] = m_current;
    Square const left         = m_is_white ? Square{m_row - 1, m_col - 1} :
                                             Square{m_row + 1, m_col - 1};
    Square const right        = m_is_white ? Square{m_row - 1, m_col + 1} :
                                             Square{m_row + 1, m_col + 1};
    Square const ahead        = m_is_white ? Square{m_row - 1, m_col + 0} :
                                             Square{m_row + 1, m_col + 0};
    Square const aheadx2      = m_is_white ? Square{m_row - 2, m_col + 0} :
                                             Square{m_row + 2, m_col + 0};

    if (!left.isOutOfBounds()) {
        m_attacked_squares.set(left);
        if (((m_is_white && black_pos.test(left)) ||
             (!m_is_white && white_pos.test(left)))) {
            m_p_legal_moves.set(left);
        }
    }
    if (!right.isOutOfBounds()) {
        m_attacked_squares.set(right);
        if (((m_is_white && black_pos.test(right)) ||
             (!m_is_white && white_pos.test(right)))) {
            m_p_legal_moves.set(right);
        }
    }
    if (!aheadx2.isOutOfBounds()) {
        if (!(white_pos | black_pos).test(aheadx2)) {
            m_p_legal_moves.set(aheadx2);
        }
    }
    if (!ahead.isOutOfBounds()) {
        if (!(white_pos | black_pos).test(ahead)) {
            m_p_legal_moves.set(ahead);
        } else {
            m_p_legal_moves.reset(aheadx2);
        }
    }
}

King::King(bool is_white, Square initial_pos)
    : Piece(is_white, initial_pos, "king") {}

auto King::type() const noexcept -> PieceType { return PieceType::King; }

void King::updatePseudoLegalMoves(BitBoard white_pos, BitBoard black_pos) {
    m_p_legal_moves.reset();
    m_attacked_squares.reset();
    auto const [m_row, m_col] = m_current;

    auto check_move           = [&](Square square) {
        if (!square.isOutOfBounds()) {
            m_attacked_squares.set(square);
            if (!((!m_is_white && black_pos.test(square)) ||
                  (m_is_white && white_pos.test((square))))) {
                m_p_legal_moves.set(square);
            }
        }
    };
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) { check_move({m_row + i, m_col + j}); }
    }
}

[[noreturn]] void unreachable() {
#ifdef _MSC_VER
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
