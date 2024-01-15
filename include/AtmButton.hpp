#pragma once

#include <string>
#include "AtmWidgetConcept.hpp"
#include "AtmTypes.hpp"

struct Button {
    std::string text;
    boundary_t button_area;
    bool is_pressed;

    const boundary_t boundary_area() const noexcept {
        return button_area;
    }

    void boundary_area(const boundary_t& area) noexcept {
        button_area = area;
    }

    const bool pressed() {
        return is_pressed;
    }

    void push() {
        is_pressed = true;
    }

    void release() {
        is_pressed = false;
    }
};