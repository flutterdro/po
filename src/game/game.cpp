#include "game.h"

namespace fs     = std::filesystem;
namespace views  = std::views;
namespace ranges = std::ranges;
auto loadResources() -> void {
    RM& rm = RM::getInstance();
    rm.loadShader("shaders/main.vs", "shaders/main.fs", "piece");
    rm.loadShader("shaders/main.vs", "shaders/board.fs", "board");

    const fs::path piece_sprites_dir{"assets/chess-sprites"};
    for (auto const& dir_entry : fs::directory_iterator{piece_sprites_dir}) {
        if (dir_entry.is_regular_file() &&
            dir_entry.path().filename().c_str()[0] != '.') {
            rm.loadTexture(dir_entry.path().c_str(), dir_entry.path().stem());
        }
    }
    rm.loadSprite(*rm.getShader("piece"), "piece");
    rm.loadSprite(*rm.getShader("board"), "board");
    unsigned char data[64];
    for (int i = 0; i < 64; ++i) {
        // those warnings JUST WON"T SHUTUP
        data[i] = static_cast<unsigned char>((i + i / 8) % 2 * 0xFF);
    }
    rm.loadTexture(data, "board");
}

auto Game::init() -> void {
    m_piece_ptr_owner.reserve(32);
    m_piece_ptr_owner.push_back(m_board[{0, 1}] = new Knight(0, {0, 1}));
    m_piece_ptr_owner.push_back(m_board[{0, 6}] = new Knight(0, {0, 6}));
    m_piece_ptr_owner.push_back(m_board[{7, 1}] = new Knight(1, {7, 1}));
    m_piece_ptr_owner.push_back(m_board[{7, 6}] = new Knight(1, {7, 6}));
    m_piece_ptr_owner.push_back(m_board[{0, 2}] = new Bishop(0, {0, 2}));
    m_piece_ptr_owner.push_back(m_board[{0, 5}] = new Bishop(0, {0, 5}));
    m_piece_ptr_owner.push_back(m_board[{7, 2}] = new Bishop(1, {7, 2}));
    m_piece_ptr_owner.push_back(m_board[{7, 5}] = new Bishop(1, {7, 5}));
    m_piece_ptr_owner.push_back(m_board[{0, 0}] = new Rook(0, {0, 0}));
    m_piece_ptr_owner.push_back(m_board[{0, 7}] = new Rook(0, {0, 7}));
    m_piece_ptr_owner.push_back(m_board[{7, 0}] = new Rook(1, {7, 0}));
    m_piece_ptr_owner.push_back(m_board[{7, 7}] = new Rook(1, {7, 7}));
    m_piece_ptr_owner.push_back(m_board[{0, 4}] = new Queen(0, {0, 4}));
    m_piece_ptr_owner.push_back(m_board[{7, 3}] = new Queen(1, {7, 3}));
    m_piece_ptr_owner.push_back(m_board[{0, 3}] = new King(0, {0, 3}));
    white_king_pos = {7, 4};
    m_piece_ptr_owner.push_back(m_board[{7, 4}] = new King(1, {7, 4}));
    black_king_pos = {0, 3};

    for (int i = 0; i < 8; ++i) {
        m_piece_ptr_owner.push_back(m_board[{1, i}] = new Pawn(0, {1, i}));
        m_piece_ptr_owner.push_back(m_board[{6, i}] = new Pawn(1, {6, i}));
    }

    RM& rm = RM::getInstance();
    m_board.pseudoUpdate();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f, -1.0f, 1.0f);
    m_board_sprite       = *rm.getSprite("board");
    m_board_texture      = *rm.getTexture("board");
    rm.getShader("piece")->set("projection", projection);
    rm.getShader("board")->set("projection", projection);
}

auto Game::draw() -> void {
    RM& rm              = RM::getInstance();
    auto is_not_nullptr = [](auto const ptr) { return ptr != nullptr; };
    auto [white_coverage, black_coverage] = m_board.getCoverage();

    if (auto shader = *rm.getShader("board"); is_piece_selected) {
        auto [s_row, s_col] = selected_square;
        BitBoard board      = m_board[selected_square]->getPseudoLegalMoves();
        int arr[64];
        for (int i = 0; i < 64; ++i) { arr[i] = board.test({i / 8, i % 8}); }
        shader.set("board", arr);
    } else {
        int arr[64];
        memset(arr, 0, sizeof(int) * 64);
        shader.set("board", arr);
    }
    m_board_sprite.draw(m_board_texture, {400.0f, 400.0f}, {400.0f, 400.0f});
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (m_board[{row, col}] != nullptr) {
                m_board[{row, col}]->draw(static_cast<float>(col * 100 + 50),
                                          static_cast<float>(row * 100 + 50));
            }
        }
    }
}
namespace {
auto getSquare(glm::vec2 pos) -> Square {
    return {static_cast<int>(pos.y) / 100, static_cast<int>(pos.x) / 100};
}
}  // namespace

auto Game::proccessInput() -> void {
    if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT)) { proccessClick(); }
}
auto Game::proccessClick() -> void {
    glm::vec2 const mouse_pos{Mouse::getX(), Mouse::getY()};
    Square const clicked_square{getSquare(mouse_pos)};
    fmt::print("clicked on {}\n", clicked_square);
    fmt::print("selected {}\n", selected_square);
    if (clicked_square.isOutOfBounds()) return;

    Piece const* const clicked_piece{m_board[clicked_square]};
    if (!is_piece_selected && clicked_piece) {
        fmt::print("Went into thinking mode\n");
        if (clicked_piece->isWhite() == is_white_turn) {
            selected_square   = clicked_square;
            is_piece_selected = true;
        }
        return;
    }
    if (Piece * selected_piece{m_board[selected_square]}; is_piece_selected) {
        fmt::print("Went into unthinking mode\n");
        if (selected_piece->isLegalMove(clicked_square)) {
            Board bup_board      = m_board;
            Square bup_wking_pos = white_king_pos;
            Square bup_bking_pos = black_king_pos;
            white_king_pos =
                selected_piece->type() == PieceType::King && is_white_turn ?
                    clicked_square :
                    white_king_pos;
            black_king_pos =
                selected_piece->type() == PieceType::King && !is_white_turn ?
                    clicked_square :
                    black_king_pos;
            m_board.move(selected_square, clicked_square);
            m_board.pseudoUpdate();
            // if (validateMove(m_board)) {
            //     is_white_turn = !is_white_turn;
            // } else {
            //     m_board        = bup_board;
            //     white_king_pos = bup_wking_pos;
            //     black_king_pos = bup_bking_pos;
            //     m_board.pseudoUpdate();
            // }
        }
        is_piece_selected = false;
    }
}
auto Game::validateMove(Board const& board) -> bool {
    auto is_not_nullptr = [](auto const ptr) { return ptr != nullptr; };
    auto const [white_coverage, black_coverage] = board.getCoverage();
    Square const attacking_king =
        is_white_turn ? white_king_pos : black_king_pos;
    BitBoard const coverage = is_white_turn ? black_coverage : white_coverage;
    return coverage.test(attacking_king);
}
auto Game::update() -> void {
    proccessInput();
    // if (is_in_check) { isCheckmate(); }
}

Game::~Game() {
    for (auto& piece : m_piece_ptr_owner) { delete piece; }
}
