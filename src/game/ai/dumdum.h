#ifndef DUMDUM_H__
#define DUMDUM_H__

#include <thread>
#include <chrono>

#include <fmt/chrono.h>

#include "../logic/board.h"

struct Node {
    std::unique_ptr<Board> board;
    std::vector<std::unique_ptr<Node>> kids;

    Node(Board board_) 
        : board{std::make_unique<Board>(std::move(board_))}, kids{} {}
};

auto expand_tree(Node* head, bool is_white)
    ->void;

auto alphaBetaMax(std::unique_ptr<Node>& head, int depth_left, bool is_white, 
                  int alpha, int beta) noexcept
    -> int;
auto alphaBetaMin(std::unique_ptr<Node>& head, int depth_left, bool is_white, 
                  int alpha, int beta) noexcept
    -> int;

auto findBestMove(Board board) 
    -> std::pair<Square, Square>;

#endif