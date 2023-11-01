#ifndef USER_INTERFACE_H__
#define USER_INTERFACE_H__

#include <functional>
#include <vector>

#include <glm/glm.hpp>

#include "../engine/sprite.h"
#include "../utilities/rectangle.h"

template<typename WidgetT>
concept WidgetC = requires(WidgetT&& w) {
    { w.draw() }           -> std::same_as<void>;
    { w.on_click_action() } -> std::same_as<void>;
    { w.on_hover_action() } -> std::same_as<void>;
    { WidgetT::load_resources() };
};

class Widget {
    struct WidgetConcept {
        virtual ~WidgetConcept() = default;

        virtual auto do_draw(ItemFrame const&)
            -> void = 0;
        virtual auto do_on_click_action()
            -> void = 0;
        virtual auto do_on_hover_action() 
            -> void = 0;

        virtual auto clone() const
            -> std::unique_ptr<WidgetConcept> = 0;
    };
    template<WidgetC WidgetT>
    struct WidgetModel : public WidgetConcept {
        WidgetModel(WidgetT widget)
            : widget(std::move(widget)) {}
        
        auto do_draw(ItemFrame const& rect_) 
            -> void override final {
            widget.draw(rect_);
        }
        auto do_on_click_action()
            -> void override final {
            widget.on_click_action();
        }
        auto do_on_hover_action()
            -> void override final {
            widget.on_hover_action();
        }

        auto clone() const
            -> std::unique_ptr<WidgetConcept> override final {
            return std::make_unique<WidgetConcept>(*this);
        }
        WidgetT widget;
    };
public:
    template<WidgetC WidgetT>
    Widget(WidgetT widget) 
        : pimpl{std::make_unique<WidgetModel>(std::move(widget))} {}
    Widget(Widget&&) = default;
    auto operator=(Widget&&) 
        -> Widget& = default;
    Widget(Widget const& other);

    auto draw()
        -> void;
    auto on_click_action()
        -> void;
    auto on_hover_action()
        -> void;

    auto shift_by(glm::vec2 offset)
        -> Widget&;
    auto scale_by(glm::vec2 scale)
        -> Widget&;
    
private:
    auto contains_cursor()
        -> bool;
private:
    std::unique_ptr<WidgetConcept> pimpl;
    ItemFrame rect;
};

// class Widget {
// public:
//     Widget() = default;
//     Widget(Widget const&) = delete;
//     Widget& operator=(Widget const&) = delete;
//     virtual ~Widget() = default;

//     auto set_sprite(Sprite const&)
//         -> void;
//     auto get_sprite()
//         -> Sprite&;
//     virtual auto draw() 
//         -> void = 0;
//     virtual auto on_click_action() 
//         -> void = 0;
// protected:
//     Sprite m_sprite;
// };

class Button {
public:
    Button();

    void onHoverEvent();
    void bindEvent(std::function<void()> event);
};

#endif