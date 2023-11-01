#ifndef ZOBRIST_HASH_H__
#define ZOBRIST_HASH_H__

#include <array>
#include <fstream>
#include <unordered_map>

#include "ibr.h"
#include "bitboard.h"

using u64 = std::uint64_t;

class Hasher {
private:
	Hasher();
public:
	static auto get_instance()
		-> Hasher&;
	auto get_piece_hash(bool, PieceType, Square) const
		-> u64;
	auto get_turn_hash(bool) const
		-> u64;
private:
	std::array<u64, 781> rand_seq;
};
class Board;
auto zhash(IntermediateBoardRepresentation board, bool is_white_turn, Hasher const& hasher)
	-> u64;
inline auto zhash(Board const&) -> u64 { return 0; }
namespace std {
template<>
struct hash<BoardState> {
	auto operator()(BoardState const& state) 
		-> size_t {
		return zhash(state.board, state.is_white_turn, m_hash);
	}
	auto operator()(BoardState const& state) const
		-> size_t {
		return zhash(state.board, state.is_white_turn, m_hash);
	}
	Hasher& m_hash{Hasher::get_instance()};
};
}
#endif