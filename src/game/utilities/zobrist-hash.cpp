#include "zobrist-hash.h"

auto Hasher::get_instance()
	-> Hasher& {
	static Hasher* instance = new Hasher;
	return *instance;
}

Hasher::Hasher() {
	std::ifstream fin{"assets/zobrist-prn-seq.data"};
	for (auto& number : rand_seq) {
		fin >> number;
	}
}

auto Hasher::get_piece_hash(bool is_white, PieceType type, Square square) const
	-> u64 {
	return rand_seq[(square.row * 8 + square.col) * (hash(type) + is_white * 6)];
}

auto Hasher::get_turn_hash(bool is_white_turn) const
	-> u64 {
	return is_white_turn ? 0 : rand_seq[768];
}

auto zhash(IntermediateBoardRepresentation board, bool is_white_turn, Hasher const& hasher)
	-> u64 {
	u64 ret{hasher.get_turn_hash(is_white_turn)};
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			Square const square{row, col};
			auto const [type, is_white]{board[square]};
			ret ^= hasher.get_piece_hash(is_white, type, square);
		}
	}
	return ret;
}