#include "game.h"

void loadResources() {
    RM& rm = RM::getInstance();
    rm.loadShader("shaders/main.vs", "shaders/main.fs", "piece");
    rm.loadShader("shaders/main.vs", "shaders/board.fs", "board");
    rm.loadTexture("assets/chess-sprites/ready/pawn_b.png", "black-pawn");
    rm.loadTexture("assets/chess-sprites/ready/pawn_w.png", "white-pawn");
    rm.loadSprite(*rm.getShader("piece"), "piece");
    rm.loadSprite(*rm.getShader("board"), "board");
    unsigned char data[64];
    for (int i = 0; i < 64; ++i) {
        //those warnings JUST WON"T SHUTUP
        data[i] = static_cast<unsigned char>((i + i/8)%2 * 0xFF);
    }
    rm.loadTexture(data, "board");
}

void Game::init() {
    for (auto& row : m_board) {
        for (auto& cell : row) {
            cell = nullptr;
        }
    }
    m_piece_ptr_owner.push_back(m_board[0][1] = new Knight(0, 0, 1));
    m_piece_ptr_owner.push_back(m_board[0][6] = new Knight(0, 0, 6));
    m_piece_ptr_owner.push_back(m_board[7][1] = new Knight(1, 7, 1));
    m_piece_ptr_owner.push_back(m_board[7][6] = new Knight(1, 7, 6));
    m_piece_ptr_owner.push_back(m_board[0][2] = new Bishop(0, 0, 2));
    m_piece_ptr_owner.push_back(m_board[0][5] = new Bishop(0, 0, 5));
    m_piece_ptr_owner.push_back(m_board[7][2] = new Bishop(1, 7, 2));
    m_piece_ptr_owner.push_back(m_board[7][5] = new Bishop(1, 7, 5));

    RM& rm = RM::getInstance();
    update_b(m_board);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f, -1.0f, 1.0f);
    m_board_sprite = *rm.getSprite("board");
    m_board_texture = *rm.getTexture("board");
    rm.getShader("piece")->set("projection", projection);
    rm.getShader("board")->set("projection", projection);

}

void Game::draw() {
    m_board_sprite.draw(m_board_texture, {400.0f, 400.0f}, {400.0f, 400.0f});
    for (size_t row = 0; row < m_board.size(); ++row) {
        for (size_t col = 0; col < m_board[0].size(); ++col) {
            if (m_board[row][col] != nullptr) {
                m_board[row][col]->draw(static_cast<float>(col * 100 + 50), static_cast<float>(row * 100 + 25));
            }
        }
    }
}
namespace{
std::pair<int, int> getSquare(glm::vec2 pos) {
    return {static_cast<int>(pos.y)/100, static_cast<int>(pos.x)/100};
}
}
void Game::update() {
    glm::vec2 mouse_pos{Mouse::getX(), Mouse::getY()};
    auto [row, col] = getSquare(mouse_pos);
    bool click = Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT);
    if (click && !thinking) {
        if (m_board[row][col] != nullptr) {
            fmt::print("Went into thinking mode\n");
            thinking = true;
            thinking_row = row;
            thinking_col = col;
        }
        click = false;
    }
    if (thinking && click) {
        fmt::print("Went into unthinking mode\n");
        m_board[thinking_row][thinking_col]->move(row, col, m_board);
        thinking = false;
        update_b(m_board);
    }
}

Game::~Game() {
    for (auto& piece : m_piece_ptr_owner) {
        delete piece;
    }
}