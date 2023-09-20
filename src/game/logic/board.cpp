#include "board.h"

namespace views = std::views;

Board::Board() { std::fill(m_board.begin(), m_board.end(), nullptr); }

auto Board::operator[](Square square) -> Piece*& {
    return m_board[square.row * 8 + square.col];
}

auto Board::operator[](Square square) const -> Piece* const& {
    return m_board[square.row * 8 + square.col];
}

auto Board::move(Square from, Square to) -> void {
    (*this)[to]   = (*this)[from];
    (*this)[from] = nullptr;
}

void Board::pseudoUpdate() {
    BitBoard white_pos;
    BitBoard black_pos;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (Piece* ocupied = (*this)[{i, j}]; ocupied) {
                if (ocupied->isWhite()) {
                    white_pos.set({i, j});
                } else {
                    black_pos.set({i, j});
                }
            }
        }
    }
    fmt::print("white pos:\n{}\nblack_pos:\n{}\n", white_pos, black_pos);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (Piece* ocupied = (*this)[{i, j}]; ocupied) {
                ocupied->setPos({i, j});
                ocupied->updatePseudoLegalMoves(white_pos, black_pos);
            }
        }
    }
}

auto Board::getCoverage() const -> std::pair<BitBoard, BitBoard> {
    BitBoard white_coverage;
    BitBoard black_coverage;
    for (auto const piece :
         m_board | views::filter([](auto ptr) { return ptr != nullptr; })) {
        if (piece->isWhite()) {
            white_coverage |= piece->getCoverage();
        } else {
            black_coverage |= piece->getCoverage();
        }
    }
    return {white_coverage, black_coverage};
}