#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <iostream>

#define GLFW_INCLUDE_NONE
#ifdef DEBUG_
#   define GLAD_DEBUG
#endif
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fmt/core.h>

#include "rtw/matop.h"
#include "rtw/dectree.h"
#include "logger/logger.h"
#include "game/engine/shader.h"
#include "game/engine/texture.h"
#include "game/engine/sprite.h"
#include "game/engine/io/mouse.h"
#include "game/logic/piece.h"
#include "game/game.h"

using namespace rtw;


int main() try {
    if (!glfwInit()) {
        log(LogLvl::ERROR, "Failed to initialize GLFW\n");
        return -1;
    }
    Window window(800, 600, "Chess");
    window.make_context_current();
    window.setup_mouse_callbacks(Mouse{});
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

    while (window.is_open()) {
        chess.update();
        chess.draw();

        window.update();
    }
    glfwTerminate();
    return 0;
} catch (std::system_error& e) {
    fmt::print(stderr, "{}", e.code().message());
} catch (std::exception& e) {
    fmt::print("{}\n", e.what());
}