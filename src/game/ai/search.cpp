#include "search.h"




namespace rtw {

static std::unordered_map<zobrist_key, int> pol;

auto alpha_beta(std::unique_ptr<Node> const& root, TTable& table,
                float alpha, float beta, int depth) 
    -> float;
auto expand_move_tree(std::unique_ptr<Node> const& root)
    -> void;
auto reorder_moves(std::unique_ptr<Node> const& root, TTable const& table) noexcept
    -> void;

static size_t collisions = 0;
static size_t total = 0;
static size_t counter = 0;

auto search(Board const& board) -> Board {
    fmt::print("{0:-^{1}}\n", "Evaluation begin", 30);
    auto start = std::chrono::steady_clock::now();
    Board ret;

    std::unique_ptr<Node> const root{std::make_unique<Node>(board)};
    TTable transposition_table{};
    float alpha{-1000.0f};
    float beta{1000.0f};
    float max{alpha};

    transposition_table.reserve(1000*1000);

    expand_move_tree(root);
    size_t total_count = 0;
    int depth = 5;
    for (auto const& child_node : root->child_nodes) {
        child_node->board.soft_update();
        float score = -alpha_beta(child_node, transposition_table, -beta, -alpha, depth);
        if (score > max) {
            ret = child_node->board;
            max = score;
        }
        total_count += counter;
        counter = 0;
    }
    auto end = std::chrono::steady_clock::now();
    fmt::print("Evaluated {} nodes\n", total_count);
    fmt::print("Evaluation took {:%M:%S}\n", end - start);
    fmt::print("{0:-^{1}}\n", "Evaluation end", 30);
    return ret;
}

auto alpha_beta(std::unique_ptr<Node> const& root, TTable& table,
                float alpha, float beta, int depth)
    -> float {
    ++counter;
    if (depth <= 0) return root->board.evaluate();

    root->board.soft_update();
    expand_move_tree(root);
    reorder_moves(root, table);

    for (auto& child_node : root->child_nodes) {
        float score;
        zobrist_key const curr_pos = TTable::gen_key(child_node->board);
        if (auto entry_opt = table.find_entry(curr_pos);
            entry_opt.has_value()) {
            score = entry_opt->get().evaluation;
        } else {
            score = -alpha_beta(child_node, table, -beta, -alpha, depth - 1);
            table.add_entry(curr_pos, 
                TTEntry{
                    .depth = depth,
                    .evaluation = score
                }
            );
        }
        child_node = nullptr;
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

auto expand_move_tree(std::unique_ptr<Node> const& root)
    -> void {

    auto const& board{root->board};
    auto& child_nodes{root->child_nodes};
    auto has_value_and_matches_color = [is_white_turn = board.is_white_turn()]
    (std::optional<Piece> const& opt) { 
        return opt.has_value() && opt->is_white() == is_white_turn; 
    };

    for (auto const& piece : board.get_pieces() 
                                | std::views::filter(has_value_and_matches_color)) {

        BitBoard moves{piece->get_moves()};
        Square const curr_pos{piece->get_pos()};
        while (moves) {
            Board look_ahead{board};
            look_ahead.move(curr_pos, moves.pop_top());
            child_nodes.push_back(
                std::make_unique<Node>(
                    std::move(look_ahead)
                )
            );
        }

    }
}

auto reorder_moves(std::unique_ptr<Node> const& root, TTable const& table) noexcept
    -> void {

    auto& child_nodes = root->child_nodes;
    auto order = [&table](std::unique_ptr<Node> const& a, std::unique_ptr<Node> const& b) noexcept
        -> bool {
        auto const eval1_opt = table.find_entry(TTable::gen_key(a->board));
        float const eval1 = eval1_opt.has_value() ? 
            fabs(eval1_opt->get().evaluation) : 
            -10000.0f;
        
        auto const eval2_opt = table.find_entry(TTable::gen_key(b->board));
        float const eval2 = eval2_opt.has_value() ?
            fabs(eval2_opt->get().evaluation) :
            10000.0f;
        return eval1 > eval2;
    };

    std::sort(child_nodes.begin(), child_nodes.end(), order);
}


}
