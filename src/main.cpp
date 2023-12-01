
#include <random>
#include <unordered_set>

#define GLFW_INCLUDE_NONE
#ifdef DEBUG_
#   define GLAD_DEBUG
#endif
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fmt/core.h>

#include "game/logic/piece.h"
#include "game/engine/io/keyboard.h"
#include "game/game.h"


int main() try {
    if (not glfwInit()) {
        log(LogLvl::ERROR, "Failed to initialize GLFW\n");
        return -1;
    }
    Window window(800, 600, "Chess");
    window.make_context_current();
    window.setup_mouse_callbacks<Mouse>();
    window.setup_keyboard_callback<Keyboard>();
    if (int version = gladLoadGL(glfwGetProcAddress); not version) {
        log(LogLvl::ERROR, "Failed to initialiaze OpenGL context\n");
        glfwTerminate();
        return -1;
    } else {
        log(LogLvl::STATUS, "Loaded OpenGL {}.{}\n",
            GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    }
    
    compile_shaders();
    loadResources();
    Game chess(window);
    chess::test();
    while (window.is_open()) {
        chess.update();
        chess.draw();

        window.update();
    }


    // std::ifstream fin1{"../../tests/test-data/movegen/black-pos.data"};
    // u64 board;
    // while (fin1 >> board) {
    //     fmt::print("{:b}\n", board);
    // }
    // chess::Rook<true> rook;
    // std::ofstream fout{"../../tests/test-data/movegen/positions.data"};
    // fout << 0 << ' ';
    // fout << 63 << ' ';
    // fout << 56 << ' ';
    // fout << 7 << ' ';
    // fout << 20 << ' ';
    // fout << 42 << ' ';
    // fout << 31 << ' ';
    // fout << 18 << ' ';
    // std::random_device rd;
    // std::mt19937 gen{rd()};
    // std::uniform_int_distribution<> dist(0, 63);
    // std::vector<u64> black_pos{};
    // std::vector<u64> white_pos{};
    // std::unordered_set<int> dict; 
    // while (black_pos.size() < 5) {
    //     while (dict.size() < 8) {
    //         dict.insert(dist(gen));
    //     }
    //     u64 in{0};
    //     for (const auto& square : dict) {
    //         in |= 1_u64 << square;
    //     }
    //     black_pos.push_back(in);
    //     dict.clear();
    // } 
    // while (white_pos.size() < 5) {
    //     while (dict.size() < 8) {
    //         dict.insert(dist(gen));
    //     }
    //     u64 in{0};
    //     for (const auto& square : dict) {
    //         in |= 1_u64 << square;
    //     }
    //     white_pos.push_back(in);
    //     dict.clear();
    // } 
    // std::ofstream fout;
    // fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    // fout.open("../../tests/test-data/movegen/black-pos.data");
    // for (auto bitboard : black_pos) {
    //     fout << bitboard << ' ';
    // }
    // fout.close();
    // fout.open("../../tests/test-data/movegen/white-pos.data");
    // for (auto bitboard : white_pos) {
    //     fout << bitboard << ' ';
    // }
    // fout.close();
    // test();
    glfwTerminate();
    return 0;
} catch (std::system_error& e) {
    fmt::print(stderr, "{}\n", e.code().message());
} catch (std::exception& e) {
    fmt::print("{}\n", e.what());
}