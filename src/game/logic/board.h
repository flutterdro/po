#ifndef BOARD_H__
#define BOARD_H__

#include <array>
#include <ranges>
#include <algorithm>
#include <exception>
#include <cstddef>
#include <concepts>

#include <glm/glm.hpp>

#include "../utilities/bitboard.h"
#include "piece.h"

class Board {
public:
    Board();
    Board(Board const&) = default;
    Board(Board&&) = default;
    auto operator=(Board const&)
        -> Board& = default;
    auto operator=(Board&&)
        -> Board& = default;
    auto operator[](Square)  
        -> std::optional<std::reference_wrapper<Piece>>;
    auto operator[](Square) const  
        -> std::optional<std::reference_wrapper<Piece const>>;
    auto move(Square from, Square to)  
        -> std::optional<Square>;
    auto soft_update()  
        -> void;
    auto hard_update()  
        -> void;
    auto get_coverage() const noexcept
        -> std::pair<BitBoard, BitBoard>;
    auto get_pieces() const noexcept
        -> std::vector<std::optional<Piece>> const&;
    auto get_config() const noexcept 
        -> BoardConfig;
    auto is_promotable() const noexcept
        -> bool;
    auto is_white_turn() const noexcept
        -> bool;
    auto promote(Square, PieceType)
        -> void;
    auto evaluate()
        -> float;
    auto swap(Board&)
        -> void;
    auto set_turn(bool) noexcept 
        -> void;
    auto reset()
        -> void;
    auto to_ibr()
        -> IntermediateBoardRepresentation;
    friend struct fmt::formatter<Board>;
private:
    template<typename PieceT>
    auto add_piece(Square) 
        -> void;
    auto at(Square) noexcept
        -> std::optional<Piece>&;
    auto at(Square) const noexcept
        -> std::optional<Piece> const&;
private:
    std::array<std::optional<int>, 64> m_board;
    std::vector<std::optional<Piece>> m_piece_box;
    bool m_promotable;
    bool m_is_white_turn{true};
    BoardConfig m_config;

};

template<typename PieceT>
auto Board::add_piece(Square at) 
    -> void {
    m_piece_box.emplace_back(Piece{PieceT{}, at});
    m_board[at.to_index()] = m_piece_box.size() - 1;
}

namespace fmt {
template<>
struct formatter<Board> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(Board const& v, FormatContext& ctx) {
        std::string ft;
        for (auto row : vws::iota(0, 8)) {
            for (auto col : vws::iota(0, 8)) {
                if (not v[{row, col}].has_value()) {
                    ft += '0';
                } else {
                    ft += [](auto const& piece) {
                        switch(piece->get().type()) {
                        using enum PieceType;
                        case Pawn: return 'P';
                        case Rook: return 'R';
                        case King: return 'K';
                        case Queen: return 'Q';
                        case Bishop: return 'B';
                        case Knight: return 'H';
                        }
                    }(v[{row, col}]);
                }
            }
            ft += '\n';
        }
        return format_to(ctx.out(), "{}", ft.c_str());
    }
};
}


#endif