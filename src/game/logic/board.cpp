#include "board.h"

namespace views = std::views;

Board::Board() { 
    std::fill(m_board.begin(), m_board.end(), std::nullopt); 
    m_piece_box.reserve(48);

    //first 2 elements must be kings
    add_piece<King<0>>(Square{7, 3});
    add_piece<King<1>>(Square{0, 3});
    add_piece<Rook<0>>(Square{7, 0});
    add_piece<Rook<0>>(Square{7, 7});
    add_piece<Rook<1>>(Square{0, 0});
    add_piece<Rook<1>>(Square{0, 7});


    add_piece<Knight<1>>(Square{0, 1});
    add_piece<Knight<1>>(Square{0, 6});
    add_piece<Knight<0>>(Square{7, 1});
    add_piece<Knight<0>>(Square{7, 6});
    add_piece<Bishop<1>>(Square{0, 2});
    add_piece<Bishop<1>>(Square{0, 5});
    add_piece<Bishop<0>>(Square{7, 2});
    add_piece<Bishop<0>>(Square{7, 5});
    add_piece<Queen<1>>(Square{0, 4});
    add_piece<Queen<0>>(Square{7, 4});
    for (int i = 0; i < 8; ++i) {
        add_piece<Pawn<1>>(Square{1, i});
        add_piece<Pawn<0>>(Square{6, i});
    }
    //soft_update();
}

auto Board::operator[](Square square) 
    -> std::optional<std::reference_wrapper<Piece>> {
    if (m_board[square.to_index()].has_value()) {
        return m_piece_box[*m_board[square.to_index()]].value();
    }
    return {};
}

auto Board::operator[](Square square) const 
    -> std::optional<std::reference_wrapper<Piece const>> {
    if (m_board[square.to_index()].has_value()) {
        return m_piece_box[*m_board[square.to_index()]].value();
    }
    return {};
}

auto Board::at(Square square) noexcept
    -> std::optional<Piece>& {
        return m_piece_box[*m_board[square.to_index()]];
}

auto Board::at(Square square) const noexcept
    -> std::optional<Piece> const& {
    return m_piece_box[*m_board[square.to_index()]];
}

auto Board::move(Square from, Square to) 
    -> std::optional<Square> {

    if (m_board[to.to_index()].has_value()) at(to) = std::nullopt;

    m_board[to.to_index()] = m_board[from.to_index()];
    at(to)->set_pos(to);
    at(to)->take_away_vcard();
    if (*m_board[to.to_index()] == 0) {
        if (to == Square{7, 1} and m_config.tiny_castling_black) {
            m_piece_box[2]->set_pos({7, 2});
            std::swap(m_board[Square{7, 0}.to_index()], m_board[Square{7, 2}.to_index()]);
            m_config.tiny_castling_black = false;
        }
        if (to == Square{7, 5} and m_config.big_castling_black) {
            m_piece_box[3]->set_pos({7, 4});
            std::swap(m_board[Square{7, 7}.to_index()], m_board[Square{7, 4}.to_index()]);
            m_config.big_castling_black = false;
        }
    }
    if (*m_board[to.to_index()] == 1) {
        if (to == Square{0, 1} and m_config.tiny_castling_white) {
            m_piece_box[4]->set_pos({0, 2});
            std::swap(m_board[Square{0, 0}.to_index()], m_board[Square{0, 2}.to_index()]);
            m_config.big_castling_white = false;
        }
        if (to == Square{0, 5} and m_config.big_castling_white) {
            m_piece_box[5]->set_pos({0, 4});
            std::swap(m_board[Square{0, 7}.to_index()], m_board[Square{0, 4}.to_index()]);
            m_config.tiny_castling_white = false;
        }
    }
    m_board[from.to_index()] = std::nullopt;

    m_is_white_turn = !m_is_white_turn;
    return {};
}

auto Board::is_white_turn() const noexcept
    -> bool {
    return m_is_white_turn;
}

auto Board::promote(Square const square, PieceType const to) 
    -> void {
}

