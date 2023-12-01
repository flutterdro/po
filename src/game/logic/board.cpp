#include "board.h"
#include <ranges>
#include <algorithm>
#include <cassert>

namespace views = std::views;
namespace chess {
board::board()
    : m_turn_to_move{piece::color::white} {
    std::ranges::fill(m_board, std::nullopt);

    m_config.m_castling = static_cast<move::castling>(move::castling::black | move::castling::white);

    //first 2 elements must be kings
    add_piece<king<piece::color::black>>(to_square(7, 3));
    add_piece<king<piece::color::white>>(to_square(0, 3));
    add_piece<rook<piece::color::black>>(to_square(7, 0));
    add_piece<rook<piece::color::black>>(to_square(7, 7));
    add_piece<rook<piece::color::white>>(to_square(0, 0));
    add_piece<rook<piece::color::white>>(to_square(0, 7));


    add_piece<knight<piece::color::white>>(to_square(0, 1));
    add_piece<knight<piece::color::white>>(to_square(0, 6));
    add_piece<knight<piece::color::black>>(to_square(7, 1));
    add_piece<knight<piece::color::black>>(to_square(7, 6));
    add_piece<bishop<piece::color::white>>(to_square(0, 2));
    add_piece<bishop<piece::color::white>>(to_square(0, 5));
    add_piece<bishop<piece::color::black>>(to_square(7, 2));
    add_piece<bishop<piece::color::black>>(to_square(7, 5));
    add_piece<queen<piece::color::white>>(to_square(0, 4));
    add_piece<queen<piece::color::black>>(to_square(7, 4));
    for (size_t i = 0; i < 8; ++i) {
        add_piece<pawn<piece::color::white>>(to_square(1, i));
        add_piece<pawn<piece::color::black>>(to_square(6, i));
    }
    //soft_update();
}

auto board::operator[](square square) 
    -> std::optional<std::reference_wrapper<piece>> {
    if (m_board[square.pos].has_value()) {
        return m_piece_box[*m_board[square.pos]].value();
    }
    return {};
}

auto board::operator[](square square) const 
    -> std::optional<std::reference_wrapper<piece const>> {
    if (m_board[square.pos].has_value()) {
        return m_piece_box[*m_board[square.pos]].value();
    }
    return {};
}

auto board::at(square square) noexcept
    -> std::optional<piece>& {
        return m_piece_box[*m_board[square.pos]];
}

auto board::at(square square) const noexcept
    -> std::optional<piece> const& {
    return m_piece_box[*m_board[square.pos]];
}

auto board::undo_move(chess::move move) noexcept
    -> void {
    m_board[move.source().pos] = m_board[move.destination().pos];
    m_board[move.destination().pos] = std::nullopt;
    at(move.source())->set_pos(move.source());

    m_config.m_castling = static_cast<move::castling>(m_config.m_castling | move.castle_invalid());
    m_config.m_enpassant = move.enpassant_invalid();

    if (move.is_capture()) {
        if (move.move_type() == move::type::en_passant) {
            auto const is_white = m_turn_to_move == piece::color::white;
            auto const b = is_white - !is_white; // +1 or -1 depending on whose turn to move
            auto const square = to_square(move.destination().row() + b, move.destination().col());
            m_board[square.pos] = move.captured_piece_index();
            m_piece_box[move.captured_piece_index()] = create_piece(
                piece::type::pawn,
                m_turn_to_move,
                square
            );
        } else {
            m_board[move.destination().pos] = move.captured_piece_index();
            m_piece_box[move.captured_piece_index()] = create_piece(
                move.captured_piece_type(),
                m_turn_to_move,
                move.destination()
            );
        }
    }

    if (move.move_type() == move::type::promotion) {
        at(move.source()) = m_turn_to_move == piece::color::white ?
            piece{pawn<piece::color::black>{}, move.source()} :
            piece{pawn<piece::color::white>{}, move.source()};
    }

    if (move.move_type() == move::type::castle) {
        if (move.destination().col() == 1) {
            size_t const offset = std::to_underlying(flip_color(m_turn_to_move));
            square const rook_src{to_square(7 * (1 - offset), 0)};
            square const rook_dest{to_square(7 * (1 - offset), 2)};
            m_piece_box[2 + 2 * offset]->set_pos(rook_src);
            std::swap(m_board[rook_src.pos], m_board[rook_dest.pos]);
        } else {
            size_t const offset{std::to_underlying(flip_color(m_turn_to_move))};
            square const rook_src{to_square(7 * (1 - offset), 7)};
            square const rook_dest{to_square(7 * (1 - offset), 4)};
            m_piece_box[2 + 2 * offset + 1]->set_pos(rook_src);
            std::swap(m_board[rook_src.pos], m_board[rook_dest.pos]);
        }
    }

    m_turn_to_move = flip_color(m_turn_to_move);

}


auto board::do_move(chess::move const move) noexcept
    -> void {
    m_config.m_enpassant = 0;
    m_config.m_castling = static_cast<move::castling>(m_config.m_castling & ~move.castle_invalid());

    switch(move.move_type()) {
    using enum move::type;
    case castle:
        if (move.destination().col() == 1) {
            size_t const offset = std::to_underlying(m_turn_to_move);
            square const rook_src{to_square(7 * (1 - offset), 0)};
            square const rook_dest{to_square(7 * (1 - offset), 2)};
            m_piece_box[2 + 2 * offset]->set_pos(rook_dest);
            std::swap(m_board[rook_src.pos], m_board[rook_dest.pos]);
        } else {
            size_t const offset{std::to_underlying(m_turn_to_move)};
            square const rook_src{to_square(7 * (1 - offset), 7)};
            square const rook_dest{to_square(7 * (1 - offset), 4)};
            m_piece_box[2 + 2 * offset + 1]->set_pos(rook_dest);
            std::swap(m_board[rook_src.pos], m_board[rook_dest.pos]);
        }
        break;
    case null:
        switch (move.piece_tomove_type()) {
        case piece::type::pawn:
            if(std::abs(static_cast<int>(move.source().row()) - static_cast<int>(move.destination().row())) > 1) {
                m_config.validate_enpasant(move.source().col());
            }
            break;
        default:
            break;
        }

        break;
    case en_passant:
        if (m_turn_to_move == piece::color::white) {
            auto const captured_square{to_square(move.destination().row() - 1, move.destination().col())};
            m_board[captured_square.pos] = std::nullopt;
        } else {
            auto const captured_square{to_square(move.destination().row() + 1, move.destination().col())};
            m_board[captured_square.pos] = std::nullopt;
        }
        break;
    case promotion:
        at(move.source()) = create_piece(
            move.promoted_piece_type(),
            m_turn_to_move,
            move.source()
        );
        break;
    }

    if (move.is_capture()) {
        m_piece_box[move.captured_piece_index()] = std::nullopt;
    }

    m_board[move.destination().pos] = m_board[move.source().pos];
    m_board[move.source().pos] = std::nullopt;
    at(move.destination())->set_pos(move.destination());

    m_turn_to_move = flip_color(m_turn_to_move);
}

auto board::move(square from, square to)
    -> std::optional<square> {
    if (m_board[to.pos].has_value()) at(to) = std::nullopt;

    m_board[to.pos] = m_board[from.pos];
    at(to)->set_pos(to);
    m_board[from.pos] = std::nullopt;

    m_turn_to_move = flip_color(m_turn_to_move);
    return {};
}

auto board::turn_to_move() const noexcept
    -> piece::color {
    return m_turn_to_move;
}
// I am a conosieur of the principle assume perfect and valid data and validate it somewhere else
auto board::gen_move_info(square source, square destination) const noexcept
    -> chess::move {
    chess::move ret;

    auto invalidate_castle = [&config = this->m_config, &ret](piece::color color, move::castling castling) {
        if (config.castle(color, castling)) {
            ret.set_castle_invalid(color, castling);
        }
    };
    int const horizontal_offset = static_cast<int>(source.col()) - static_cast<int>(destination.col());
    ret.set_piece_tomove_type(at(source)->get_type());
    ret.set_destination(destination);
    ret.set_source(source);
    if (ret.piece_tomove_type() == piece::type::rook) {
        if (ret.source().col() == 0) invalidate_castle(m_turn_to_move, move::castling::kingside);
        if (ret.source().col() == 7) invalidate_castle(m_turn_to_move, move::castling::queenside);
    } else if (ret.piece_tomove_type() == piece::type::king) {
        invalidate_castle(m_turn_to_move, move::castling::kingside);
        invalidate_castle(m_turn_to_move, move::castling::queenside);
    }
    ret.set_move_type(move::type::null);
    if (m_board[destination.pos].has_value()) {
        ret.mark_capture();
        auto const captured_type = at(destination)->get_type();
        ret.set_captured_piece_type(captured_type);
        if (captured_type == piece::type::rook) {
            if (ret.destination().col() == 0) invalidate_castle(flip_color(m_turn_to_move), move::castling::kingside);
            if (ret.destination().col() == 7) invalidate_castle(flip_color(m_turn_to_move), move::castling::queenside);
        } else if (captured_type == piece::type::king) {
            invalidate_castle(flip_color(m_turn_to_move), move::castling::kingside);
            invalidate_castle(flip_color(m_turn_to_move), move::castling::queenside);
        }
        ret.set_captured_piece_index(*m_board[destination.pos]);
    } else if (ret.piece_tomove_type() == piece::type::pawn and std::abs(horizontal_offset) == 1) {
        ret.set_move_type(move::type::en_passant);
        ret.mark_capture();
        ret.set_captured_piece_type(piece::type::pawn);
        ret.set_captured_piece_index(*m_board[
            to_square(
                m_turn_to_move == piece::color::white ?
                    destination.row() - 1 :
                    destination.row() + 1,
                destination.col()
            ).pos]
        );
    }
    if (ret.piece_tomove_type() == piece::type::king and std::abs(horizontal_offset) > 1) {
        invalidate_castle(m_turn_to_move, move::castling::kingside);
        invalidate_castle(m_turn_to_move, move::castling::queenside);
        ret.set_move_type(move::type::castle);
    }
    if (ret.piece_tomove_type() == piece::type::pawn and (destination.row() == 7 or destination.row() == 0)) {
        ret.set_move_type(move::type::promotion);
    }

    return ret;
}

void board::soft_update() {
    bitboard white_pos;
    bitboard black_pos;
    for (auto const& piece : m_piece_box) {
        if (piece.has_value()) {
            if (piece->get_color() == piece::color::white) {
                white_pos.set(piece->get_pos());
            } else {
                black_pos.set(piece->get_pos());
            }
        }
    }

    for (auto& piece : m_piece_box) {
        if (piece.has_value()) {
            piece->pseudo_update(white_pos, black_pos);
        }
    }
    auto [white_coverage, black_coverage] = get_coverage();

    if (m_config.castle(move::castling::short_white) and
        not (static_cast<u64>(white_pos | black_pos | black_coverage) & 0b0110_u64)) {
        at({3})->validate_move({1});
    }
    if (m_config.castle(move::castling::long_white) and
        not (static_cast<u64>(white_pos | black_pos | black_coverage) & 0b01110000_u64)) {
        at({3})->validate_move({5});
    }
    if (m_config.castle(move::castling::short_black) and
        not (static_cast<u64>(white_pos | black_pos | white_coverage) & 0b0110_u64 << 8 * 7)) {
        at({59})->validate_move({57});
    }
    if (m_config.castle(move::castling::long_black) and
        not (static_cast<u64>(white_pos | black_pos | white_coverage) & 0b01110000_u64 << 8 * 7)) {
        at({59})->validate_move({61});
    }

    auto col = std::bit_width(m_config.m_enpassant) - 1;
    if (col >= 0) {
        bool const a = m_turn_to_move == piece::color::white;
        int const b = a - !a;
        unsigned const row = 3 + a;
        if (col != 0) {
            if (m_board[to_square(row, col - 1).pos].has_value() and
                at(to_square(row, col - 1))->get_type() == piece::type::pawn) {
                at(to_square(row, col - 1))->validate_move(to_square(row + b, col));
            }
        }
        if (col != 7) {
            if (m_board[to_square(row, col + 1).pos].has_value() and
                at(to_square(row, col + 1))->get_type() == piece::type::pawn) {
                at(to_square(row, col + 1))->validate_move(to_square(row + b, col));
            }
        }
    }

}

auto board::get_coverage() const noexcept
    -> std::pair<bitboard, bitboard> {
    bitboard white_coverage;
    bitboard black_coverage;
    auto has_value = []<typename T>(std::optional<T> const& v) { return v.has_value(); };

    for (auto const& piece :
         m_piece_box | views::filter(has_value)) {
        if (piece->get_color() == piece::color::white) {
            white_coverage |= piece->get_coverage();
        } else {
            black_coverage |= piece->get_coverage();
        }
    }
    return {white_coverage, black_coverage};
}

auto board::get_config() const noexcept 
    -> config { return m_config; }

auto board::hard_update() -> void {
    soft_update();
}

auto board::get_pieces() const noexcept
    -> std::array<std::optional<piece>, 32> const& {
    return m_piece_box;
}

auto board::evaluate()
    -> float {
    float evaluation = 0;
    for (auto const& piece : m_piece_box) {
            if (piece.has_value()) {
                auto is_white = piece->get_color() == piece::color::white;
                switch(piece->get_type()) {
                using enum piece::type;
                case king:
                    evaluation += is_white ? 200 : -200;
                    break;
                case pawn:
                    evaluation += is_white ? 1 : -1;
                    break;
                case knight:
                    evaluation += is_white ? 3 : -3;
                    break;
                case bishop:
                    evaluation += is_white ? 5 : -5;
                    break;
                case rook:
                    evaluation += is_white ? 5 : -5;
                    break;
                case queen:
                    evaluation += is_white ? 9 : -9;
                    break;
                }
            }
        }
    return m_turn_to_move == piece::color::white ? evaluation : -evaluation;
}

auto board::to_ibr() 
    -> IntermediateBoardRepresentation {
    IntermediateBoardRepresentation ret;
    auto has_value = [] <typename T> (std::optional<T> const& v) { return v.has_value(); };
    for (auto const& piece : m_piece_box | views::filter(has_value)) {
        ret[piece->get_pos()] = {static_cast<PieceType>(piece->get_type()), piece->get_color() == piece::color::white};
    }
    return ret;
}

auto board::swap(board& other) noexcept
    -> void {
    std::swap(m_board, other.m_board);
    std::swap(m_piece_box, other.m_piece_box);
    std::swap(m_config, other.m_config);
}

auto board::reset()
    -> void {
    board{}.swap(*this);
}

auto board::set_turn(piece::color color) noexcept
    -> void {
    m_turn_to_move = color;
}
}