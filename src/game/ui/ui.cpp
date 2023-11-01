#include "ui.h"

Widget::Widget(Widget const& other) 
    : rect(other.rect) {
    pimpl = other.pimpl->clone();
}

auto Widget::draw()
    -> void {
    pimpl->do_draw(rect);
}

auto Widget::on_click_action() 
    -> void {
    pimpl->do_on_click_action();
}


auto Widget::on_hover_action() 
    -> void {
    pimpl->do_on_hover_action();
}

auto Widget::shift_by(glm::vec2 offset) 
    -> Widget& {
    rect.tlc_pos += offset;
    return *this;
}

auto Widget::scale_by(glm::vec2 scale) 
    -> Widget& {
    rect.size *= scale;
}