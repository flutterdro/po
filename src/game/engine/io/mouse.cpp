#include "mouse.h"

double Mouse::m_x                                     = 0;
double Mouse::m_y                                     = 0;
double Mouse::m_last_x                                = 0;
double Mouse::m_last_y                                = 0;
double Mouse::m_dx                                    = 0;
double Mouse::m_dy                                    = 0;
double Mouse::m_scroll_dx                             = 0;
double Mouse::m_scroll_dy                             = 0;

bool Mouse::m_is_first_appearance                     = true;
bool Mouse::m_buttons[GLFW_MOUSE_BUTTON_LAST]         = {0};
bool Mouse::m_buttons_changed[GLFW_MOUSE_BUTTON_LAST] = {0};

void Mouse::cursorPosCallback(GLFWwindow * window, double x, double y) {
    m_x = x;
    m_y = y;

    if (m_is_first_appearance) {
        m_last_x              = x;
        m_last_y              = y;
        m_is_first_appearance = false;
    }
    m_dx     = m_last_x - m_x;
    m_dy     = m_y - m_last_y;

    m_last_x = x;
    m_last_y = y;
}

void Mouse::mouseButtonCallback(GLFWwindow * window, int button, int action,
                                int mods) {
    if (action != GLFW_RELEASE) {
        if (!m_buttons[button]) { m_buttons[button] = true; }
    } else {
        m_buttons[button] = false;
    }
    m_buttons_changed[button] = action != GLFW_REPEAT;
}

void Mouse::mouseWheelCallback(GLFWwindow * window, double dx, double dy) {
    m_scroll_dx = dx;
    m_scroll_dy = dy;
}

double Mouse::getX() noexcept { return m_x; }

double Mouse::getY() noexcept { return m_y; }

double Mouse::getDx() noexcept {
    double ret = m_dx;
    m_dx       = 0;
    return ret;
}

double Mouse::getDy() noexcept {
    double ret = m_dy;
    m_dy       = 0;
    return ret;
}

double Mouse::getScrollDx() noexcept {
    double ret  = m_scroll_dx;
    m_scroll_dx = 0;
    return ret;
}

double Mouse::getScrollDy() noexcept {
    double ret  = m_scroll_dy;
    m_scroll_dy = 0;
    return ret;
}

bool Mouse::button(int button) { return m_buttons[button]; }

bool Mouse::buttonChanged(int button) {
    bool ret                  = m_buttons_changed[button];
    m_buttons_changed[button] = false;
    return ret;
}

bool Mouse::buttonWentDown(int button) {
    return m_buttons[button] && buttonChanged(button);
}

bool Mouse::buttonWentUp(int button) {
    return !m_buttons[button] && buttonChanged(button);
}
