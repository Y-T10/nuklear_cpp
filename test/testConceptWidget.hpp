#pragma once

#include "../include/AtmWidgetConcept.hpp"

#include <cstddef>
#include <cstdint>

struct Button {
    const std::tuple<size_t, size_t> area_size() noexcept;
    const std::tuple<int32_t, int32_t> area_position() noexcept;

    void resize_area(const std::tuple<size_t, size_t>& s) noexcept;
    void place(const std::tuple<int32_t, int32_t>& p) noexcept;
};

struct BrokenButton {
    const std::tuple<size_t, size_t> area_size() noexcept;
    const std::tuple<int32_t, int32_t> area_position() noexcept;
};

static_assert(Widget<int32_t, size_t, Button>);
static_assert(!Widget<int64_t, size_t, Button>);
static_assert(!Widget<int32_t, uint8_t, Button>);
static_assert(!Widget<int32_t, size_t, BrokenButton>);