#include "ui.h"

void Button::bindEvent(std::function<void()> event) {
    m_on_click_event = std::move(event);
}