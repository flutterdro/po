#include <catch2/catch_test_macros.hpp>
#include <tuple>

#include "../src/game/utilities/bitboard.h"

TEST_CASE("Bitboard std wrappers", "[bitboard]") {
	chess::bitboard bits{
		0b00100100010000111001100010000101111111
	};

	SECTION("pop_top()") {

		REQUIRE(bits.pop_top() == chess::to_square(4, 3));
		REQUIRE(bits == chess::bitboard{0b00'0001'0001'0000'1110'0110'0010'0001'0111'1111});

		REQUIRE(bits.pop_top() == chess::to_square(4, 0));
		REQUIRE(bits == chess::bitboard{0b00'0000'0001'0000'1110'0110'0010'0001'0111'1111});

		REQUIRE(bits.pop_top() == chess::to_square(3, 4));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'1110'0110'0010'0001'0111'1111});

		REQUIRE(bits.pop_top() == chess::to_square(2, 7));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0110'0110'0010'0001'0111'1111});
		REQUIRE(bits.pop_top() == chess::to_square(2, 6));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0010'0110'0010'0001'0111'1111});
		REQUIRE(bits.pop_top() == chess::to_square(2, 5));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0000'0110'0010'0001'0111'1111});
		REQUIRE(bits.pop_top() == chess::to_square(2, 2));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0000'0010'0010'0001'0111'1111});
		REQUIRE(bits.pop_top() == chess::to_square(2, 1));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0000'0000'0010'0001'0111'1111});
		REQUIRE(bits.pop_top() == chess::to_square(1, 5));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0000'0000'0000'0001'0111'1111});
		REQUIRE(bits.pop_top() == chess::to_square(1, 0));
		REQUIRE(bits == chess::bitboard{0b00'0000'0000'0000'0000'0000'0000'0000'0111'1111});
		bits.pop_top();bits.pop_top();bits.pop_top();
		int count{3};
		while (bits) {
			REQUIRE(bits.pop_top() == chess::to_square(0, count));
			--count;
		}
	}
}