#ifndef MOUSE_H__
#define MOUSE_H__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

class Mouse {
public:
    static void cursorPosCallback(GLFWwindow* window, double x, double y);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

    [[nodiscard]]static double getX() noexcept;
    [[nodiscard]]static double getY() noexcept;
    [[nodiscard]]static double getDx() noexcept;
    [[nodiscard]]static double getDy() noexcept;
    [[nodiscard]]static double getScrollDx() noexcept;
    [[nodiscard]]static double getScrollDy() noexcept;

    [[nodiscard]]static bool button(int button);
    [[nodiscard]]static bool buttonWentDown(int button);
    [[nodiscard]]static bool buttonWentUp(int button);
private:
    [[nodiscard]]static bool buttonChanged(int button);
private:
    static double m_x;
    static double m_y;
    static double m_last_x;
    static double m_last_y;
    static double m_dx;
    static double m_dy;
    static double m_scroll_dx;
    static double m_scroll_dy;
    
    static bool m_is_first_appearance;

    static bool m_buttons[];
    static bool m_buttons_changed[];
};

#endif