#ifndef SEARCH_H__
#define SEARCH_H__

#include <thread>
#include <chrono>
#include <unordered_map>

#include <fmt/chrono.h>

#include "../logic/board.h"
#include "../utilities/zobrist-hash.h"
#include "../utilities/rectangle.h"
#include "transposition-table.h"

namespace rtw {

struct Node {
	Board board;
	std::vector<std::unique_ptr<Node>> child_nodes;

	Node(Board board) : board{std::move(board)} {}
};

[[nodiscard]]
auto search(Board const&)
	-> Board;
// auto quisce_search()
// 	-> void;
// auto reorder_moves(std::unique_ptr<Node> const&, TTable const&) noexcept
// 	-> void;
// auto expand_move_tree(std::unique_ptr<Node> const& root)
// 	-> void;
}
#endif