#pragma once

#include <tuple>
#include <concepts>
#include <utility>

#include "AtmTypes.hpp"

template<class T, class coord_type>
concept Widget2 = requires(T& w) {
    // 境界を返すbundary_area関数を要求する
    // const関数も定義する
    { w.boundary_area() } -> std::same_as<const boundary_t<coord_type>>;
    { std::as_const(w).boundary_area() } -> std::same_as<const boundary_t<coord_type>>;
    w.boundary_area(boundary_t<coord_type>{});
};
