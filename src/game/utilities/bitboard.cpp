#include "bitboard.h"

BitBoard::BitBoard() : m_bits{0} {}

auto BitBoard::test(Square square) const -> bool {
    return m_bits & (static_cast<uint64_t>(1) << (square.row * 8 + square.col));
}

auto BitBoard::operator|=(uint64_t rhs) -> BitBoard {
    m_bits |= rhs;
    return *this;
}

auto BitBoard::operator|=(BitBoard rhs) -> BitBoard {
    return *this |= rhs.m_bits;
}

auto BitBoard::set() -> void { m_bits = 0xffffffffffffffff; }

auto BitBoard::set(Square square) -> void {
    m_bits |= (static_cast<uint64_t>(1) << (square.row * 8 + square.col));
}

auto BitBoard::reset(Square square) -> void {
    m_bits &= ~(static_cast<uint64_t>(1) << (square.row * 8 + square.col));
}

auto BitBoard::reset() -> void { m_bits = 0; }

auto Square::isOutOfBounds() const noexcept -> bool {
    if (row < 0 || row > 7 || col < 0 || col > 7) { return true; }
    return false;
}