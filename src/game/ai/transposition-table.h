#ifndef TRANSPOSITION_TABLE_H__
#define TRANSPOSITION_TABLE_H__

#include <fstream>
#include <unordered_map>

#include "../logic/board.h"

using u64 = uint64_t;

enum class NodeType {
	PV,
	All,
	Cut,
};

using zobrist_key = u64;

struct TTEntry {
	NodeType node_type{NodeType::PV};
	int depth{0};
	float evaluation{0.0f};
};

class TTable {
public:
	TTable() = default;
	[[nodiscard]]
	auto find_entry(zobrist_key) const noexcept
		-> std::optional<std::reference_wrapper<TTEntry const>>;
	auto find_eval(zobrist_key) const noexcept
		-> std::optional<float>;
	[[nodiscard]]
	auto find_entry_mut(zobrist_key) noexcept
		-> std::optional<std::reference_wrapper<TTEntry>>;

	auto add_entry(zobrist_key, TTEntry const&) 
		-> void;

	auto reserve(size_t) 
		-> void;

	[[nodiscard]]
	static auto gen_key(chess::board const&) noexcept
		-> zobrist_key;
private:
	std::unordered_map<zobrist_key, TTEntry> m_entries;
	mutable std::mutex m_mutex;
	static std::array<u64, 781> const rand_seq;
};

#endif