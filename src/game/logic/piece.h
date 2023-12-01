#ifndef PIECE_H__
#define PIECE_H__

#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>

#include "../utilities/bitboard.h"

/**
 * I rewrote this code in a style that I like to call:
 * "We do a little bloating"
 * Maaaan I love templates
 */
namespace chess {
template<bool> class Pawn;
class piece {
public: 
    enum class type {
        king = 0,
        pawn,
        knight,
        bishop,
        queen,
        rook,
        none,
    };
    enum class color : unsigned {
        black,
        white,
    };
private:
    struct piece_concept {
        virtual ~piece_concept() = default;

        virtual auto do_pseudo_update(bitboard, bitboard, square)
            -> std::pair<bitboard, bitboard> = 0;
        virtual auto do_type() const
            -> piece::type = 0;
        virtual auto clone() const 
            -> std::unique_ptr<piece_concept> = 0;
    };

    template<typename PieceT>
    struct piece_model : public piece_concept {
        explicit piece_model(PieceT piece)
            : m_piece{std::move(piece)} {}

        auto do_pseudo_update(bitboard white_ocupation, bitboard black_ocupation, square pos) 
            -> std::pair<bitboard, bitboard> override {
            bitboard protected_squares = m_piece.pseudo_update(white_ocupation | black_ocupation, pos);
            bitboard available_moves = protected_squares;
            if constexpr (PieceT::get_color() == piece::color::white) {
                available_moves &= ~white_ocupation;
            } else {
                available_moves &= ~black_ocupation;
            }
            return {available_moves, protected_squares};
        }
        auto do_type() const 
            -> enum piece::type override {
            return m_piece.get_type();
        }
        auto clone() const 
            -> std::unique_ptr<piece_concept> override {
            return std::make_unique<piece_model>(*this);
        }

        [[no_unique_address]]PieceT m_piece;
    };

    std::unique_ptr<piece_concept> pimpl;
private:
    struct piece_data {
        bitboard available_moves{0};
        bitboard protected_squares{0};
        square current{0};
        piece::color color{piece::color::white};
    };
public:
    template<typename PieceT>
    piece(PieceT piece, square square)
        : pimpl{std::make_unique<piece_model<PieceT>>(std::move(piece))},
          m_data{.color{PieceT::get_color()}, .current{square}} {}
    piece(piece const& other) 
        : pimpl{other.pimpl->clone()},
          m_data{other.m_data} {}
    auto operator=(piece const& other) 
        -> piece&;
    auto pseudo_update(bitboard white_ocupation, bitboard black_ocupation)
        -> void {
        std::tie(m_data.available_moves, m_data.protected_squares) 
            = pimpl->do_pseudo_update(white_ocupation, black_ocupation, m_data.current);
    }
    auto get_moves() const noexcept
        -> bitboard {
        return m_data.available_moves;
    }
    auto get_pos() const noexcept 
        -> square {
        return m_data.current;
    }
    auto get_coverage() const noexcept
        -> bitboard {
        return m_data.protected_squares;
    }
    auto get_color() const noexcept 
        -> piece::color {
        return m_data.color;
    }
    auto is_valid_move(square to) const noexcept
        -> bool { return m_data.available_moves.test(to); }

