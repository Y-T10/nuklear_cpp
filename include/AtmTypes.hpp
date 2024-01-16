#pragma once

#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/box.hpp"

/// 平面座標上の点
template<class T>
using point_t = typename boost::geometry::model::d2::point_xy<T>;

/// UIの境界領域
template<class T, class point_type = point_t<T>>
using boundary_t = typename boost::geometry::model::box<point_type>;