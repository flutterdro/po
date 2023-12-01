#include "piece.h"
#include <vector>
#include <cassert>

namespace rngs = std::ranges;
namespace vws = std::views;
namespace chess {

auto piece::operator=(piece const& other)
    -> piece& {
    piece tmp(other);
    std::swap(pimpl, tmp.pimpl);
    std::swap(m_data, tmp.m_data);
    return *this;
}

// auto piece::pseudo_update(bitboard white_ocupation, bitboard black_ocupation)
//     -> void {
//     switch (m_type) {
//         case type::bishop: break;
//         case type::rook: break;
//         case type::queen: break;
//         case type::knight: break;
//         case type::pawn: break;
//         case type::king: break;
//         case type::none: break;
//     }
//
// }

auto create_piece(piece::type type, piece::color color, square pos)
    -> piece {
    switch (type) {
        case piece::type::pawn:
            return color == piece::color::white ?
                piece{pawn<piece::color::white>{}, pos} :
                piece{pawn<piece::color::black>{}, pos};
        case piece::type::bishop:
            return color == piece::color::white ?
                piece{bishop<piece::color::white>{}, pos} :
                piece{bishop<piece::color::black>{}, pos};
        case piece::type::rook:
            return color == piece::color::white ?
                piece{rook<piece::color::white>{}, pos} :
                piece{rook<piece::color::black>{}, pos};
        case piece::type::knight:
            return color == piece::color::white ?
                piece{knight<piece::color::white>{}, pos} :
                piece{knight<piece::color::black>{}, pos};
        case piece::type::queen:
            return color == piece::color::white ?
                piece{queen<piece::color::white>{}, pos} :
                piece{queen<piece::color::black>{}, pos};
        case piece::type::king:
            return color == piece::color::white ?
                piece{king<piece::color::white>{}, pos} :
                piece{king<piece::color::black>{}, pos};
        case piece::type::none: assert(false); std::unreachable();
    }
}



namespace{

constexpr auto rook_blockerboards_init() 
    -> std::array<bitboard, 64> {
    std::array<bitboard, 64> ret{};
    for (auto i : vws::iota(0_u64, 64_u64)) {
        ret[i] |= 0b01111110_u64  << (i & 0b111000);
        ret[i] |= 0x1010101010100_u64 << (i & 0b111);
        ret[i] &= ~(1_u64 << i);
    }

    return ret;
}
constexpr auto bishop_blockerboards_init() 
    -> std::array<bitboard, 64> {
    std::array<bitboard, 64> ret;
    for (auto i : vws::iota(0_u64, 64_u64)) {

        int difference =  (i >> 3) - (i & 7_u64);
        int sum = (i >> 3) + (i & 7_u64);

        for (auto j : vws::iota(0_u64, 64_u64)) {
            if ((j >> 3) - (j & 7) == difference) {
                ret[i] |= 1_u64 << j;
            }
        }

        for (auto j : vws::iota(0_u64, 64_u64)) {
            if ((j >> 3) + (j & 7) == sum) {
                ret[i] |= 1_u64 << j;
            }
        }
        ret[i] &= ~(0xFF818181818181FF_u64);
        ret[i] &= ~(1_u64 << i);
    }

    return ret;
}

constexpr auto rook_offsets_init() 
    -> std::array<size_t, 64> {
    std::array<size_t, 64> ret{};
    std::array<size_t, 64> const rook_offsets{
        1 << 12, 1 << 11, 1 << 11, 1 << 11, 1 << 11, 1 << 11, 1 << 11, 1 << 12,
        1 << 11, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 11,
        1 << 11, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 11,
        1 << 11, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 11,
        1 << 11, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 11,
        1 << 11, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 11,
        1 << 11, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 10, 1 << 11,
        1 << 12, 1 << 11, 1 << 11, 1 << 11, 1 << 11, 1 << 11, 1 << 11, 1 << 12
    };
    for (auto index : vws::iota(1uz, ret.size())) {
        ret[index] = 0;
        for (auto jndex : vws::iota(0u, index)) {
            ret[index] += rook_offsets[jndex];
        }
    }

    return ret;
}

constexpr auto bishop_offsets_init() 
    -> std::array<size_t, 64> {
    std::array<size_t, 64> ret{};
    std::array<size_t, 64> const bishop_offsets{
        1 << 6, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 6,
        1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5,
        1 << 5, 1 << 5, 1 << 7, 1 << 7, 1 << 7, 1 << 7, 1 << 5, 1 << 5,
        1 << 5, 1 << 5, 1 << 7, 1 << 9, 1 << 9, 1 << 7, 1 << 5, 1 << 5,
        1 << 5, 1 << 5, 1 << 7, 1 << 9, 1 << 9, 1 << 7, 1 << 5, 1 << 5,
        1 << 5, 1 << 5, 1 << 7, 1 << 7, 1 << 7, 1 << 7, 1 << 5, 1 << 5,
        1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5,
        1 << 6, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 5, 1 << 6,

    };
    for (auto index : vws::iota(1u, ret.size())) {
        ret[index] = 0;
        for (auto jndex : vws::iota(0u, index)) {
            ret[index] += bishop_offsets[jndex];
        }
    }

    return ret;
}


struct magic {
    bitboard blockerboard;
    bitboard magicboard;
    size_t offset;
    int shift;
};

constexpr auto rook_magics_init() noexcept 
    -> std::array<magic, 64> {
    std::array<magic, 64> ret; 
    
    auto const blockerboards{rook_blockerboards_init()};
    auto const offsets{rook_offsets_init()};
    //I yoinked it from someone smart
    std::array<bitboard, 64> const magicboards{
        bitboard{0x0080001020400080_u64}, bitboard{0x0040001000200040_u64}, bitboard{0x0080081000200080_u64}, bitboard{0x0080040800100080_u64},
	    bitboard{0x0080020400080080_u64}, bitboard{0x0080010200040080_u64}, bitboard{0x0080008001000200_u64}, bitboard{0x0080002040800100_u64},
	    bitboard{0x0000800020400080_u64}, bitboard{0x0000400020005000_u64}, bitboard{0x0000801000200080_u64}, bitboard{0x0000800800100080_u64},
	    bitboard{0x0000800400080080_u64}, bitboard{0x0000800200040080_u64}, bitboard{0x0000800100020080_u64}, bitboard{0x0000800040800100_u64},
	    bitboard{0x0000208000400080_u64}, bitboard{0x0000404000201000_u64}, bitboard{0x0000808010002000_u64}, bitboard{0x0000808008001000_u64},
	    bitboard{0x0000808004000800_u64}, bitboard{0x0000808002000400_u64}, bitboard{0x0000010100020004_u64}, bitboard{0x0000020000408104_u64},
	    bitboard{0x0000208080004000_u64}, bitboard{0x0000200040005000_u64}, bitboard{0x0000100080200080_u64}, bitboard{0x0000080080100080_u64},
	    bitboard{0x0000040080080080_u64}, bitboard{0x0000020080040080_u64}, bitboard{0x0000010080800200_u64}, bitboard{0x0000800080004100_u64},
	    bitboard{0x0000204000800080_u64}, bitboard{0x0000200040401000_u64}, bitboard{0x0000100080802000_u64}, bitboard{0x0000080080801000_u64},
	    bitboard{0x0000040080800800_u64}, bitboard{0x0000020080800400_u64}, bitboard{0x0000020001010004_u64}, bitboard{0x0000800040800100_u64},
	    bitboard{0x0000204000808000_u64}, bitboard{0x0000200040008080_u64}, bitboard{0x0000100020008080_u64}, bitboard{0x0000080010008080_u64},
	    bitboard{0x0000040008008080_u64}, bitboard{0x0000020004008080_u64}, bitboard{0x0000010002008080_u64}, bitboard{0x0000004081020004_u64},
	    bitboard{0x0000204000800080_u64}, bitboard{0x0000200040008080_u64}, bitboard{0x0000100020008080_u64}, bitboard{0x0000080010008080_u64},
	    bitboard{0x0000040008008080_u64}, bitboard{0x0000020004008080_u64}, bitboard{0x0000800100020080_u64}, bitboard{0x0000800041000080_u64},
	    bitboard{0x00FFFCDDFCED714A_u64}, bitboard{0x007FFCDDFCED714A_u64}, bitboard{0x003FFFCDFFD88096_u64}, bitboard{0x0000040810002101_u64},
	    bitboard{0x0001000204080011_u64}, bitboard{0x0001000204000801_u64}, bitboard{0x0001000082000401_u64}, bitboard{0x0001FFFAABFAD1A2_u64}
    };
    std::array<int, 64> const shifts {
        52, 53, 53, 53, 53, 53, 53, 52,
	    53, 54, 54, 54, 54, 54, 54, 53,
	    53, 54, 54, 54, 54, 54, 54, 53,
	    53, 54, 54, 54, 54, 54, 54, 53,
	    53, 54, 54, 54, 54, 54, 54, 53,
	    53, 54, 54, 54, 54, 54, 54, 53,
	    53, 54, 54, 54, 54, 54, 54, 53,
	    53, 54, 54, 53, 53, 53, 53, 53
    };

    for (auto i : vws::iota(0, 64)) {
        ret[i] = magic{
            .blockerboard = blockerboards[i],
            .magicboard = magicboards[i],
            .offset = offsets[i],
            .shift = shifts[i]
        };
    }

    return ret;
}

constexpr auto bishop_magics_init() 
    -> std::array<magic, 64> {
    std::array<magic, 64> ret;

    auto const blockerboards{bishop_blockerboards_init()};
    auto const offsets{bishop_offsets_init()};
    //again I yoinked it
    std::array<bitboard, 64> const magicboards{
        bitboard{0x0002020202020200_u64}, bitboard{0x0002020202020000_u64}, bitboard{0x0004010202000000_u64}, bitboard{0x0004040080000000_u64},
	    bitboard{0x0001104000000000_u64}, bitboard{0x0000821040000000_u64}, bitboard{0x0000410410400000_u64}, bitboard{0x0000104104104000_u64},
	    bitboard{0x0000040404040400_u64}, bitboard{0x0000020202020200_u64}, bitboard{0x0000040102020000_u64}, bitboard{0x0000040400800000_u64},
	    bitboard{0x0000011040000000_u64}, bitboard{0x0000008210400000_u64}, bitboard{0x0000004104104000_u64}, bitboard{0x0000002082082000_u64},
	    bitboard{0x0004000808080800_u64}, bitboard{0x0002000404040400_u64}, bitboard{0x0001000202020200_u64}, bitboard{0x0000800802004000_u64},
	    bitboard{0x0000800400A00000_u64}, bitboard{0x0000200100884000_u64}, bitboard{0x0000400082082000_u64}, bitboard{0x0000200041041000_u64},
	    bitboard{0x0002080010101000_u64}, bitboard{0x0001040008080800_u64}, bitboard{0x0000208004010400_u64}, bitboard{0x0000404004010200_u64},
	    bitboard{0x0000840000802000_u64}, bitboard{0x0000404002011000_u64}, bitboard{0x0000808001041000_u64}, bitboard{0x0000404000820800_u64},
	    bitboard{0x0001041000202000_u64}, bitboard{0x0000820800101000_u64}, bitboard{0x0000104400080800_u64}, bitboard{0x0000020080080080_u64},
	    bitboard{0x0000404040040100_u64}, bitboard{0x0000808100020100_u64}, bitboard{0x0001010100020800_u64}, bitboard{0x0000808080010400_u64},
	    bitboard{0x0000820820004000_u64}, bitboard{0x0000410410002000_u64}, bitboard{0x0000082088001000_u64}, bitboard{0x0000002011000800_u64},
	    bitboard{0x0000080100400400_u64}, bitboard{0x0001010101000200_u64}, bitboard{0x0002020202000400_u64}, bitboard{0x0001010101000200_u64},
	    bitboard{0x0000410410400000_u64}, bitboard{0x0000208208200000_u64}, bitboard{0x0000002084100000_u64}, bitboard{0x0000000020880000_u64},
	    bitboard{0x0000001002020000_u64}, bitboard{0x0000040408020000_u64}, bitboard{0x0004040404040000_u64}, bitboard{0x0002020202020000_u64},
	    bitboard{0x0000104104104000_u64}, bitboard{0x0000002082082000_u64}, bitboard{0x0000000020841000_u64}, bitboard{0x0000000000208800_u64},
	    bitboard{0x0000000010020200_u64}, bitboard{0x0000000404080200_u64}, bitboard{0x0000040404040400_u64}, bitboard{0x0002020202020200_u64}
    };
    std::array<int, 64> const shifts{
        58, 59, 59, 59, 59, 59, 59, 58,
	    59, 59, 59, 59, 59, 59, 59, 59,
	    59, 59, 57, 57, 57, 57, 59, 59,
	    59, 59, 57, 55, 55, 57, 59, 59,
	    59, 59, 57, 55, 55, 57, 59, 59,
	    59, 59, 57, 57, 57, 57, 59, 59,
	    59, 59, 59, 59, 59, 59, 59, 59,
	    58, 59, 59, 59, 59, 59, 59, 58
    };

    for (auto i : vws::iota(0, 64)) {
        ret[i] = magic{
            .blockerboard = blockerboards[i],
            .magicboard   = magicboards[i],
            .offset       = offsets[i],
            .shift        = shifts[i]
        };
    }

    return ret;
}

constexpr std::array<magic, 64> rook_magics   = rook_magics_init();
constexpr std::array<magic, 64> bishop_magics = bishop_magics_init();

auto rook_moves_lut_init()
    -> std::array<bitboard, 102400> {
    std::array<bitboard, 102400> ret{};
    for (auto i : vws::iota(0u, 64u)) {
        constexpr size_t capacity = 4096;
        std::array<bitboard, capacity> ref{};
        std::array<bitboard, capacity> res{};
        std::array<bitboard, capacity> ocup{};

        bitboard subset{0};
        size_t size{0};
        do {
            ocup[size] = subset;
            ref[size] = rook<piece::color::white>::naive_update(subset, square{i});
            ++size;
            subset = (subset + -rook_magics[i].blockerboard) & rook_magics[i].blockerboard;
        } while (subset);
        for (auto j : vws::iota(0ul, size)) {
            size_t index = static_cast<u64>(ocup[j] * rook_magics[i].magicboard) >> rook_magics[i].shift;
            res[index] = ref[j];
        }

        rngs::copy(res | vws::take(size), rngs::begin(ret | vws::drop(rook_magics[i].offset)));
    }

    return ret;
}
auto bishop_moves_lut_init()
    -> std::array<bitboard, 5248> {
    std::array<bitboard, 5248> ret{};
    for (auto i : vws::iota(0u, 64u)) {
        constexpr size_t capacity = 512;
        std::array<bitboard, capacity> ref{};
        std::array<bitboard, capacity> res{};
        std::array<bitboard, capacity> ocup{};

        bitboard subset{0};
        size_t size{0};
        do {
            ocup[size] = subset;
            ref[size] = bishop<piece::color::white>::naive_update(subset, square{i});
            ++size;
            subset = (subset + -bishop_magics[i].blockerboard) & bishop_magics[i].blockerboard;
        } while (subset);
        for (auto j : vws::iota(0ul, size)) {
            size_t index = static_cast<u64>(ocup[j] * bishop_magics[i].magicboard) >> bishop_magics[i].shift;
            res[index] = ref[j];
        }

        rngs::copy(res | vws::take(size), rngs::begin(ret | vws::drop(bishop_magics[i].offset)));
    }

    return ret;
}

std::array<bitboard, 102400> const rook_moves_lut = rook_moves_lut_init();
std::array<bitboard, 5248> const bishop_moves_lut = bishop_moves_lut_init();
}


auto get_rook_moves(bitboard ocupied, square pos)
    -> bitboard {
    return rook_moves_lut[(static_cast<u64>(
        ((rook_magics[pos.pos].blockerboard & ocupied) *
        rook_magics[pos.pos].magicboard)) >> rook_magics[pos.pos].shift) + 
        rook_magics[pos.pos].offset
    ];
}

auto get_bishop_moves(bitboard ocupied, square pos)
    -> bitboard {
    return bishop_moves_lut[(static_cast<u64>(
        ((bishop_magics[pos.pos].blockerboard & ocupied) *
        bishop_magics[pos.pos].magicboard)) >> bishop_magics[pos.pos].shift) + 
        bishop_magics[pos.pos].offset
    ];
}

void test() {
    // for (auto i : std::views::iota(0, 8)) {
    //     for (auto j : std::views::iota(0, 8)) {
    //         // fmt::print("{: >6} ", rook_offsets[i*8 + j]);
    //     }
    //     fmt::print("\n");
    // }
    // fmt::print("{}\n", rook_offsets[64]);
}
}
