#include "board.h"

namespace views = std::views;

Board::Board() { 
    m_rect = {.size = {800.0f, 800.0f}};
    std::fill(m_board.begin(), m_board.end(), nullptr); 
    (*this)[{0, 1}] = m_piece_box.alloc<Knight>(0, Square{0, 1});
    (*this)[{0, 6}] = m_piece_box.alloc<Knight>(0, Square{0, 6});
    (*this)[{7, 1}] = m_piece_box.alloc<Knight>(1, Square{7, 1});
    (*this)[{7, 6}] = m_piece_box.alloc<Knight>(1, Square{7, 6});
    (*this)[{0, 2}] = m_piece_box.alloc<Bishop>(0, Square{0, 2});
    (*this)[{0, 5}] = m_piece_box.alloc<Bishop>(0, Square{0, 5});
    (*this)[{7, 2}] = m_piece_box.alloc<Bishop>(1, Square{7, 2});
    (*this)[{7, 5}] = m_piece_box.alloc<Bishop>(1, Square{7, 5});
    (*this)[{0, 0}] = m_piece_box.alloc<Rook>(0, Square{0, 0});
    (*this)[{0, 7}] = m_piece_box.alloc<Rook>(0, Square{0, 7});
    (*this)[{7, 0}] = m_piece_box.alloc<Rook>(1, Square{7, 0});
    (*this)[{7, 7}] = m_piece_box.alloc<Rook>(1, Square{7, 7});
    (*this)[{0, 4}] = m_piece_box.alloc<Queen>(0, Square{0, 4});
    (*this)[{7, 3}] = m_piece_box.alloc<Queen>(1, Square{7, 3});
    (*this)[{0, 3}] = m_piece_box.alloc<King>(0, Square{0, 3});
    (*this)[{7, 4}] = m_piece_box.alloc<King>(1, Square{7, 4});
    for (int i = 0; i < 8; ++i) {
        (*this)[{1, i}] = m_piece_box.alloc<Pawn>(0, Square{1, i});
        (*this)[{6, i}] = m_piece_box.alloc<Pawn>(1, Square{6, i});
    }
    fullValidation();
}
Board::Board(Board const& other) 
    : m_piece_box(other.m_piece_box), m_rect(other.m_rect){

    /**
     * ptrdiff_t is hopelessly broken on 32 bit systems
     * so I pray that everybody in this day and age uses 64 bit
    */
    std::ptrdiff_t offset = m_piece_box.getBegin() - other.m_piece_box.getBegin();

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            (*this)[{row, col}] = nullptr;
            //I deeply regret my actions but there is no turning back
            if ((other[{row, col}])) {
                (*this)[{row, col}] = reinterpret_cast<Piece*>(
                    reinterpret_cast<std::byte*>(other[{row, col}]) + offset
                );
            }
        }
    }
}


auto Board::operator=(Board const& other)
    -> Board& {
    if (this != &other) {
        m_piece_box = other.m_piece_box;
        m_rect = other.m_rect;
        m_promotable = other.m_promotable;

        std::ptrdiff_t offset = m_piece_box.getBegin() - other.m_piece_box.getBegin();

        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                (*this)[{row, col}] = nullptr;
                //I deeply regret my actions but there is no turning back
                if ((other[{row, col}])) {
                    (*this)[{row, col}] = reinterpret_cast<Piece*>(
                        reinterpret_cast<std::byte*>(other[{row, col}]) + offset
                    );
                }
            }
        }
    }
    return *this;
}

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

auto Board::draw() -> void {
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if ((*this)[{row, col}] != nullptr) {
                (*this)[{row, col}]->draw(static_cast<float>(col * 100),
                                          static_cast<float>(row * 100));
            }
        }
    }
}

auto Board::promote(bool is_white, Square square, PieceType to) 
    -> void {
    switch(to) {
    case PieceType::Queen:
        (*this)[square] = m_piece_box.alloc<Queen>(is_white, square);
        break;
    case PieceType::Bishop:
        (*this)[square] = m_piece_box.alloc<Bishop>(is_white, square);
        break;
    case PieceType::Knight:
        (*this)[square] = m_piece_box.alloc<Knight>(is_white, square);
        break;
    case PieceType::Rook:
        (*this)[square] = m_piece_box.alloc<Rook>(is_white, square);
        break;
    default: 
        assert(false);
    }
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
        if ((*this)[{0, i}] and (*this)[{0, i}]->type() == PieceType::Pawn) {
            (*this)[{0, i}] = m_piece_box.alloc<Queen>(1, Square{0, i});
        }
        if ((*this)[{7, i}] and (*this)[{7, i}]->type() == PieceType::Pawn) {
            (*this)[{7, i}] = m_piece_box.alloc<Queen>(0, Square{7, i});
        }
    }
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

auto Board::fullValidation() -> void {
    pseudoUpdate();
    Square wking;
    Square bking;
    for (auto piece :
         m_board | views::filter([](auto ptr) { return ptr != nullptr; })) {
        if (piece->type() == PieceType::King) {
            if (piece->isWhite()) {
                wking = piece->getPos();
            } else {
                bking = piece->getPos();
            }
        }
    }
    for (auto piece :
         m_board | views::filter([](auto ptr) { return ptr != nullptr; })) {
        BitBoard pseudo_legal_moves{piece->getPseudoLegalMoves()};

        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (pseudo_legal_moves.test({row, col})) {
                    Board lookahead{*this};
                    Square bup_wking{wking};
                    Square bup_bking{bking};

                    lookahead.move(piece->getPos(), {row, col});
                    lookahead.pseudoUpdate();
                    if (piece->type() == PieceType::King) {
                        if (piece->isWhite()) {
                            wking = {row, col};
                        } else {
                            bking = {row, col};
                        }
                    }
                    auto [white_coverage, black_coverage] =
                        lookahead.getCoverage();
                    if ((piece->isWhite() && black_coverage.test(wking)) ||
                        (!piece->isWhite() && white_coverage.test(bking))) {
                        piece->invalidateMove({row, col});
                    }
                    wking = bup_wking;
                    bking = bup_bking;
                }
            }
        }
    }
}


auto Board::evaluate()
    -> int {
    bool kneel_before_in_the_presence_of_the_white_king = false;
    bool kneel_before_in_the_presence_of_the_black_king = false;
    int evaluation = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (Piece const* const ocupied = (*this)[{row, col}]; ocupied) {
                bool is_white = ocupied->isWhite();
                switch(ocupied->type()) {
                using enum PieceType;
                case King:
                    if (is_white) kneel_before_in_the_presence_of_the_white_king = true;
                    else kneel_before_in_the_presence_of_the_black_king = true;
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
    }

    if (not kneel_before_in_the_presence_of_the_white_king) {
        evaluation = std::numeric_limits<int>::min();
    }
    if (not kneel_before_in_the_presence_of_the_black_king) {
        evaluation = std::numeric_limits<int>::max();
    }
    return evaluation;
}