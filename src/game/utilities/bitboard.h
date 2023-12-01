#ifndef DBITSET_H__
#define DBITSET_H__

#include <cinttypes>
#include <bit>
#include <ranges>

#include <fmt/core.h>

namespace vws = std::views;
namespace chess {
struct square {
    unsigned int pos;
    constexpr auto is_out_of_bounds() const noexcept
        -> bool {
        return pos >= 64;
    }
    constexpr auto operator==(square const& other) const noexcept
        -> bool = default;
    constexpr auto row() const noexcept 
        -> unsigned {
        return pos >> 3;
    }
    constexpr auto col() const noexcept
        -> unsigned {
        return pos & 7;
    }
};
[[nodiscard]] constexpr auto to_square(unsigned int row, unsigned int col) noexcept 
    -> chess::square { return {(row << 3) | col}; }
}


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

namespace chess {

enum class rank : u64 {
    first   = 0b1111'1111_u64 << 0 * 8,
    second  = 0b1111'1111_u64 << 1 * 8,
    third   = 0b1111'1111_u64 << 2 * 8,
    fourth  = 0b1111'1111_u64 << 3 * 8,
    fifth   = 0b1111'1111_u64 << 4 * 8,
    sixth   = 0b1111'1111_u64 << 5 * 8,
    seventh = 0b1111'1111_u64 << 6 * 8,
    eighth  = 0b1111'1111_u64 << 7 * 8,
};
enum class file : u64 {
    a = 72340172838076673_u64 << 7,
    b = 72340172838076673_u64 << 6,
    c = 72340172838076673_u64 << 5,
    d = 72340172838076673_u64 << 4,
    e = 72340172838076673_u64 << 3,
    f = 72340172838076673_u64 << 2,
    g = 72340172838076673_u64 << 1,
    h = 72340172838076673_u64 << 0,
};
class bitboard {
    friend struct fmt::formatter<bitboard>;
public:
    constexpr bitboard() : m_bits(0) {}
    constexpr bitboard(u64 b) : m_bits(b) {}
    constexpr auto test(square square) const noexcept //
        -> bool {
        return m_bits & (1_u64 << square.pos);
    }
    constexpr auto operator==(bitboard const& other) const noexcept
        -> bool = default;
    constexpr auto operator|=(uint64_t rhs) noexcept //
        -> bitboard { return {m_bits |= rhs}; }
    constexpr auto operator|=(bitboard rhs) noexcept //
        -> bitboard { return *this |= rhs.m_bits; }
    constexpr auto operator~() noexcept
        -> bitboard { return {~m_bits}; }
    constexpr auto operator&=(uint64_t rhs) noexcept //
        -> bitboard { return {m_bits &= rhs}; }
    constexpr auto operator&=(bitboard rhs) noexcept //
        -> bitboard { return *this &= rhs.m_bits; }
    constexpr auto operator-() const
        -> bitboard { return {(~m_bits) + 1}; }
    constexpr auto operator+(bitboard rhs) noexcept
        -> bitboard { return {m_bits + rhs.m_bits}; }
    constexpr auto operator*(bitboard rhs) noexcept
        -> bitboard { return {m_bits * rhs.m_bits}; }
    constexpr operator bool() const noexcept { return !!m_bits; }
    constexpr explicit operator u64() const noexcept { return m_bits; }
    [[nodiscard]]
    constexpr auto popcount() const noexcept
        -> int { return std::popcount(m_bits); }
    constexpr auto copy() const noexcept
        -> bitboard { return *this; }
    constexpr auto pop_top() noexcept
        -> square {
        auto const top_bit = static_cast<unsigned>(std::bit_width(m_bits) - 1);
        m_bits &= ~(1_u64 << top_bit);
        return {top_bit};
    }
    constexpr auto set() noexcept
        -> bitboard& {
        m_bits = 0xffffffffffffffff;
        return *this;    
    }
    constexpr auto set(square square)  noexcept
        -> bitboard& {
        m_bits |= (1_u64 << square.pos);
        return *this;
    }
    constexpr auto reset()  noexcept
        -> bitboard& { 
        m_bits = 0;
        return *this;
    }
    constexpr auto reset(square square) noexcept
        -> bitboard& {
        m_bits &= ~(1_u64 << square.pos); 
        return *this;
    }

    constexpr auto swap_bit_seq(int seq_begin1, int seq_begin2, int seq_len) noexcept
        -> bitboard& {
        u64 mask = (1_u64 << seq_len) - 1;
        u64 x    = (m_bits >> seq_begin1) ^ (m_bits >> seq_begin2) & mask;
        m_bits  ^= (x << seq_begin1) ^ (x << seq_begin2);
        return *this;
    }

    constexpr auto flip() noexcept
        -> bitboard& {
        bswap(m_bits);
        return *this;
    }

private:
    u64 m_bits;
};

constexpr auto operator|(bitboard lhs, bitboard rhs) -> bitboard {
    return lhs |= rhs;
}

constexpr auto operator&(bitboard lhs, bitboard rhs) -> bitboard {
    return lhs &= rhs;
}
}

namespace fmt {
template<>
struct formatter<chess::square> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(chess::square const& v, FormatContext& ctx) {
        char const col = [&] {
            switch (v.col()) {
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
        return format_to(ctx.out(), "{1:}{0:}", 7 - v.row() + 1, col);
    }
};

template<>
struct formatter<chess::bitboard> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }
    template<typename FormatContext>
    auto format(chess::bitboard const& v, FormatContext& ctx) {
        uint8_t arr[8];
        [[maybe_unused]]u64* a = new (arr) u64{v.m_bits};
        std::string frmt;
        for (auto row : vws::iota(0, 8) | vws::reverse) {
            for (auto col : vws::iota(0, 8) | vws::reverse) {
                int bit = (arr[row] >> col) & 1;
                frmt += bit ? '1' : '.';
            }
            frmt += '\n';
        }
        frmt.pop_back();
        return format_to(
            ctx.out(), "{}", frmt);
    }
};
}  // namespace fmt

#endif