#pragma once

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <variant>
#include <cstdint>
#include <vector>
#include <utility>

#include "AtmWidgetConcept.hpp"

#include "boost/geometry/algorithms/disjoint.hpp"
#include "AtmTypes.hpp"

/// TODO: ウィジェト内の座標と外の座標の変換を扱う
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

    template<class widget_t>
    void emplace_back(widget_t&& w) noexcept {
        widget_array.emplace_back(std::forward<widget_t>(w));
    };

    const boundary_type boundary_area() const noexcept {
        return area_boundary;
    }

    void boundary_area(const boundary_type& b) noexcept {
        area_boundary = b;
    }

    template <class T>
    iterator under(const point_t<T>& pos) noexcept {
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