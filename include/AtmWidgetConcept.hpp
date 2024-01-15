#pragma once

#include <tuple>
#include <concepts>
#include <utility>

#include "AtmTypes.hpp"

/**
 * @brief UIクラスのコンセプト
 * @tparam position_type 位置座標の型
 * @tparam length_type 長さの型
 * @tparam T 検査する型
 */
template<class position_type, class length_type, class T>
concept Widget = requires(T& w) {
    { w.area_size() } -> std::same_as<const std::tuple<length_type, length_type>>;
    { w.area_position() } -> std::same_as<const std::tuple<position_type, position_type>>;
    w.resize_area(std::tuple<length_type, length_type>{});
    w.place(std::tuple<position_type, position_type>{});

    { w.under(std::tuple<position_type, position_type>{}) } -> std::same_as<bool>;
};

template<class T>
concept Widget2 = requires(T& w) {
    // 境界を返すbundary_area関数を要求する
    // const関数も定義する
    { w.boundary_area() } -> std::same_as<const boundary_t>;
    { std::as_const(w).boundary_area() } -> std::same_as<const boundary_t>;
    w.boundary_area(boundary_t{});
};
