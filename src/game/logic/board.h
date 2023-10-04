#ifndef BOARD_H__
#define BOARD_H__

#include <array>
#include <ranges>
#include <algorithm>
#include <exception>
#include <cstddef>
#include <concepts>

#include <glm/glm.hpp>

#include "../utilities/bitboard.h"
#include "piece.h"

//64 is a max amount of pieces but for a good measure double it
template<size_t max_piece_count = 128>
class PieceBox {
public:
    PieceBox() {
        size_t max_size = std::max({sizeof(Pawn), sizeof(Rook), sizeof(Knight),
                                    sizeof(Bishop), sizeof(King), sizeof(Queen)});
        begin = new std::byte[max_size * max_piece_count];
        size = max_size * max_piece_count;
        mem_it = begin;
    }
    PieceBox(PieceBox const& other) {
        size = other.size;
        begin = new std::byte[other.size];
        mem_it = begin + (other.mem_it - other.begin);
        memcpy(begin, other.begin, other.size);
    }
    PieceBox(PieceBox&& other) {
        begin  = other.begin;
        mem_it = other.mem_it;
        size   = other.size;

        other.begin  = nullptr;
        other.mem_it = nullptr;
        other.size   = 0;
    }
    PieceBox& operator=(PieceBox const& other) {
        if (this != &other) {
            delete[] begin;

            size = other.size;
            begin = new std::byte[other.size];
            mem_it = begin + (other.mem_it - other.begin);
            memcpy(begin, other.begin, other.size);
        }
        
        return *this;
    }
    PieceBox& operator=(PieceBox&& other) {
        if (this != &other) {
            delete[] begin;

            begin  = other.begin;
            mem_it = other.mem_it;
            size   = other.size;

            other.begin  = nullptr;
            other.mem_it = nullptr;
            other.size   = 0;
        }

        return *this;
    }
    ~PieceBox() {
        delete[] begin;
    }
    template<typename T, typename... Ts> 
    auto alloc(Ts&&... args)
        -> T* {
        static_assert(std::derived_from<T, Piece> == true);

        mem_it = align_for<T>(mem_it);
        //I will regret not throwing an exception here
        if (mem_it + sizeof(T) > begin + size) throw std::bad_alloc();

        T* ret = new(mem_it) T(std::forward<decltype(args)>(args)...);
        mem_it += sizeof(T);
        return ret;
    }
    template<typename T>
    constexpr auto align_for(std::byte* ptr) 
        -> std::byte* {
        return ptr + (alignof(T) - reinterpret_cast<uintptr_t>(ptr) % alignof(T)) % alignof(T);
    }
    auto getBegin() const
        -> std::byte* {
        return begin;
    }
private:
    std::byte* begin; //owner
    std::byte* mem_it;
    std::size_t size;
};

class Board {
public:
    Board();
    Board(Board const&);
    Board(Board&&) = default;
    auto operator=(Board const&)
        -> Board&;
    auto operator=(Board&&)
        -> Board& = default;
    auto operator[](Square)  
        -> Piece*&;
    auto operator[](Square) const  
        -> Piece* const&;
    auto getCoverage() const  
        -> std::pair<BitBoard, BitBoard>;
    auto move(Square from, Square to)  
        -> void;
    auto pseudoUpdate()  
        -> void;
    auto fullValidation()  
        -> void;
    auto draw()
        -> void;
    auto isPromotable() const noexcept
        -> bool;
    auto promote(bool, Square, PieceType)
        -> void;
    auto evaluate()
        -> int;
    friend struct fmt::formatter<Board>;
private:
    std::array<Piece*, 64> m_board;
    PieceBox<128> m_piece_box;
    Rectangle m_rect;
    bool m_promotable;
};

namespace fmt {
template<>
struct formatter<Board> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(Board const& v, FormatContext& ctx) {
        std::string ft;
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (!v[{row, col}]) {
                    ft += '0';
                } else {
                    ft += [](Piece* piece) {
                        switch(piece->type()) {
                        using enum PieceType;
                        case Pawn: return 'P';
                        case Rook: return 'R';
                        case King: return 'K';
                        case Queen: return 'Q';
                        case Bishop: return 'B';
                        case Knight: return 'H';
                        }
                    }(v[{row, col}]);
                }
            }
            ft += '\n';
        }
        return format_to(ctx.out(), "{}", ft.c_str());
    }
};
}


#endif