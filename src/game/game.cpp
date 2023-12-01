#include "game.h"

#include "ai/search.h"
#include "engine/io/keyboard.h"

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
    engine::resource_manager& rm = engine::resource_manager::get_instance();
    glm::mat4 const projection = glm::ortho(0.0f, (float)m_window.get_wwidth(), (float)m_window.get_wheight(), 0.0f, -1.0f, 1.0f);
    rm.request<engine::shader>("pieces").set("projection", projection);
    rm.request<engine::shader>("basic-rectangle").set("projection", projection);
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
    if (Keyboard::key_went_down(GLFW_KEY_U)) {
        if (not m_move_history.empty()) {
            m_board.undo_move(m_move_history.back());
            m_move_history.pop_back();
        }
    }
}
auto Game::proccessClick() -> void {
    auto const opt = m_gui.on_click_action();
    if (opt.has_value()) {
        m_move_history.push_back(*opt);
    }
    m_board.hard_update();
}
auto Game::update() -> void {
    proccessInput();
    // rtw::search(m_board);
    if (m_board.turn_to_move() == chess::piece::color::black) {
        chess::board board = rtw::search(m_board);
        m_board.swap(board);
        m_board.set_turn(chess::piece::color::white);
    }
    evaluation = m_board.evaluate();
    eval_bar.update();
}

auto ChessWidget::load_resources()
    -> void {
    [[maybe_unused]]static char dummy = [](){
        engine::resource_manager& rm = engine::resource_manager::get_instance();

        unsigned char data[64];
        for (int i = 0; i < 64; ++i) {
            // those warnings JUST WON"T SHUTUP
            data[i] = static_cast<unsigned char>((i + i / 8) % 2 * 0xFF);
        }
        rm.submit_emplace<engine::texture>("board", data);

        const fs::path piece_sprites_dir{"assets/chess-sprites"};
        for (auto const& dir_entry : fs::directory_iterator{piece_sprites_dir}) {
            if (dir_entry.is_regular_file() &&
                dir_entry.path().filename().c_str()[0] != '.') {
                rm.submit_emplace<engine::texture>(dir_entry.path().stem(), dir_entry.path().c_str());
            }
        }

        return 1;
    }();
}

ChessWidget::ChessWidget(chess::board& board)
    : m_board{board} {

    engine::resource_manager& rm = engine::resource_manager::get_instance();

    m_sprite = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("board"),
        .texture = rm.request<engine::texture>("board")
    };
    rect.size = {600.0f, 600.0f};

    piece_s[1][static_cast<size_t>(PieceType::Pawn)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("white-pawn")
    };
    piece_s[1][static_cast<size_t>(PieceType::King)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("white-king")
    };
    piece_s[1][static_cast<size_t>(PieceType::Queen)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("white-queen")
    };
    piece_s[1][static_cast<size_t>(PieceType::Rook)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("white-rook")
    };
    piece_s[1][static_cast<size_t>(PieceType::Knight)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("white-knight")
    };
    piece_s[1][static_cast<size_t>(PieceType::Bishop)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("white-bishop")
    };
    piece_s[0][static_cast<size_t>(PieceType::Pawn)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("black-pawn")
    };
    piece_s[0][static_cast<size_t>(PieceType::King)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("black-king")
    };
    piece_s[0][static_cast<size_t>(PieceType::Queen)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("black-queen")
    };
    piece_s[0][static_cast<size_t>(PieceType::Rook)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("black-rook")
    };
    piece_s[0][static_cast<size_t>(PieceType::Knight)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("black-knight")
    };
    piece_s[0][static_cast<size_t>(PieceType::Bishop)] = engine::sprite{
        .vao = rm.request<engine::vao>(),
        .shader = rm.request<engine::shader>("pieces"),
        .texture = rm.request<engine::texture>("black-bishop")
    };

}

auto ChessWidget::get_sprite()
    -> engine::sprite& { return m_sprite; }
auto ChessWidget::draw()
    -> void {
    i_board = m_board.to_ibr();

    if (m_selected) {
        chess::bitboard board = m_board[*m_selected]->get().get_moves();
        int arr[64];
        for (int i = 0; i < 64; ++i) { arr[i] = board.test(chess::to_square(7 - i / 8, 7 - i % 8)); }
        m_sprite.shader.set("board", arr);
    } else {
        int const arr[64]{};
        m_sprite.shader.set("board", arr);
    }

    m_sprite.draw(rect);
    glm::vec2 const square_size = rect.size * (1.0f / 8.0f);
    glm::vec2 const square_offset = rect.tlc_pos;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            IntermediatePieceData const piece = i_board[chess::to_square(row, col)];
            if (piece.type != PieceType::None) {
                piece_s[piece.is_white][static_cast<size_t>(piece.type)]
                    .shader
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
    -> chess::square {
    glm::vec2 relative_pos = (mouse_pos - rect.tlc_pos)/(rect.size/8.0f);
    chess::square ret{chess::to_square(7 - static_cast<int>(relative_pos.y), 7 - static_cast<int>(relative_pos.x))};
    return ret;
}

auto ChessWidget::on_click_action()
    -> std::optional<chess::move> {
    chess::square const clicked = get_square({Mouse::getX(), Mouse::getY()});
    if (clicked.is_out_of_bounds()) return {};
    if (m_selected) {
        std::optional<chess::move> ret{};
        if (m_board[*m_selected]->get().is_valid_move(clicked)) {
            auto const move = m_board.gen_move_info(m_selected.value(), clicked);
            ret = move;
            m_board.do_move(move);
            m_board.soft_update();
        }
        m_selected.reset();
        return ret;
    } else {
        auto const selected_piece_opt = m_board[clicked];
        if (selected_piece_opt.has_value() and
            selected_piece_opt->get().get_color() == m_board.turn_to_move()) {
            m_selected = clicked;
        }
    }
    return {};
}
EvalBar::EvalBar(float& eval)
    : curr_eval{eval} {
    white = engine::rectangle{
        {1.0f, 1.0f, 1.0f},
        ItemFrame{
            .tlc_pos{610.0f, 0.0f},
            .size{40.0f, 600.0f}
        }
    };
    black = engine::rectangle{
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

