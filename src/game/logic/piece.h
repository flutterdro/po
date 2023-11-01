#ifndef PIECE_H__
#define PIECE_H__

#include <array>
#include <vector>
#include <tuple>
#include <string>
#include <filesystem>
#include <ranges>
#include <fmt/core.h>

#include "../utilities/ibr.h"
#include "../utilities/bitboard.h"

/**
 * I rewrote this code in a style that I like to call:
 * "We do a little bloating"
 * Maaaan I love templates
 */

template<bool> class Pawn;
class Piece {
private:
    struct PieceConcept {
        virtual ~PieceConcept() = default;

        virtual auto do_pseudo_update(BitBoard, BitBoard, Square)
            -> std::pair<BitBoard, BitBoard> = 0;
        virtual auto do_type() const
            -> PieceType = 0;
        virtual auto clone() const 
            -> std::unique_ptr<PieceConcept> = 0;
    };

    template<typename PieceT>
    struct PieceModel : public PieceConcept {
        PieceModel(PieceT piece) 
            : m_piece(std::move(piece)) {}

        auto do_pseudo_update(BitBoard white_ocupation, BitBoard black_ocupation, Square pos) 
            -> std::pair<BitBoard, BitBoard> override {
            BitBoard protected_squares = m_piece.pseudo_update(white_ocupation | black_ocupation, pos);
            BitBoard available_moves = protected_squares;
            if constexpr (PieceT::is_white()) {
                available_moves &= ~white_ocupation;
            } else {
                available_moves &= ~black_ocupation;
            }
            return {available_moves, protected_squares};
        }
        auto do_type() const 
            -> PieceType override {
            return m_piece.type();
        }
        auto clone() const 
            -> std::unique_ptr<PieceConcept> override {
            return std::make_unique<PieceModel>(*this);
        }

        PieceT m_piece;
    };

    std::unique_ptr<PieceConcept> pimpl;
private:
    struct PieceData {
        BitBoard available_moves{0};
        BitBoard protected_squares{0};
        Square current{0, 0};
        bool is_first_move{true};
        bool is_white{true};
    };
public:
    template<typename PieceT>
    Piece(PieceT piece, Square square)
        : pimpl{std::make_unique<PieceModel<PieceT>>(std::move(piece))},
          m_data{.is_white{PieceT::is_white()}, .current{square}} {}
    Piece(Piece const& other) 
        : pimpl{other.pimpl->clone()},
          m_data{other.m_data} {}
    auto operator=(Piece const& other) 
        -> Piece& {
        Piece tmp(other);
        std::swap(pimpl, tmp.pimpl);
        std::swap(m_data, tmp.m_data);
        return *this;
    }
    auto pseudo_update(BitBoard white_ocupation, BitBoard black_ocupation)
        -> void {
        std::tie(m_data.available_moves, m_data.protected_squares) 
            = pimpl->do_pseudo_update(white_ocupation, black_ocupation, m_data.current);
    }
    auto get_moves() const noexcept
        -> BitBoard {
        return m_data.available_moves;
    }
    auto get_pos() const noexcept 
        -> Square {
        return m_data.current;
    }
    auto get_coverage() const noexcept
        -> BitBoard {
        return m_data.protected_squares;
    }
    auto is_white() const noexcept 
        -> bool {
        return m_data.is_white;
    }
    auto is_valid_move(Square to) const noexcept
        -> bool { return m_data.available_moves.test(to); }
    auto is_first_move() const noexcept
        -> bool { return m_data.is_first_move; }
    auto is_attacked(BitBoard enemy_attacking) 
        -> bool { return enemy_attacking.test(m_data.current); }
    auto type() const noexcept
        -> PieceType {
        return pimpl->do_type();
    }
    auto take_away_vcard() noexcept
        -> void { m_data.is_first_move = false; }
    auto invalidate_move(Square square) noexcept 
        -> void {
        m_data.available_moves.reset(square);
    }
    auto validate_move(Square square) noexcept 
        -> void {
        m_data.available_moves.set(square);
    }
    auto set_pos(Square square)
        -> void {
        m_data.current = square;
    }
private:
    PieceData m_data;
};

