#include "transposition-table.h"

auto rand_seq_init() 
	-> std::array<u64, 781> {
	
	std::array<u64, 781> ret;

	std::ifstream fin{"assets/zobrist-prn-seq.data"};
	for (auto& number : ret) {
		fin >> number;
	}

	return ret;
}

std::array<u64, 781> const TTable::rand_seq = rand_seq_init();

auto TTable::find_entry(zobrist_key key) const noexcept
	-> std::optional<std::reference_wrapper<const TTEntry>> {
	
	std::lock_guard lock(m_mutex);
	auto const it = m_entries.find(key);
	if (it == m_entries.end()) return {};

	return it->second;
}

auto TTable::find_eval(zobrist_key key) const noexcept 
	-> std::optional<float> {
	std::lock_guard lock(m_mutex);
	auto const it = m_entries.find(key);
	if (it == m_entries.end()) return {};

	return it->second.evaluation;
}


auto TTable::find_entry_mut(zobrist_key key) noexcept
	-> std::optional<std::reference_wrapper<TTEntry>> {
	
	std::lock_guard lock(m_mutex);
	auto it = m_entries.find(key);
	if (it == m_entries.end()) return {};

	return it->second;
}

auto TTable::add_entry(zobrist_key key, TTEntry const& entry) 
	-> void {
	std::lock_guard lock(m_mutex);
	auto [it, success] = m_entries.insert({key, entry});
	if (not success and it->second.depth < entry.depth) {
		it->second = entry;
	}
	
}

auto TTable::gen_key(chess::board const& board) noexcept
	-> zobrist_key {
	
	zobrist_key ret{0};

	auto has_value = [](auto const& opt) { return opt.has_value(); };
	auto piece_to_hash = [](chess::piece const& piece) 
		-> zobrist_key {
		return rand_seq[
			piece.get_pos().pos *
			(std::to_underlying(piece.get_type()) + std::to_underlying(piece.get_color()) * 6)
		];
	};
	// auto castling_rights_to_hash = [](chess::board::config const& config)
	// 	-> zobrist_key {
	// 	zobrist_key ret{0};
	// 	ret ^= config.big_castling_black  ?  rand_seq[772] : 0;
	// 	ret ^= config.big_castling_white  ?  rand_seq[769] : 0;
	// 	ret ^= config.tiny_castling_black ?  rand_seq[770] : 0;
	// 	ret ^= config.tiny_castling_white ?  rand_seq[771] : 0;
	//
	// 	return ret;
	// };
	auto turn_to_hash = [](chess::board const& board) 
		-> zobrist_key {
		return board.turn_to_move() == chess::piece::color::white ? 0 : rand_seq[768];
	};
	for (auto const& piece : board.get_pieces() 
								| std::views::filter(has_value)) {
		
		ret ^= piece_to_hash(*piece);
	}

	ret ^= turn_to_hash(board);
	// ret ^= castling_rights_to_hash(board.get_config());

	return ret;
}


auto TTable::reserve(size_t count) 
	-> void {
	m_entries.reserve(count);
}