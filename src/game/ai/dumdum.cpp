#include "dumdum.h"

auto alphaBetaMax(std::unique_ptr<Node>& head, int depth_left, bool is_white, 
                  int alpha, int beta) noexcept
    -> int {
    if (depth_left == 0) return head->board->evaluate();
    for (auto& kid : head->kids) {
        expand_tree(kid.get(), !is_white);
        int score = alphaBetaMin(kid, depth_left - 1, !is_white, alpha, beta);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

auto alphaBetaMin(std::unique_ptr<Node>& head, int depth_left, bool is_white, 
                  int alpha, int beta) noexcept
    -> int {
    if (depth_left == 0) return -head->board->evaluate();
    
    for (auto& kid : head->kids) {
        expand_tree(kid.get(), !is_white);

        int score = alphaBetaMax(kid, depth_left - 1, !is_white, alpha, beta);
        if (score <= alpha) return alpha;
        if (score < beta) beta = score;
        kid = nullptr;
    }
    return beta;
}


auto expand_tree(Node* head, bool is_white) 
   -> void {
    auto const& board = *head->board;
    auto& kids = head->kids; 

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (board[{row, col}] and is_white == board[{row, col}]->isWhite()) {
                for (int mrow = 0; mrow < 8; ++mrow) {
                    for (int mcol = 0; mcol < 8; ++mcol) {
                        if (board[{row,col}]->isLegalMove({mrow, mcol})) {
                            Board look_ahead{board};
                            look_ahead.move({row, col}, {mrow, mcol});
                            look_ahead.pseudoUpdate();
                            kids.push_back(
                                std::make_unique<Node>(std::move(look_ahead))
                            );
                        }
                    }
                }
            }
        }
    }
}


auto findBestMove(Board board) 
    -> std::pair<Square, Square> {
    fmt::print("{}\n", board);
    Node head(std::move(board));
    expand_tree(&head, false);
    int max_eval = std::numeric_limits<int>::max();
    Board best;
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    std::vector<std::thread> pool;
    std::vector<int> result;
    std::mutex mut;
    auto const start_whole{std::chrono::steady_clock::now()};
    for (auto& kid : head.kids) {
        auto const start_move{std::chrono::steady_clock::now()};
        expand_tree(kid.get(), true);
        // pool.emplace_back([&result, &mut](auto... a) {
        //     int score = alphaBetaMax(a...);
        //     std::lock_guard<std::mutex> lock{mut};
        //     result.push_back(score);
        // }, kid.get(), 4, false, alpha, beta);
        int res = alphaBetaMin(kid, 3, true, alpha, beta);
        if (max_eval > res) {
            max_eval = res;
            best = *kid->board;
        }
        auto const end_move{std::chrono::steady_clock::now()};
        fmt::print("{:>10%M:%S} ", end_move - start_move);
    }
    auto const end_whole{std::chrono::steady_clock::now()};
    fmt::print("\nwhole estimation took: {:>10%M:%S}\n", end_whole - start_whole);
    // for (auto& poo : pool) {
    //     poo.join();
    // }
    Square val[2];
    int i = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* next = best[{row, col}];
            Piece* prev = board[{row, col}];
            bool cond1 = next and prev and 
                (next->type() != prev->type() or next->isWhite() != prev->isWhite());
            bool cond2 = (next and !prev) or (!next and prev);
            if (cond1 or cond2) {
                val[i] = {row, col};
                ++i;
            }
        }
    }

    return best[val[0]] ? std::pair{val[1], val[0]} : std::pair{val[0], val[1]};
}