template<bool is_white_t>
class Knight {
public:
    auto pseudo_update(BitBoard, Square pos)
        -> BitBoard {
        auto const [m_row, m_col] = pos;
        BitBoard ret;
        auto set = [&](Square square) {
            if (not square.isOutOfBounds()) {
                ret.set(square);
            }
        };
        set({m_row + 1, m_col + 2});
        set({m_row + 2, m_col + 1});
        set({m_row - 1, m_col + 2});
        set({m_row - 2, m_col + 1});
        set({m_row + 1, m_col - 2});
        set({m_row + 2, m_col - 1});
        set({m_row - 1, m_col - 2});
        set({m_row - 2, m_col - 1});

        return ret;
    }
    constexpr auto type() const noexcept  
        -> PieceType { return PieceType::Knight; }
    static constexpr auto is_white() noexcept
        -> bool { return is_white_t; }

};
template<bool is_white_t>
class Bishop {
public:
    auto pseudo_update(BitBoard ocupied, Square pos)
        -> BitBoard {
        BitBoard ret;
        auto const [m_row, m_col] = pos;
        // I hate myself for doing this
        auto check_move = [&](Square square) -> bool {
            if (square.isOutOfBounds()) return false; 
            ret.set(square);
            if (ocupied.test(square)) {
                return false;
            }
            return true;
        };
        for (int i = 1; check_move({m_row + i, m_col + i}); ++i) {}
        for (int i = 1; check_move({m_row - i, m_col - i}); ++i) {}
        for (int i = 1; check_move({m_row - i, m_col + i}); ++i) {}
        for (int i = 1; check_move({m_row + i, m_col - i}); ++i) {}

        return ret;
    }
    constexpr auto type() const noexcept  
        -> PieceType { return PieceType::Bishop; }
    static constexpr auto is_white() noexcept
        -> bool { return is_white_t; }
};
template<bool is_white_t>
class Rook {
public:
    Rook() = default;
    auto pseudo_update(BitBoard ocupied, Square pos)
        -> BitBoard {
        BitBoard ret;
        auto const [m_row, m_col] = pos;
        // this code sucks but at least it is maintenable!
        auto check_move = [&](Square square) -> bool {
            if (square.isOutOfBounds()) { return false; }
            ret.set(square);
            if (ocupied.test(square)) {
                return false;
            }
            return true;
        };
        for (int i = 1; check_move({m_row + i, m_col}); ++i) {}
        for (int i = 1; check_move({m_row - i, m_col}); ++i) {}
        for (int i = 1; check_move({m_row, m_col + i}); ++i) {}
        for (int i = 1; check_move({m_row, m_col - i}); ++i) {}

        return ret;
    }
    constexpr auto type() const noexcept  
        -> PieceType { return PieceType::Rook; }
    static constexpr auto is_white() noexcept
        -> bool { return is_white_t; }
private:
};

template<bool is_white_t>
class Queen {
public:
    auto pseudo_update(BitBoard ocupied, Square pos)
        -> BitBoard {
        BitBoard ret;
        auto const [m_row, m_col] = pos;
        // god save the queen
        auto check_move = [&](Square square) -> bool {
            if (square.isOutOfBounds()) { return false; }
            ret.set(square);
            if (ocupied.test(square)) {
                return false;
            }
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
        
        return ret;
    }
    constexpr auto type() const noexcept  
        -> PieceType { return PieceType::Queen; }
    static constexpr auto is_white() noexcept
        -> bool { return is_white_t; }
private:
};

template<bool is_white_t>
class Pawn {
public:
    auto pseudo_update(BitBoard ocupied, Square pos) 
        -> BitBoard {
        BitBoard ret;
        auto const [m_row, m_col] = pos;

        auto set = [&](Square square) -> bool {
            if (not square.isOutOfBounds() and not ocupied.test(square)) {
                ret.set(square);
                return true;
            }
            return false;
        };
        if constexpr (is_white_t) {
            if (not Square{m_row + 1, m_col - 1}.isOutOfBounds() and ocupied.test({m_row + 1, m_col - 1})) 
                ret.set({m_row + 1, m_col - 1});
            if (ocupied.test({m_row + 1, m_col + 1})) ret.set({m_row + 1, m_col + 1});
            if (set({m_row + 1, m_col + 0}) and pos.row == 1) {
                set({m_row + 2, m_col + 0});
            }
        } else {
            if (not Square{m_row + 1, m_col - 1}.isOutOfBounds() and ocupied.test({m_row - 1, m_col - 1})) 
                ret.set({m_row - 1, m_col - 1});
            if (ocupied.test({m_row - 1, m_col + 1})) ret.set({m_row - 1, m_col + 1});
            if (set({m_row - 1, m_col + 0}) and pos.row == 6) {
                set({m_row - 2, m_col + 0});
            }
        }

        return ret;
    }
    constexpr auto type() const noexcept  //
        -> PieceType { return PieceType::Pawn; }
    static constexpr auto is_white() noexcept
        -> bool { return is_white_t; }
};

template<bool is_white_t>
class King {
public:
    auto pseudo_update(BitBoard, Square pos) 
        -> BitBoard {
        BitBoard ret;
        auto const [m_row, m_col] = pos;

        auto set = [&](Square square) {
            if (!square.isOutOfBounds()) {
                ret.set(square);
            }
        };
        
        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) { set({m_row + i, m_col + j}); }
        }
        ret.reset(pos);

        return ret;
    }
    constexpr auto type() const noexcept  //
        -> PieceType { return PieceType::King; }
    static constexpr auto is_white() noexcept
        -> bool { return is_white_t; }
};

#endif