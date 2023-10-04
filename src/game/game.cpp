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
    

    RM& rm = RM::getInstance();
    m_board.pseudoUpdate();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f, -1.0f, 1.0f);
    m_board_sprite       = *rm.getSprite("board");
    m_board_texture      = *rm.getTexture("board");
    rm.getShader("piece")->set("projection", projection);
    rm.getShader("board")->set("projection", projection);
}

auto Game::draw() -> void {
    RM& rm = RM::getInstance();

    if (auto shader = *rm.getShader("board"); is_piece_selected) {
        BitBoard board = m_board[selected_square]->getPseudoLegalMoves();
        int arr[64];
        for (int i = 0; i < 64; ++i) { arr[i] = board.test({i / 8, i % 8}); }
        shader.set("board", arr);
    } else {
        int arr[64];
        memset(arr, 0, sizeof(int) * 64);
        shader.set("board", arr);
    }
    m_board_sprite.draw(m_board_texture, {.tlc_pos = {0.0f, 0.0f},
                                          .size = {800.0f, 800.0f}});
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (m_board[{row, col}] != nullptr) {
                m_board[{row, col}]->draw(static_cast<float>(col * 100),
                                          static_cast<float>(row * 100));
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
        m_board.fullValidation();
        if (selected_piece->isLegalMove(clicked_square)) {
            m_board.move(selected_square, clicked_square);
            is_white_turn = !is_white_turn;
            m_board.fullValidation();
        }
        is_piece_selected = false;
    }
}
auto Game::validateMove(Board const& board) -> bool { 
    auto const [white_coverage, black_coverage] = board.getCoverage();
    Square const attacking_king =
        is_white_turn ? white_king_pos : black_king_pos;
    BitBoard const coverage = is_white_turn ? black_coverage : white_coverage;
    return coverage.test(attacking_king);
}
auto Game::update() -> void { 
    // if (not is_white_turn) {
    //     auto [from, to] = findBestMove(m_board);
    //     m_board.move(from, to);
    //     m_board.pseudoUpdate();
    //     is_white_turn = !is_white_turn;
    // } else {
        proccessInput(); 
    // }
}

Game::~Game() {
    for (auto& piece : m_piece_ptr_owner) { delete piece; }

}

