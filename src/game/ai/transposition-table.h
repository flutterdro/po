#ifndef TRANSPOSITION_TABLE_H__
#define TRANSPOSITION_TABLE_H__

#include <unordered_set>
#include <fstream>

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

// namespace std {
// template<>
// struct hash<TTEntry> {
// 	auto operator()(TTEntry const& key) const noexcept {
// 		return std::hash<u64>{}(key.zobrist_key);
// 	}
// };	
// }

class TTable {
public:
	TTable() = default;
	[[nodiscard]]
	auto find_entry(zobrist_key) const noexcept
		-> std::optional<std::reference_wrapper<const TTEntry>>;
	[[nodiscard]]
	auto find_entry_mut(zobrist_key) noexcept
		-> std::optional<std::reference_wrapper<TTEntry>>;

	auto add_entry(zobrist_key, TTEntry const&) 
		-> void;

	auto reserve(size_t) 
		-> void;

	[[nodiscard]]
	static auto gen_key(Board const&) noexcept
		-> zobrist_key;
private:
	std::unordered_map<zobrist_key, TTEntry> m_entries;

	static std::array<u64, 781> const rand_seq;
};

#endif