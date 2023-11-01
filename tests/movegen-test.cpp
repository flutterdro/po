#include <catch2/catch_test_macros.hpp>

#include "../src/game/logic/piece.h"

TEST_CASE("Pseudo legal move generation", "[plmg]") {
	BitBoard const white_ocupying{10732882775568128};
	BitBoard const black_ocupying{17818793201004187648};

	SECTION("Knight") {
		Piece white_knight{Knight<true>{}, {2, 0}};
		Piece black_knight{Knight<false>{}, {2, 0}};
		white_knight.pseudo_update(white_ocupying, black_ocupying);
		black_knight.pseudo_update(white_ocupying, black_ocupying);
		REQUIRE(white_knight.get_moves() == BitBoard{8657043458});
		REQUIRE(black_knight.get_moves() == BitBoard{67109890});
	}
	SECTION("Bishop") {

		Piece white_bishop{Bishop<true>{}, {4, 3}};
		Piece black_bishop{Bishop<false>{}, {4, 3}};
		white_bishop.pseudo_update(white_ocupying, black_ocupying);
		black_bishop.pseudo_update(white_ocupying, black_ocupying);
		REQUIRE(white_bishop.get_moves() == BitBoard{21990570328064});
		REQUIRE(black_bishop.get_moves() == BitBoard{567348337721600});
	}

	SECTION("Rook") {
		Piece black_rook{Rook<false>{}, {4, 6}};
		black_rook.pseudo_update(white_ocupying, black_ocupying);
		REQUIRE(black_rook.get_moves() == BitBoard{71177275965440});
	}

}

// 0000'0000'0000'0000'0100'0000'1011'1100'0100'0000'0100'0000'0000'0000'0000'0000
