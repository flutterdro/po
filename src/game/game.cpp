#include "game.h"

void loadResources() {
    RM& rm = RM::getInstance();
    rm.loadShader("assets/shaders/main.vs", "assets/shaders/main.fs", "piece");
    rm.loadShader("assets/shaders/main.vs", "assets/shaders/board.fs", "board");
    rm.loadTexture("assets/chess-sprites/ready/pawn_b.png", "black-pawn");
    rm.loadTexture("assets/chess-sprites/ready/pawn_w.png", "white-pawn");
    unsigned char data[64];
    for (int i = 0; i < 64; ++i) {
        //those warnings JUST WON"T SHUTUP
        data[i] = static_cast<unsigned char>((i + i/8)%2 * 0xFF);
    }
    rm.loadTexture(data, "board");
}


Game::Game() 
    : m_board_sprite(*RM::getInstance().getShader("board"),
                     *RM::getInstance().getTexture("board")) {
    for (auto& row : m_board) {
        for (auto& cell : row) {
            cell = nullptr;
        }
    }
    m_board[0][1] = new Knight(0, 0, 1);
    m_board[0][6] = new Knight(0, 0, 6);
}

void Game::draw() {
    m_board_sprite.draw({400.0f, 400.0f}, {400.0f, 400.0f});
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
        if (m_board[thinking_row][thinking_col]->isLegalMove({row, col})) {
            fmt::print("Went into unthinking mode\n");
            m_board[thinking_row][thinking_col]->move(row, col, m_board);
            thinking = false;
            update_b(m_board);
        }
    }
}

void Game::init() {
    RM& rm = RM::getInstance();
    update_b(m_board);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f, -1.0f, 1.0f);
    rm.getShader("piece")->set("projection", projection);
    rm.getShader("board")->set("projection", projection);
}

Game::~Game() {
    for (auto& row : m_board) {
        for (auto& cell : row) {
            if (cell != nullptr) {
                delete cell;
            }
        }
    }
}