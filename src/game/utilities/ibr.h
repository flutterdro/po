#ifndef I_B_R_H__
#define I_B_R_H__
/**
 * Intermidiate board representation
 * Implementation independent board representation
 * every board implementation must be convertable into imr
*/

#include "bitboard.h"
#include <ranges>
#include <algorithm>

enum class PieceType {
    King = 0,
    Pawn,
    Knight,
    Bishop,
    Queen,
    Rook,
    None,
}; 

struct IntermediatePieceData{
    PieceType type;
    bool is_white;
	bool operator==(IntermediatePieceData const&) const = default;
};

class IntermediateBoardRepresentation {
public:
	constexpr IntermediateBoardRepresentation() {
		std::fill(ibr.begin(), ibr.end(), IntermediatePieceData{PieceType::None, 0});
	}
	[[nodiscard]]
	constexpr auto operator[](chess::square square)
		-> IntermediatePieceData& { return ibr[square.pos]; }
	[[nodiscard]]
	constexpr auto operator[](chess::square square) const
		-> IntermediatePieceData const& { return ibr[square.pos]; }
	bool operator==(IntermediateBoardRepresentation const&) const = default;
	friend struct fmt::formatter<IntermediateBoardRepresentation>;
private:
	std::array<IntermediatePieceData, 64> ibr;
};

struct BoardState {
	IntermediateBoardRepresentation board;
	bool is_white_turn;
	bool operator==(BoardState const&) const = default;
};
namespace vws = std::views;
namespace rngs = std::ranges;


#endif