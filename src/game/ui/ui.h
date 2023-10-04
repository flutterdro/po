#ifndef USER_INTERFACE_H__
#define USER_INTERFACE_H__

#include <functional>

#include <glm/glm.hpp>

class Widget {

};

class Button {
public:
    Button();

    void onHoverEvent();
    void bindEvent(std::function<void()> event);

private:
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    std::function<void()> m_on_click_event;
};

#endif