#ifndef I_B_R_H__
#define I_B_R_H__
/**
 * Intermidiate board representation
 * Implementation independent board representation
 * every board implementation must be convertable into imr
*/

#include "bitboard.h"
#include <ranges>

enum class PieceType {
    King = 0,
    Pawn,
    Knight,
    Bishop,
    Queen,
    Rook,
    None,
}; 

constexpr auto hash(PieceType type) 
	-> u64 {
	return static_cast<std::underlying_type_t<PieceType>>(type);
}

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
	constexpr auto operator[](Square square)
		-> IntermediatePieceData& { return ibr[square.row * 8 + square.col]; }
	[[nodiscard]]
	constexpr auto operator[](Square square) const
		-> IntermediatePieceData const& { return ibr[square.row * 8 + square.col]; }
	bool operator==(IntermediateBoardRepresentation const&) const = default;
	friend struct fmt::formatter<IntermediateBoardRepresentation>;
private:
	std::array<IntermediatePieceData, 64> ibr;
};

struct BoardConfig {
	std::array<bool, 8> valid_enpassant{};
	bool big_castling_black{true};
	bool big_castling_white{true};
	bool tiny_castling_black{true};
	bool tiny_castling_white{true};
};

struct BoardState {
	IntermediateBoardRepresentation board;
	bool is_white_turn;
	bool operator==(BoardState const&) const = default;
};
namespace vws = std::views;
namespace rngs = std::ranges;
namespace fmt {
template<>
struct formatter<IntermediateBoardRepresentation> {
	template<typename ParseContext> 
	constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	}
	template<typename FormatContext>
    auto format(BitBoard const& v, FormatContext& ctx) {
        std::string res;
		
		for (auto i : vws::iota(0, 7) | vws::reverse) {
			for (auto j : vws::iota(0, 7) | vws::reverse) {
				
			}
		}
        return format_to(ctx.out(), "{}", res);
    }
};
}

#endif