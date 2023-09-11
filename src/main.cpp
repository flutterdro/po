#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <iostream>
// #include <TGUI.hpp>
// #include <Backend/GLFW-OpenGL3.hpp>

#define GLFW_INCLUDE_NONE
#define GLAD_DEBUG
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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


int main() {

    if(!glfwInit()) {
        LOG(LoggerLvl::ERROR, "Failed to initialize GLFW\n");
        return -1;
    } 

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef DEBUG_ 
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(800, 800, "Chess", nullptr, nullptr);
    if (!window) {
        LOG(LoggerLvl::ERROR, "Failed to create a window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
    glfwSetScrollCallback(window, Mouse::mouseWheelCallback);
    glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        LOG(LoggerLvl::ERROR, "Failed to initialiaze OpenGL context\n");
        glfwTerminate();
        return -1;
    } else {
        LOG(LoggerLvl::STATUS, "Loaded OpenGL {}.{}\n", 
        GLAD_VERSION_MAJOR(version), 
        GLAD_VERSION_MINOR(version));
    }
    glViewport(0,0,800,800);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    RM& rm = RM::getInstance();
    loadResources();
    Game chess;
    chess.init();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        chess.draw();
        chess.update();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    rm.cleanUp();
    return 0;
}