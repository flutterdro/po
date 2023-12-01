#ifndef BOARD_H__
#define BOARD_H__

#include <array>
#include <utility>

#include "../utilities/bitboard.h"
#include "../utilities/ibr.h"
#include "piece.h"
namespace chess {

// 0b'000000000000'000'000'000'00'0'000000'000000
//                                        ^0-5 bits source square
//                                 ^6-11 bits destination square
//                               ^12 bit did capture ocur?
//                             ^13-14 bits move type
//                        ^14-16 promotion type
//                    ^17-19 capture type
//                ^20-22 bits piece to move type
//   ^23-34 bits config resets
// ^35 index of capture piece
struct move {
    using utype = u64;
    enum class type : utype {
        null       = 0b000,
        castle     = 0b010,
        promotion  = 0b100,
        en_passant = 0b110,
    };
    enum castling : unsigned char {
        short_white = 1,
        long_white  = short_white << 1,
        short_black = short_white << 2,
        long_black  = short_white << 3,
        white       = short_white | long_white,
        black       = short_black | long_black,
        kingside    = short_black | short_white,
        queenside   = long_black  | long_white,
    };
    [[nodiscard]] constexpr auto clone() const noexcept
        -> move { return *this; }
    [[nodiscard]] constexpr auto source() const noexcept
        -> square { return {static_cast<unsigned>(underlying_bits & 0b111111)}; }
    [[nodiscard]] constexpr auto destination() const noexcept
        -> square { return {static_cast<unsigned>((underlying_bits >> 6) & 0b111111)}; }
    [[nodiscard]] constexpr auto is_capture() const noexcept
        -> bool { return underlying_bits & 1u << 12; }
    [[nodiscard]] constexpr auto move_type() const noexcept
        -> type { return static_cast<type>((underlying_bits >> 12) & 0b110); }
    [[nodiscard]] constexpr auto promoted_piece_type() const noexcept
        -> piece::type { return static_cast<piece::type>(underlying_bits >> 14 & 0b111); }
    [[nodiscard]] constexpr auto captured_piece_type() const noexcept
        -> piece::type { return static_cast<piece::type>(underlying_bits >> 17 & 0b111); }
    [[nodiscard]] constexpr auto piece_tomove_type() const noexcept
        -> piece::type { return static_cast<piece::type>(underlying_bits >> 20 & 0b111); }
    [[nodiscard]] constexpr auto captured_piece_index() const noexcept
        -> size_t { return underlying_bits >> 35; }
    [[nodiscard]] constexpr auto enpassant_invalid() const noexcept
        -> bool { return underlying_bits >> 23 & 0b11111111; }
    [[nodiscard]] constexpr auto castle_invalid() const noexcept
        -> castling { return static_cast<castling>(underlying_bits >> 31 & 0b1111); }

    constexpr auto set_source(square square_) noexcept
        -> void { underlying_bits |= square_.pos; }
    constexpr auto set_destination(square square_) noexcept
        -> void { underlying_bits |= square_.pos << 6; }
    constexpr auto mark_capture() noexcept
        -> void { underlying_bits |= 1u << 12; }
    constexpr auto set_move_type(type type_) noexcept
        -> void { underlying_bits |= std::to_underlying(type_) << 12; }
    constexpr auto set_promoted_piece_type(piece::type type_) noexcept
        -> move& { underlying_bits |= std::to_underlying(type_) << 14; return *this; }
    constexpr auto set_captured_piece_type(piece::type type_) noexcept
        -> void { underlying_bits |= std::to_underlying(type_) << 17; }
    constexpr auto set_piece_tomove_type(piece::type type_) noexcept
        -> void { underlying_bits |= std::to_underlying(type_) << 20; }
    constexpr auto set_captured_piece_index(size_t index) noexcept
        -> void { underlying_bits |= index << 35; }
    constexpr auto set_enpassant_invalid(unsigned col) noexcept
        -> void { underlying_bits |= 1_u64 << 23 + col; }
    constexpr auto set_castle_invalid(move::castling castling) noexcept
        -> void { underlying_bits |= static_cast<u64>(castling) << 31; }
    constexpr auto set_castle_invalid(piece::color color, move::castling castling) noexcept
    -> void {
        underlying_bits |= static_cast<u64>(
            color == piece::color::white ?
                (castling & move::castling::white) :
                (castling & move::castling::black)
        ) << 31;
    }

