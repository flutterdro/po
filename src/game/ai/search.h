#ifndef SEARCH_H__
#define SEARCH_H__

#include <thread>
#include <chrono>
#include <unordered_map>

#include <fmt/chrono.h>

#include "../logic/board.h"
#include "../utilities/rectangle.h"
#include "transposition-table.h"

namespace rtw {

struct Node {
	chess::board board;
	std::vector<std::unique_ptr<Node>> child_nodes;

	// Node(chess::board board) : board{std::move(board)} {}
};

[[nodiscard]]
auto search(chess::board const&)
	-> chess::board;
}
#endif