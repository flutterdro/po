#include "game.h"

namespace fs     = std::filesystem;
namespace views  = std::views;
namespace ranges = std::ranges;
auto loadResources() -> void {
    ChessWidget::load_resources();
}

Game::Game(Window& window)
    : m_window{window}, m_gui{m_board}, eval_bar{evaluation} {
    glViewport(0, 0, window.get_width(), window.get_height());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_board.soft_update();
    ResourceManager& rm = ResourceManager::get_instance();
    glm::mat4 projection = glm::ortho(0.0f, (float)m_window.get_wwidth(), (float)m_window.get_wheight(), 0.0f, -1.0f, 1.0f);
    rm.request_shader("pieces").set("projection", projection);
    rm.request_shader("basic-rectangle").set("projection", projection);
    m_gui
        .get_sprite()
        .get_shader()
        .set("projection", projection);
}

auto Game::draw() -> void {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_gui.draw();
    eval_bar.draw();
}
namespace {

}  // namespace

auto Game::proccessInput() -> void {
    if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT)) { proccessClick(); }
    if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_RIGHT)) { m_board.reset(); }
}
auto Game::proccessClick() -> void {
    m_gui.on_click_action();
    m_board.hard_update();
}
auto Game::update() -> void { 
    proccessInput(); 
    if (not m_board.is_white_turn()) {
        Board board = rtw::search(m_board);
        m_board.swap(board);
        m_board.set_turn(true);
    }
    evaluation = m_board.evaluate();
    eval_bar.update();
}

auto ChessWidget::load_resources()
    -> void {
    static char dummy = [](){
        ResourceManager& rm = ResourceManager::get_instance();

        unsigned char data[64];
        for (int i = 0; i < 64; ++i) {
            // those warnings JUST WON"T SHUTUP
            data[i] = static_cast<unsigned char>((i + i / 8) % 2 * 0xFF);
        }
        rm.loadTexture(data, "board");
        rm.emplaceSprite("board", rm.request_shader("board"), rm.request_texture("board"));

        const fs::path piece_sprites_dir{"assets/chess-sprites"};
        for (auto const& dir_entry : fs::directory_iterator{piece_sprites_dir}) {
            if (dir_entry.is_regular_file() &&
                dir_entry.path().filename().c_str()[0] != '.') {
                Texture tex = rm.load_texture_image(dir_entry.path().c_str(), dir_entry.path().stem());
                rm.emplaceSprite(dir_entry.path().stem(), rm.request_shader("pieces"), tex);
            }
        }

        return 1;
    }();
}

ChessWidget::ChessWidget(Board& board)
    : m_board{board}{
    ResourceManager& rm = ResourceManager::get_instance();

    m_sprite = rm.request_sprite("board");
    rect.size = {600.0f, 600.0f};
    
    piece_s[1][static_cast<size_t>(PieceType::Pawn)]   = rm.request_sprite("white-pawn");
    piece_s[1][static_cast<size_t>(PieceType::King)]   = rm.request_sprite("white-king");
    piece_s[1][static_cast<size_t>(PieceType::Queen)]  = rm.request_sprite("white-queen");
    piece_s[1][static_cast<size_t>(PieceType::Rook)]   = rm.request_sprite("white-rook");
    piece_s[1][static_cast<size_t>(PieceType::Knight)] = rm.request_sprite("white-knight");
    piece_s[1][static_cast<size_t>(PieceType::Bishop)] = rm.request_sprite("white-bishop");
    piece_s[0][static_cast<size_t>(PieceType::Pawn)]   = rm.request_sprite("black-pawn");
    piece_s[0][static_cast<size_t>(PieceType::King)]   = rm.request_sprite("black-king");
    piece_s[0][static_cast<size_t>(PieceType::Queen)]  = rm.request_sprite("black-queen");
    piece_s[0][static_cast<size_t>(PieceType::Rook)]   = rm.request_sprite("black-rook");
    piece_s[0][static_cast<size_t>(PieceType::Knight)] = rm.request_sprite("black-knight");
    piece_s[0][static_cast<size_t>(PieceType::Bishop)] = rm.request_sprite("black-bishop");
}

auto ChessWidget::get_sprite()
    -> Sprite& { return m_sprite; }
auto ChessWidget::draw() 
    -> void {
    i_board = m_board.to_ibr();

    if (m_selected) {
        BitBoard board = m_board[*m_selected]->get().get_moves();
        int arr[64];
        for (int i = 0; i < 64; ++i) { arr[i] = board.test({7 - i / 8, 7 - i % 8}); }
        m_sprite.get_shader().set("board", arr);
    } else {
        int arr[64];
        memset(arr, 0, sizeof(int) * 64);
        m_sprite.get_shader().set("board", arr);
    } 

    m_sprite.draw(rect);
    glm::vec2 const square_size = rect.size * (1.0f / 8.0f);
    glm::vec2 const square_offset = rect.tlc_pos;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            IntermediatePieceData const piece = i_board[{row, col}];
            if (piece.type != PieceType::None) {
                piece_s[piece.is_white][static_cast<size_t>(piece.type)]
                    .get_shader()
                    .set("white", piece.is_white);
                ItemFrame piece_rect{
                    .size{square_size}, 
                    .tlc_pos{square_size.x * (7 - col) + square_offset.x, 
                             square_size.y * (7 - row) + square_offset.y}
                };
                piece_s[piece.is_white][static_cast<size_t>(piece.type)]
                    .draw(piece_rect);
            }
        }
    }
}

auto ChessWidget::get_square(glm::vec2 mouse_pos) const
    -> Square {
    glm::vec2 relative_pos = (mouse_pos - rect.tlc_pos)/(rect.size/8.0f);
    Square ret{7 - static_cast<int>(relative_pos.y), 7 - static_cast<int>(relative_pos.x)};
    return ret;
}

auto ChessWidget::on_click_action()
    -> void {
    Square const clicked = get_square({Mouse::getX(), Mouse::getY()});
    if (clicked.isOutOfBounds()) return;
    if (m_selected) {
        if (m_board[*m_selected]->get().is_valid_move(clicked)) {
            m_board.move(m_selected.value(), clicked);
            m_board.soft_update();
        }
        m_selected.reset();
    } else {
        auto const selected_piece_opt = m_board[clicked];
        if (selected_piece_opt.has_value() and 
            selected_piece_opt->get().is_white() == m_board.is_white_turn()) { 
            m_selected = clicked;
        }
    }
}
EvalBar::EvalBar(float& eval)
    : curr_eval{eval} {
    white = rectangle{
        {1.0f, 1.0f, 1.0f},
        ItemFrame{
            .tlc_pos{610.0f, 0.0f},
            .size{40.0f, 600.0f}
        }
    };
    black = rectangle{
        {0.0f, 0.0f, 0.0f},
        ItemFrame{
            .tlc_pos{610.0f, 0.0f},
            .size{40.0f, 600.0f}
        }
    };
}
auto EvalBar::update()
    -> void {
    float height = glm::clamp(curr_eval, -30.0f, 30.0f) * 10.0f -300.0f;
    black.rescale({40.0f, -height});
}
auto EvalBar::draw() 
    -> void {
    white.draw();
    black.draw();
}

