#ifndef DBITSET_H__
#define DBITSET_H__

#include <cinttypes>

#include <fmt/core.h>
struct Square {
    int row;
    int col;
    auto isOutOfBounds() const noexcept  //
        -> bool;
};

class BitBoard {
public:
    BitBoard();
    auto test(Square) const  //
        -> bool;
    auto operator|=(uint64_t rhs)  //
        -> BitBoard;
    auto operator|=(BitBoard rhs)  //
        -> BitBoard;
    auto set()  //
        -> void;
    auto set(Square)  //
        -> void;
    auto reset()  //
        -> void;
    auto reset(Square)  //
        -> void;
    friend struct fmt::formatter<BitBoard>;
private:
    uint64_t m_bits;
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
        uint64_t* a = new (arr) uint64_t{v.m_bits};
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