    auto is_attacked(bitboard enemy_attacking) const
        -> bool { return enemy_attacking.test(m_data.current); }
    auto get_type() const noexcept
        -> enum piece::type {
        return pimpl->do_type();
    }
    auto invalidate_move(square square) noexcept 
        -> void {
        m_data.available_moves.reset(square);
    }
    auto validate_move(square square) noexcept 
        -> void {
        m_data.available_moves.set(square);
    }
    auto set_pos(square square)
        -> void {
        m_data.current = square;
    }
private:
    piece_data m_data;
    type m_type;
};

auto create_piece(piece::type type, piece::color color, square pos)
    -> piece;

constexpr auto flip_color(piece::color color) noexcept
    -> piece::color {
    return static_cast<piece::color>(std::to_underlying(color) ^ 1);
}

template<piece::color color>
class knight {
public:
    auto pseudo_update(bitboard, square pos)
        -> bitboard {
        auto const row = pos.row();
        auto const col = pos.col();
        bitboard ret;
        auto set = [&](auto row, auto col) {
            if (not (row > 7 || row < 0 || col > 7 || col < 0)) {
                ret.set(to_square(row, col));
            }
        };
        set(row + 1, col + 2);
        set(row + 2, col + 1);
        set(row - 1, col + 2);
        set(row - 2, col + 1);
        set(row + 1, col - 2);
        set(row + 2, col - 1);
        set(row - 1, col - 2);
        set(row - 2, col - 1);

        return ret;
    }
    constexpr auto get_type() const noexcept  
        -> piece::type { return piece::type::knight; }
    static constexpr auto get_color() noexcept
        -> piece::color { return color; }

};

void test();
auto get_bishop_moves(bitboard, square)
    -> bitboard;

template<piece::color color>
class bishop {
public:
    static constexpr auto naive_update(bitboard const ocupied, square const pos)
        -> bitboard {
        bitboard ret;
        int const row = static_cast<int>(pos.row());
        int const col = static_cast<int>(pos.col());
        auto check_move = [&ret, ocupied](int const row, int const col) -> bool {
            if (row > 7 || row < 0 || col > 7 || col < 0) return false; 
            ret.set(to_square(row, col));
            if (ocupied.test(to_square(row, col))) {
                return false;
            }
            return true;
        };
        for (int i = 1; check_move(row + i, col + i); ++i) {}
        for (int i = 1; check_move(row - i, col - i); ++i) {}
        for (int i = 1; check_move(row - i, col + i); ++i) {}
        for (int i = 1; check_move(row + i, col - i); ++i) {}

        return ret;
    }
    auto pseudo_update(bitboard ocupied, square pos)
        -> bitboard { return get_bishop_moves(ocupied, pos); }
    constexpr auto get_type() const noexcept  
        -> piece::type { return piece::type::bishop; }
    static constexpr auto get_color() noexcept
        -> piece::color { return color; }
};

auto get_rook_moves(bitboard, square)
    -> bitboard;
template<piece::color color>
class rook {
public:
    static constexpr auto naive_update(bitboard ocupied, square pos)
        -> bitboard { 
        bitboard ret;
        auto const row = pos.row();
        auto const col = pos.col();
        auto check_move = [&](int row, int col) -> bool {
            if (row > 7 || row < 0 || col > 7 || col < 0) { return false; }
            ret.set(to_square(row, col));
            if (ocupied.test(to_square(row, col))) {
                return false;
            }
            return true;
        };
        for (int i = 1; check_move(row + i, col); ++i) {}
        for (int i = 1; check_move(row - i, col); ++i) {}
        for (int i = 1; check_move(row, col + i); ++i) {}
        for (int i = 1; check_move(row, col - i); ++i) {}

        return ret; 
    }
    auto pseudo_update(bitboard ocupied, square pos)
        -> bitboard {
        return get_rook_moves(ocupied, pos);
    }
    constexpr auto get_type() const noexcept  
        -> piece::type { return piece::type::rook; }
    static constexpr auto get_color() noexcept
        -> piece::color { return color; }
private:
};

template<piece::color color>
class queen {
public:
    auto pseudo_update(bitboard ocupied, square pos)
        -> bitboard { return get_bishop_moves(ocupied, pos) | get_rook_moves(ocupied, pos); }
    constexpr auto get_type() const noexcept  
        -> piece::type { return piece::type::queen; }
    static constexpr auto get_color() noexcept
        -> piece::color { return color; }
private:
};

template<piece::color color>
class pawn {
public:
    auto pseudo_update(bitboard ocupied, square pos) 
        -> bitboard {
        bitboard ret;
        auto const row = pos.row();
        auto const col = pos.col();

        auto set = [&](square square) -> bool {
            if (not square.is_out_of_bounds() and not ocupied.test(square)) {
                ret.set(square);
                return true;
            }
            return false;
        };
        if constexpr (color == piece::color::white) {
            if (not to_square(row + 1, col - 1).is_out_of_bounds() and ocupied.test(to_square(row + 1, col - 1))) 
                ret.set(to_square(row + 1, col - 1));
            if (ocupied.test(to_square(row + 1, col + 1))) ret.set(to_square(row + 1, col + 1));
            if (set(to_square(row + 1, col + 0)) and pos.row() == 1) {
                set(to_square(row + 2, col + 0));
            }
        } else {
            if (not to_square(row + 1, col - 1).is_out_of_bounds() and ocupied.test(to_square(row - 1, col - 1))) 
                ret.set(to_square(row - 1, col - 1));
            if (ocupied.test(to_square(row - 1, col + 1))) ret.set(to_square(row - 1, col + 1));
            if (set(to_square(row - 1, col + 0)) and pos.row() == 6) {
                set(to_square(row - 2, col + 0));
            }
        }

        return ret;
    }
    constexpr auto get_type() const noexcept  //
        -> piece::type { return piece::type::pawn; }
    static constexpr auto get_color() noexcept
        -> piece::color { return color; }
};

template<piece::color color>
class king {
public:
    auto pseudo_update(bitboard, square pos) 
        -> bitboard {
        bitboard ret;
        auto const row = pos.row();
        auto const col = pos.col();

        auto set = [&](square square) {
            if (not square.is_out_of_bounds()) {
                ret.set(square);
            }
        };
        
        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) { set(to_square(row + i, col + j)); }
        }
        ret.reset(pos);

        return ret;
    }
    constexpr auto get_type() const noexcept  //
        -> piece::type { return piece::type::king; }
    static constexpr auto get_color() noexcept
        -> piece::color { return color; }
};
}

#endif