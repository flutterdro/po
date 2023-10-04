#ifndef DBITSET_H__
#define DBITSET_H__

#include <cinttypes>

#include <fmt/core.h>
struct Square {
    int row;
    int col;
    auto isOutOfBounds() const noexcept  //
        -> bool;
    constexpr explicit operator int() { return row * 8 + col; }
};

using u64 = std::uint64_t;
 
inline consteval auto operator ""_u64(u64 lit) -> u64 { return lit; }

class BitBoard {
public:
    BitBoard();
    BitBoard(u64);
    auto test(Square) const noexcept //
        -> bool;
    auto operator|=(uint64_t rhs) noexcept //
        -> BitBoard;
    auto operator|=(BitBoard rhs) noexcept //
        -> BitBoard;
    auto operator~() noexcept
        -> BitBoard;
    auto operator&=(uint64_t rhs) noexcept //
        -> BitBoard;
    auto operator&=(BitBoard rhs) noexcept //
        -> BitBoard;
    auto set()  //
        -> void;
    auto set(Square)  //
        -> void;
    auto reset()  //
        -> void;
    auto reset(Square)  //
        -> void;

    auto swapBitSeq(int seq_begin1, int seq_begin2, int seq_len)
        -> void;
    friend struct fmt::formatter<BitBoard>;
private:
    u64 m_bits;
};

inline auto operator|(BitBoard lhs, BitBoard rhs) -> BitBoard {
    return lhs |= rhs;
}

namespace fmt {
template<>
struct formatter<Square> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(Square const& v, FormatContext& ctx) {
        char const col = [&] {
            switch (v.col) {
            case 0:
                return 'a';
            case 1:
                return 'b';
            case 2:
                return 'c';
            case 3:
                return 'd';
            case 4:
                return 'e';
            case 5:
                return 'f';
            case 6:
                return 'h';
            case 7:
                return 'g';
            default:
                return 'o';
            }
        }();
        return format_to(ctx.out(), "{1:}{0:}", 7 - v.row + 1, col);
    }
};

template<>
struct formatter<BitBoard> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(BitBoard const& v, FormatContext& ctx) {
        uint8_t arr[8];
        u64* a = new (arr) u64{v.m_bits};
        return format_to(
            ctx.out(),
            "{:08b}\n{:08b}\n{:08b}\n{:08b}\n{:08b}\n{:08b}\n{:08b}\n{:08b}",
            arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]);
    }
};
}  // namespace fmt

template<typename T>
bool isnt_nullptr(T* ptr) {
    return ptr != nullptr;
}

#endif