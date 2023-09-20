#ifndef BOARD_H__
#define BOARD_H__

#include <array>
#include <ranges>

#include "../utilities/bitboard.h"
#include "piece.h"

class Board {
public:
    Board();
    auto operator[](Square)  //
        -> Piece*&;
    auto operator[](Square) const  //
        -> Piece* const&;
    auto getCoverage() const  //
        -> std::pair<BitBoard, BitBoard>;
    auto move(Square from, Square to)  //
        -> void;
    auto pseudoUpdate()  //
        -> void;
private:
    std::array<Piece*, 64> m_board;
};

#endif