void Board::soft_update() {
    BitBoard white_pos;
    BitBoard black_pos;
    for (auto const& piece : m_piece_box) {
        if (piece.has_value()) {
            if (piece->is_white()) {
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
    auto& black_king = m_piece_box[0];
    auto& white_king = m_piece_box[1];
    auto const& brook_right = m_piece_box[2];
    auto const& brook_left  = m_piece_box[3];
    auto const& wrook_right = m_piece_box[4];
    auto const& wrook_left  = m_piece_box[5];

    auto [white_attacking, black_attacking] = get_coverage();

    if (black_king.has_value() and black_king->is_first_move()) {
        if (brook_right.has_value() and brook_right->is_first_move()) {
            bool is_blocked = false;
            for (int col = 1; col < 3; ++col) {
                is_blocked = is_blocked or m_board[Square{7, col}.to_index()].has_value();
            }
            is_blocked = is_blocked or black_king->is_attacked(white_attacking);
            if (not is_blocked) {
                black_king->validate_move({7, 1});
            }
        } else {
            m_config.tiny_castling_black = false;
        }
        if (brook_left.has_value() and brook_left->is_first_move()) {
            bool is_blocked = false;
            for (int col = 4; col < 7; ++col) {
                is_blocked = is_blocked or m_board[Square{7, col}.to_index()].has_value();
            }
            is_blocked = is_blocked or black_king->is_attacked(white_attacking);
            if (not is_blocked) {
                black_king->validate_move({7, 5});
            }
        } else {
            m_config.big_castling_black = false;
        }
    } else {
        m_config.tiny_castling_black = false;
        m_config.big_castling_black = false;
    }

    if (white_king.has_value() and white_king->is_first_move()) {
        if (wrook_right.has_value() and wrook_right->is_first_move()) {
            bool is_blocked = false;
            for (int col = 1; col < 3; ++col) {
                is_blocked = is_blocked or m_board[Square{0, col}.to_index()].has_value();
            }
            is_blocked = is_blocked or white_king->is_attacked(black_attacking);
            if (not is_blocked) {
                white_king->validate_move({0, 1});
            }
        } else {
            m_config.tiny_castling_white = false;
        }
        if (wrook_left.has_value() and wrook_left->is_first_move()) {
            bool is_blocked = false;
            for (int col = 4; col < 7; ++col) {
                is_blocked = is_blocked or m_board[Square{0, col}.to_index()].has_value();
            }
            is_blocked = is_blocked or white_king->is_attacked(black_attacking);
            if (not is_blocked) {
                white_king->validate_move({0, 5});
            }
        } else {
            m_config.big_castling_white = false;
        }
    } else {
        m_config.tiny_castling_white = false;
        m_config.big_castling_white = false;
    }
}

auto Board::get_coverage() const noexcept
    -> std::pair<BitBoard, BitBoard> {
    BitBoard white_coverage;
    BitBoard black_coverage;
    auto has_value = []<typename T>(std::optional<T> const& v) { return v.has_value(); };
    for (auto const& piece :
         m_piece_box | views::filter(has_value)) {
        if (piece->is_white()) {
            white_coverage |= piece->get_coverage();
        } else {
            black_coverage |= piece->get_coverage();
        }
    }
    return {white_coverage, black_coverage};
}

auto Board::get_config() const noexcept 
    -> BoardConfig { return m_config; }

auto Board::hard_update() -> void {
    soft_update();
}

auto Board::get_pieces() const noexcept
    -> std::vector<std::optional<Piece>> const& {
    return m_piece_box;
}

auto Board::evaluate()
    -> float {
    int evaluation = 0;
    for (auto const& piece : m_piece_box) {
            if (piece.has_value()) {
                bool is_white = piece->is_white();
                switch(piece->type()) {
                using enum PieceType;
                case King:
                    evaluation += is_white ? 200 : -200;
                    break;
                case Pawn:
                    evaluation += is_white ? 1 : -1;
                    break;
                case Knight:
                    evaluation += is_white ? 3 : -3;
                    break;
                case Bishop:
                    evaluation += is_white ? 5 : -5;
                    break;
                case Rook:
                    evaluation += is_white ? 5 : -5;
                    break;
                case Queen:
                    evaluation += is_white ? 9 : -9;
                    break;
                }
            }
        }
    return m_is_white_turn ? evaluation : -evaluation;
}

auto Board::to_ibr() 
    -> IntermediateBoardRepresentation {
    IntermediateBoardRepresentation ret;
    auto has_value = [] <typename T> (std::optional<T> const& v) { return v.has_value(); };
    for (auto const& piece : m_piece_box | views::filter(has_value)) {
        ret[piece->get_pos()] = {piece->type(), piece->is_white()};
    }
    return ret;
}

auto Board::swap(Board& other)
    -> void {
    std::swap(m_board, other.m_board);
    std::swap(m_piece_box, other.m_piece_box);
    std::swap(m_config, other.m_config);
}

auto Board::reset()
    -> void {
    Board{}.swap(*this);
}

auto Board::set_turn(bool turn) noexcept
    -> void {
    m_is_white_turn = turn;
}