#ifndef DBITSET_H__
#define DBITSET_H__

#include <cinttypes>
#include <bit>

#include <fmt/core.h>

struct Square {
    int row;
    int col;
    constexpr auto isOutOfBounds() const noexcept  //
        -> bool {
        if (row < 0 || row > 7 || col < 0 || col > 7) { return true; }
        return false;
    }
    constexpr auto operator==(Square const& other) const noexcept
        -> bool = default;
    constexpr explicit operator int() { return row * 8 + col; }
    constexpr auto to_index() const noexcept
        -> size_t { return row * 8 + col; }
};

using u64 = std::uint64_t;
 
inline consteval auto operator ""_u64(u64 lit) -> u64 { return lit; }

#if defined(_MSC_VER)
inline auto bswap (std::uint64_t v) noexcept { return _byteswap_uint64(v); }
inline auto bswap (std::uint32_t v) noexcept { return _byteswap_ulong(v); }
inline auto bswap (std::uint16_t v) noexcept { return _byteswap_ushort(v); }

constexpr auto const_bswap (std::uint64_t v) noexcept {
  return ((v & UINT64_C(0x0000'0000'0000'00FF)) << 56) |
         ((v & UINT64_C(0x0000'0000'0000'FF00)) << 40) |
         ((v & UINT64_C(0x0000'0000'00FF'0000)) << 24) |
         ((v & UINT64_C(0x0000'0000'FF00'0000)) <<  8) |
         ((v & UINT64_C(0x0000'00FF'0000'0000)) >>  8) |
         ((v & UINT64_C(0x0000'FF00'0000'0000)) >> 24) |
         ((v & UINT64_C(0x00FF'0000'0000'0000)) >> 40) |
         ((v & UINT64_C(0xFF00'0000'0000'0000)) >> 56);
}

constexpr auto const_bswap (std::uint32_t v) noexcept {
  return ((v & UINT32_C(0x0000'00FF)) << 24) |
         ((v & UINT32_C(0x0000'FF00)) <<  8) |
         ((v & UINT32_C(0x00FF'0000)) >>  8) |
         ((v & UINT32_C(0xFF00'0000)) >> 24);
}

constexpr auto const_bswap (std::uint16_t v) noexcept {
  return ((v & UINT16_C(0x00FF)) << 8) |
         ((v & UINT16_C(0xFF00)) >> 8);
}
#else
constexpr auto bswap (std::uint64_t v) noexcept { return __builtin_bswap64(v); }
constexpr auto bswap (std::uint32_t v) noexcept { return __builtin_bswap32(v); }
constexpr auto bswap (std::uint16_t v) noexcept { return __builtin_bswap16(v); }
#endif 
class BitBoard {
    friend struct fmt::formatter<BitBoard>;
public:
    constexpr BitBoard() : m_bits(0) {}
    constexpr BitBoard(u64 b) : m_bits(b) {}
    constexpr auto test(Square square) const noexcept //
        -> bool {
        return m_bits & (1_u64 << (square.row * 8 + square.col)) and not square.isOutOfBounds();
    }
    constexpr auto operator==(BitBoard const& other) const noexcept
        -> bool = default;
    constexpr auto operator|=(uint64_t rhs) noexcept //
        -> BitBoard { return {m_bits |= rhs}; }
    constexpr auto operator|=(BitBoard rhs) noexcept //
        -> BitBoard { return *this |= rhs.m_bits; }
    constexpr auto operator~() noexcept
        -> BitBoard { return {~m_bits}; }
    constexpr auto operator&=(uint64_t rhs) noexcept //
        -> BitBoard { return {m_bits &= rhs}; }
    constexpr auto operator&=(BitBoard rhs) noexcept //
        -> BitBoard { return *this &= rhs.m_bits; }
    constexpr operator bool() const noexcept { return !!m_bits; }
    [[nodiscard]]
    constexpr auto popcount() const noexcept
        -> int { return std::popcount(m_bits); }
    constexpr auto copy() const
        -> BitBoard { return *this; }
    constexpr auto pop_top() 
        -> Square {
        int top_bit = std::bit_width(m_bits) - 1;
        m_bits &= ~(1_u64 << top_bit);
        return {top_bit / 8, top_bit % 8};
    }
    constexpr auto set()  //
        -> BitBoard& {
        m_bits = 0xffffffffffffffff;
        return *this;    
    }
    constexpr auto set(Square square)  //
        -> BitBoard& {
        m_bits |= (1_u64 << (square.row * 8 + square.col));
        return *this;
    }
    constexpr auto reset()  //
        -> BitBoard& { 
        m_bits = 0;
        return *this;
    }
    constexpr auto reset(Square square)  //
        -> BitBoard& {
        m_bits &= ~(1_u64 << (square.row * 8 + square.col)); 
        return *this;
    }

    constexpr auto swap_bit_seq(int seq_begin1, int seq_begin2, int seq_len)
        -> BitBoard& {
        u64 mask = (1_u64 << seq_len) - 1;
        u64 x    = (m_bits >> seq_begin1) ^ (m_bits >> seq_begin2) & mask;
        m_bits  ^= (x << seq_begin1) ^ (x << seq_begin2);
        return *this;
    }

    constexpr auto flip()
        -> BitBoard& {
        bswap(m_bits);
        return *this;
    }

private:
    u64 m_bits;
};

constexpr auto operator|(BitBoard lhs, BitBoard rhs) -> BitBoard {
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
            arr[7 - 0], arr[7 - 1], arr[7 - 2], arr[7 - 3], arr[7 - 4], arr[7 - 5], arr[7 - 6], arr[7 - 7]);
    }
};
}  // namespace fmt

template<typename T>
bool isnt_nullptr(T* ptr) {
    return ptr != nullptr;
}

#endif