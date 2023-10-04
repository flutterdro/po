#ifndef PIECE_H__
#define PIECE_H__

#include <array>
#include <vector>
#include <tuple>
#include <string>
#include <filesystem>
#include <ranges>
#include <fmt/core.h>

#include "../utilities/bitboard.h"
#include "../engine/texture.h"
#include "../engine/shader.h"
#include "../engine/sprite.h"
#include "../engine/resource-manager.h"

enum class PieceType {
    King,
    Pawn,
    Knight,
    Bishop,
    Queen,
    Rook,
};  // namespace fmt
class Piece {
public:
    Piece(bool is_white, Square initial_pos, char const* texture_handle);
    /*
    draw a sprite
    */
    virtual auto clone() const //
        -> Piece* = 0;

    void draw(float, float);
    virtual void updatePseudoLegalMoves(BitBoard white_pos,
                                        BitBoard black_pos) = 0;
    [[nodiscard]] virtual auto type() const noexcept  //
        -> PieceType = 0;

    [[nodiscard]] auto isWhite() const noexcept  //
        -> bool;
    /**
     * @brief partially validates a move
     * @return true if move is pseudo legal
     */
    [[nodiscard]] auto isLegalMove(Square square) const noexcept  //
        -> bool;
    /**
     * @brief returns every pseudo legal move in a form of bitmap
     * @return uint64_t
     */
    [[nodiscard]] auto getPseudoLegalMoves() const noexcept  //
        -> BitBoard;
    [[nodiscard]] auto getCoverage() const noexcept  //
        -> BitBoard;
    /**
     * @brief returns a current position
     * @return a pair {row, column}
     */
    [[nodiscard]] auto getPos() const noexcept  //
        -> Square const;
    auto setPos(Square pos)  //
        -> void;
    auto invalidateMove(Square)  //
        -> void;

    virtual ~Piece() {}
protected:
    BitBoard m_p_legal_moves;
    BitBoard m_attacked_squares;
    Sprite m_sprite;
    Texture m_texture;
    Square m_current;
    bool m_is_white;
};

// using Board = std::array<std::array<Piece*, 8>, 8>;
// void update_b(Board&);
class Knight : public Piece {
public:
    Knight(bool is_white, Square initial_pos);
    auto clone() const 
        -> Piece* override final;
    void updatePseudoLegalMoves(BitBoard white_pos,
                                BitBoard black_pos) override final;
    [[nodiscard]] auto type() const noexcept  //
        -> PieceType override final;
};

class Bishop : public Piece {
public:
    Bishop(bool is_white, Square initial_pos);
    auto clone() const 
        -> Piece* override final;
    void updatePseudoLegalMoves(BitBoard white_pos,
                                BitBoard black_pos) override final;
    [[nodiscard]] auto type() const noexcept  //
        -> PieceType override final;
private:
};

class Rook : public Piece {
public:
    Rook(bool is_white, Square initial_pos);
    auto clone() const 
        -> Piece* override final;
    void updatePseudoLegalMoves(BitBoard white_pos,
                                BitBoard black_pos) override final;
    [[nodiscard]] auto type() const noexcept  //
        -> PieceType override final;
private:
};

class Queen : public Piece {
public:
    Queen(bool is_white, Square initial_pos);
    auto clone() const 
        -> Piece* override final;
    void updatePseudoLegalMoves(BitBoard white_pos,
                                BitBoard black_pos) override final;
    [[nodiscard]] auto type() const noexcept  //
        -> PieceType override final;
private:
};

class Pawn : public Piece {
public:
    Pawn(bool is_white, Square initial_pos);
    auto clone() const 
        -> Piece* override final;
    void updatePseudoLegalMoves(BitBoard white_pos,
                                BitBoard black_pos) override final;
    [[nodiscard]] auto type() const noexcept  //
        -> PieceType override final;
};

class King : public Piece {
public:
    King(bool is_white, Square initial_pos);
    auto clone() const 
        -> Piece* override final;
    void updatePseudoLegalMoves(BitBoard white_pos,
                                BitBoard black_pos) override final;
    [[nodiscard]] auto type() const noexcept  //
        -> PieceType override final;
};

#endif