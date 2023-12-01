#include "search.h"
#include <future>
#include <ranges>

#define ASYNC
namespace rtw {

static std::unordered_map<zobrist_key, int> pol;

auto alpha_beta(
    chess::board& board,
    TTable&                      table,
    float                        alpha, 
    float                        beta, 
    int                          depth
) -> float;
auto expand_move_tree(chess::board const& board)
    -> std::vector<chess::move>;
auto reorder_moves(
    Node *const root, 
    TTable                const& table
) noexcept -> void;

static size_t counter = 0;

auto search(chess::board const& board) -> chess::board {
    fmt::print("{0:-^{1}}\n", "Evaluation begin", 30);
    auto start = std::chrono::steady_clock::now();
    chess::board ret;

    TTable transposition_table{};
    float alpha{-1000.0f};
    float beta{1000.0f};

    transposition_table.reserve(1000*1000);

    size_t total_count = 0;
    int depth = 6;
    for (int i = 6; i <= depth; ++i) {
        // std::unique_ptr<Node> const root{std::make_unique<Node>(board)};
        auto moves = expand_move_tree(board);
        std::vector<chess::board> copies{};
        for (auto const& move : moves) {
            chess::board copy{board};
            copy.do_move(move);
            copies.push_back(copy);
        }
        float max{alpha};
#ifdef ASYNC
        std::vector<std::future<float>> futures;
        for (auto& copy : copies) {
            futures.push_back(std::async(std::launch::async,
                [](auto&&... args) -> float {
                    return -alpha_beta(std::forward<decltype(args)>(args)...);
                },
                std::ref(copy),
                std::ref(transposition_table),
                std::ref(alpha),
                std::ref(beta),
                std::ref(i)
                )
            );
        }
        // for (auto const& copy : copies) {
        //     fmt::print("{}\n",copy.get_pieces().size());
        // }
        for (size_t j = 0; j < futures.size(); ++j) {
            if (float res = futures[j].get(); res > max) {
                ret = copies[j];
                max = res;
            }
        }
    }
#else
        for (auto const& child_node : root->child_nodes) {
            child_node->board.soft_update();
            float score = -alpha_beta(child_node, transposition_table, -beta, -alpha, i);
            if (score > max) {
                ret = child_node->board;
                max = score;
            }
            total_count += counter;
            counter = 0;
        }
    }
#endif
    auto end = std::chrono::steady_clock::now();
    fmt::print("Evaluated {} nodes\n", counter);
    fmt::print("Evaluation took {:%M:%S}\n", end - start);
    fmt::print("{0:-^{1}}\n", "Evaluation end", 30);
    return ret;
}

auto alpha_beta(
    chess::board& board,
    TTable& table,
    float alpha, 
    float beta, 
    int depth
) -> float {
    ++counter;
    if (depth <= 0) return board.evaluate();
    board.soft_update();
    auto const moves = expand_move_tree(board);
    // reorder_moves(root.get(), table);
    for (auto const move : moves) {
        float score;
        board.do_move(move);
        score = -alpha_beta(board, table, -beta, -alpha, depth - 1);
        board.undo_move(move);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    // for (auto& child_node : root->child_nodes) {
    //     float score;
    //     zobrist_key const curr_pos = TTable::gen_key(child_node->board);
    //     if (auto entry_opt = table.find_entry(curr_pos);
    //         entry_opt.has_value() && entry_opt->get().depth >= depth) {
    //         score = entry_opt->get().evaluation;
    //     } else {
    //         score = -alpha_beta(child_node, table, -beta, -alpha, depth - 1);
    //         table.add_entry(
    //             curr_pos,
    //             TTEntry{
    //                 .depth = depth,
    //                 .evaluation = score
    //             }
    //         );
    //     }
    //     child_node = nullptr;
    //     if (score >= beta) return beta;
    //     if (score > alpha) alpha = score;
    // }
    return alpha;
}

auto expand_move_tree(chess::board const& board)
    -> std::vector<chess::move> {
    auto has_value_and_matches_color = [turn_to_move = board.turn_to_move()]
    (std::optional<chess::piece> const& opt) { 
        return opt.has_value() and opt->get_color() == turn_to_move;
    };
    auto to_moves_for_piece = [&board](std::optional<chess::piece> const& piece) {
        std::vector<chess::move> ret;
        ret.reserve(50);
        chess::bitboard moves{piece->get_moves()};
        chess::square const curr_pos{piece->get_pos()};
        while (moves) {
            auto const move = board.gen_move_info(curr_pos, moves.pop_top());
            if (move.move_type() == chess::move::type::promotion) {
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::queen));
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::rook));
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::bishop));
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::knight));
                continue;
            }
            ret.push_back(move);
        }
        return ret;
    };


    // auto moves = board.get_pieces()
    //     |vws::filter(has_value_and_matches_color)
    //     |vws::transform(to_moves_for_piece)
    //     |vws::join
    //     |rngs::to<std::vector<chess::move>>;

    std::vector<chess::move> ret;
    ret.reserve(50);

    for (auto&& piece : board.get_pieces() |vws::filter(has_value_and_matches_color)) {
        chess::bitboard moves{piece->get_moves()};
        chess::square const curr_pos{piece->get_pos()};
        while (moves) {
            auto const move = board.gen_move_info(curr_pos, moves.pop_top());
            if (move.move_type() == chess::move::type::promotion) {
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::queen));
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::rook));
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::bishop));
                ret.push_back(move.clone().set_promoted_piece_type(chess::piece::type::knight));
                continue;
            }
            ret.push_back(move);
        }
    }


    return ret;
}