    utype underlying_bits{0u};
};

class board {
    struct config {
        unsigned char m_enpassant;
        move::castling m_castling;
        constexpr auto invalidate_castle(move::castling castling) noexcept
            -> void { m_castling = static_cast<move::castling>(m_castling & ~castling); }
        constexpr auto validate_castle(move::castling castling) noexcept
            -> void { m_castling = static_cast<move::castling>(m_castling | castling); }
        [[nodiscard]] constexpr auto castle(move::castling castling) const noexcept
            -> bool { return castling & m_castling; }
        [[nodiscard]] constexpr auto castle(piece::color color, move::castling castling) const noexcept
            -> bool {
            return color == piece::color::white ?
                (castling & move::castling::white) & m_castling :
                (castling & move::castling::black) & m_castling;
        }
        [[nodiscard]] constexpr auto enpassant(unsigned col) const noexcept
            -> bool { return m_enpassant & 1 << col; }
        constexpr auto validate_enpasant(unsigned col) noexcept
            -> void { m_enpassant |= 1 << col; }
        constexpr auto invalidate_enpasant(unsigned col) noexcept
            -> void { m_enpassant &= ~(1 << col); }

    };
public:
    board();
    board(board const&) = default;
    // board(board&&) = delete;
    auto operator=(board const&)
        -> board& = default;
    // auto operator=(board&&)
    //     -> board& = delete;
    auto operator[](square)  
        -> std::optional<std::reference_wrapper<piece>>;
    auto operator[](square) const  
        -> std::optional<std::reference_wrapper<piece const>>;
    auto gen_move_info(square source, square destination) const noexcept
        -> struct move;
    auto do_move(struct move const) noexcept
        -> void;
    auto undo_move(struct move) noexcept
        -> void;
    auto move(square from, square to)  
        -> std::optional<square>;
    auto soft_update()  
        -> void;
    auto hard_update()  
        -> void;
    auto get_coverage() const noexcept
        -> std::pair<bitboard, bitboard>;
    auto get_pieces() const noexcept
        -> std::array<std::optional<piece>, 32> const&;
    auto get_config() const noexcept 
        -> config;
    auto is_promotable() const noexcept
        -> bool;
    auto turn_to_move() const noexcept
        -> piece::color;
    auto promote(square, PieceType)
        -> void;
    auto evaluate()
        -> float;
    auto swap(board&) noexcept
        -> void;
    auto set_turn(piece::color) noexcept 
        -> void;
    auto reset()
        -> void;
    auto to_ibr()
        -> IntermediateBoardRepresentation;
    friend struct fmt::formatter<board>;
private:
    template<typename PieceT>
    auto add_piece(square) 
        -> void;
    auto at(square) noexcept
        -> std::optional<piece>&;
    auto at(square) const noexcept
        -> std::optional<piece> const&;
private:
    std::array<std::optional<char>, 64> m_board;
    std::array<std::optional<piece>, 32> m_piece_box;
    config m_config;
    piece::color m_turn_to_move;
};
static size_t sizeakdjc = 0;
template<typename PieceT>
auto board::add_piece(square at) 
    -> void {
    m_piece_box[sizeakdjc] = piece{PieceT{}, at};
    m_board[at.pos] = sizeakdjc;
    ++sizeakdjc;
    sizeakdjc %= 32;
}
}

namespace fmt {
template<>
struct formatter<chess::board> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(chess::board const& v, FormatContext& ctx) {
        std::string ft;
        for (auto row : vws::iota(0, 8)) {
            for (auto col : vws::iota(0, 8)) {
                if (not v[chess::to_square(row, col)].has_value()) {
                    ft += '0';
                } else {
                    ft += [](auto const& piece) {
                        switch(piece->get().type()) {
                        using enum chess::piece::type;
                        case pawn: return 'P';
                        case rook: return 'R';
                        case king: return 'K';
                        case queen: return 'Q';
                        case bishop: return 'B';
                        case knight: return 'H';
                        }
                    }(v[chess::to_square(row, col)]);
                }
            }
            ft += '\n';
        }
        return format_to(ctx.out(), "{}", ft.c_str());
    }
};
}



#endif