#include "bitboard.h"

BitBoard::BitBoard() : m_bits{0} {}
BitBoard::BitBoard(u64 b) : m_bits{b} {}

auto BitBoard::test(Square square) const noexcept -> bool {
    return m_bits & (1_u64 << (square.row * 8 + square.col));
}

auto BitBoard::operator|=(uint64_t rhs) noexcept
    -> BitBoard {
    m_bits |= rhs;
    return *this;
}

auto BitBoard::operator|=(BitBoard rhs) noexcept
    -> BitBoard {
    return *this |= rhs.m_bits;
}

auto BitBoard::operator~() noexcept
    -> BitBoard {
    return {~m_bits};
}

auto BitBoard::operator&=(uint64_t rhs) noexcept //
    -> BitBoard { return {m_bits &= rhs}; }
auto BitBoard::operator&=(BitBoard rhs) noexcept //
    -> BitBoard { return *this &= rhs.m_bits; }

auto BitBoard::set() -> void { m_bits = 0xffffffffffffffff; }

auto BitBoard::set(Square square) -> void {
    m_bits |= (1_u64 << (square.row * 8 + square.col));
}

auto BitBoard::reset(Square square) -> void {
    m_bits &= ~(1_u64 << (square.row * 8 + square.col));
}

auto BitBoard::reset() -> void { m_bits = 0; }

auto Square::isOutOfBounds() const noexcept -> bool {
    if (row < 0 || row > 7 || col < 0 || col > 7) { return true; }
    return false;
}

auto BitBoard::swapBitSeq(int seq_begin1, int seq_begin2, int seq_len)
    -> void {
    u64 mask = (1_u64 << seq_len) - 1;
    u64 x    = (m_bits >> seq_begin1) ^ (m_bits >> seq_begin2) & mask;
    m_bits  ^= (x << seq_begin1) ^ (x << seq_begin2);
}