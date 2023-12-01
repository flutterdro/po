#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <fstream>
#include <vector>

#include "../src/game/logic/piece.h"

TEST_CASE("Knight movegen", "[movegen][knight]") {

}

TEST_CASE("Bishop movegen", "[movegen][bishop]") {

}

TEST_CASE("Rook movegen", "[movegen][rook]") {
	std::vector<chess::bitboard> black_poss;
	std::vector<chess::bitboard> white_poss;
	std::vector<chess::square>   poss;
	if(std::fstream fin{"../../../tests/test-data/movegen/black-pos.data"}) {
		u64 buff;
		while (fin >> buff) {
			black_poss.push_back(chess::bitboard{buff});
		}
	} else {
		fmt::print("YOU ARE A FAILURE\n");
	}
	if(std::fstream fin{"../../../tests/test-data/movegen/white-pos.data"}) {
		u64 buff;
		while (fin >> buff) {
			white_poss.push_back(chess::bitboard{buff});
		}
	} else {
		fmt::print("YOU ARE A FAILURE\n");
	}
	if(std::fstream fin{"../../../tests/test-data/movegen/positions.data"}) {
		unsigned buff;
		while (fin >> buff) {
			poss.push_back(chess::square{buff});
		}
	} else {
		fmt::print("YOU ARE A FAILURE\n");
	}
	SECTION("Generating") {
		chess::rook<chess::piece::color::white> instance;
		size_t i = GENERATE(0, 1, 2, 3, 4);
		size_t j = GENERATE(0, 1, 2, 3, 4);
		size_t k = GENERATE(0, 1, 2, 3, 4, 5, 6, 7);
		auto a = instance.pseudo_update(white_poss[i] | black_poss[j], poss[k]);
		auto b = chess::rook<chess::piece::color::white>::naive_update(white_poss[i] | black_poss[j], poss[k]);
		fmt::print("{}\n\n{}\n--------\n", a, b);
		REQUIRE(a == b);
	}
}

TEST_CASE("Queen movegen", "[movegen][queen]") {
    
}

// 0000'0000'0000'0000'0100'0000'1011'1100'0100'0000'0100'0000'0000'0000'0000'0000
