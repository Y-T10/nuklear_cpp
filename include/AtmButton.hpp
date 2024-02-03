#pragma once

#include <string>
#include "AtmWidgetConcept.hpp"
#include "AtmTypes.hpp"

template <class coord_type>
struct Button {
    std::string text;
    boundary_t<coord_type> button_area;
    bool is_pressed;

    const boundary_t<coord_type> boundary_area() noexcept {
        return button_area;
    }

    const boundary_t<coord_type> boundary_area() const noexcept {
        return button_area;
    }

    void boundary_area(const boundary_t<coord_type>& area) noexcept {
        button_area = area;
    }

    const bool pressed() const {
        return is_pressed;
    }

    void push() {
        is_pressed = true;
    }

    void release() {
        is_pressed = false;
    }
};

static_assert(Widget2<Button<int>, int>);