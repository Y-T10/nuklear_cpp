#pragma once

#include <cstddef>
#include <type_traits>
#include <variant>
#include <cstdint>
#include <vector>

#include "AtmWidgetConcept.hpp"

template<class position_type = int32_t, class length_type = std::size_t, Widget<position_type, length_type> ...wigets>
struct WidgetArea {
    using widget_type = std::variant<wigets...>;
    using widget_container = std::vector<widget_type>;

    using iterator = typename widget_container::iterator;
    using const_iterator = typename widget_container::const_iterator;

    using size_type = std::tuple<length_type, length_type>;
    using pos_type = std::tuple<position_type, position_type>;

    widget_container widgets;
    size_type size;
    pos_type pos;

    template<class T>
    void push_back(const T& w) noexcept {
        widgets.push_back(w);
    };

    const size_type area_size() noexcept {
        return size;
    }

    const pos_type area_position() noexcept {
        return pos;
    }

    template<class Func>
    void resize_area(const size_type& s, const Func& func = Func{}) noexcept {
        static_assert(std::is_invocable_v<Func, WidgetArea&, const size_type&>);
        resize(s);
        func(*this, size);
    }
    void resize_area(const size_type& s) noexcept {
        size = s;
    }
    
    void place(const pos_type& p) noexcept {
        pos = p;
    }

    iterator begin() noexcept { return widgets.begin(); }
    const_iterator begin() const noexcept { return widgets.begin(); }
    iterator end() noexcept { return widgets.end(); }
    const_iterator end() const noexcept { return widgets.end(); }
};

static_assert(Widget<int32_t, std::size_t, WidgetArea<>>);