#pragma once

#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/box.hpp"

/// 平面座標上の点
using point_t = typename boost::geometry::model::d2::point_xy<double>;

/// UIの境界領域
using boundary_t = typename boost::geometry::model::box<point_t>;