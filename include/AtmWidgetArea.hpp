#pragma once

#include <algorithm>
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

    const bool under(const pos_type& p) noexcept {
        const auto [w, h] = size;
        const auto [x, y] = pos;
        const auto [p_x, p_y] = p;
        return
            x <= p_x && p_x <= (x + w) &&
            y <= p_y && p_y <= (y + h);
    }

    iterator begin() noexcept { return widgets.begin(); }
    const_iterator begin() const noexcept { return widgets.begin(); }
    iterator end() noexcept { return widgets.end(); }
    const_iterator end() const noexcept { return widgets.end(); }
};

static_assert(Widget<int32_t, std::size_t, WidgetArea<>>);

#include "boost/geometry/algorithms/disjoint.hpp"
#include "AtmTypes.hpp"

template<class coord_type, Widget2<coord_type> ...widgets>
struct WidgetArea2 {
    using widget_type = std::variant<widgets...>;
    using widget_container = std::vector<widget_type>;

    using iterator = typename widget_container::iterator;
    using const_iterator = typename widget_container::const_iterator;

    using boundary_type = boundary_t<coord_type>;

    widget_container widget_array;
    boundary_type area_boundary;

    template<class widget_t>
    void push_back(const widget_t& w) noexcept {
        widget_array.push_back(w);
    };

    const boundary_type boundary_area() const noexcept {
        return area_boundary;
    }

    void boundary_area(const boundary_type& b) noexcept {
        area_boundary = b;
    }

    iterator under(const point_t& pos) noexcept {
        using namespace boost;
        // posが領域外にあるかを調べる
        if(geometry::disjoint(area_boundary, pos)) {
            return widget_array.end();
        }

        // posの下にあるUIを探す
        return std::find_if(widget_array.begin(), widget_array.end(), [&pos](const widget_type& wg){
            const boundary_t<coord_type> area = std::visit([](const auto& w){
                return w.boundary_area();
            }, wg);
            return !geometry::disjoint(area, pos);
        });
    };

    iterator begin() noexcept { return widget_array.begin(); }
    const_iterator begin() const noexcept { return widget_array.begin(); }
    iterator end() noexcept { return widget_array.end(); }
    const_iterator end() const noexcept { return widget_array.end(); }
};

static_assert(Widget2<WidgetArea2<int>, int>);