auto reorder_moves(Node* const root, TTable const& table) noexcept
    -> void {
    
    auto& child_nodes = root->child_nodes;
    std::array<float, 124> evals;
    
    size_t count{0};
    size_t size{0};
    for (auto const& child_node : child_nodes) {
        auto eval_opt = table.find_eval(TTable::gen_key(child_node->board));
        if (eval_opt.has_value()) {
            evals[size] = *eval_opt;
        } else {
            ++count;
            evals[size] = root->board.turn_to_move() == chess::piece::color::white ? 10'000 : -10'000;
        }
        ++size;
    }

    auto range = vws::zip(evals, child_nodes);
    auto it = rngs::begin(range) + count;
    auto pred = [](auto const& a, auto const& b){
        return a.first < b.first;
    };
    if (root->board.turn_to_move() == chess::piece::color::white) {
        rngs::partial_sort(range, it, pred);
        // rngs::sort(vws::zip(evals, child_nodes), [](auto const& a, auto const& b){
        //     return a.first < b.first;
        // });
    } else {
        rngs::partial_sort(range, rngs::begin(range) + count, [](auto const& a, auto const& b){
            return a.first > b.first;
        });
        // rngs::sort(vws::zip(evals, child_nodes), [](auto const& a, auto const& b){
        //     return a.first > b.first;
        // });
    }
    
    // if (child_nodes.empty()) fmt::print(stderr, "I apologize\n");
    // auto order_black = [&table](std::unique_ptr<Node> const& a, std::unique_ptr<Node> const& b) {
    //     auto const eval1_opt = table.find_entry(TTable::gen_key(a->board));
    //     float const eval1 = eval1_opt.has_value() ? 
    //         eval1_opt->get().evaluation : 
    //         -10000.0f;
    //     auto const eval2_opt = table.find_entry(TTable::gen_key(b->board));
    //     float const eval2 = eval2_opt.has_value() ?
    //         eval2_opt->get().evaluation :
    //         -10000.0f;

    //     return eval1 > eval2;
    // };
    // auto order_white = [&table](std::unique_ptr<Node> const& a, std::unique_ptr<Node> const& b) {
    //     auto const eval1_opt = table.find_entry(TTable::gen_key(a->board));
    //     float const eval1 = eval1_opt.has_value() ? 
    //         eval1_opt->get().evaluation : 
    //         10000.0f;
    //     auto const eval2_opt = table.find_entry(TTable::gen_key(b->board));
    //     float const eval2 = eval2_opt.has_value() ?
    //         eval2_opt->get().evaluation :
    //         10000.0f;
    //     return eval1 < eval2;
    // };

    // if (not root->board.is_white_turn()) std::ranges::sort(child_nodes, order_black);
    // else std::ranges::sort(child_nodes, order_white);
}


}
