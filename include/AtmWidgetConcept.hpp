#pragma once

#include <tuple>
#include <type_traits>

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
