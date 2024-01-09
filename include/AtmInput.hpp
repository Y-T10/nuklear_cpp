#pragma once

#include <algorithm>
#include <tuple>
#include <variant>
#include "AtmWidgetArea.hpp"

template<class position_type, class length_type, class ...wigets>
struct WidgetAreaInput {
    using area_type = WidgetArea<position_type, length_type, wigets...>;
    using area_iterator = typename area_type::iterator;

    area_type& target_area;

    WidgetAreaInput(area_type& area) noexcept:
    target_area(area){
    };

    /**
     * @brief 座標pos下のUIをareaから探し出す．
     * posの座標空間は、areaが存在する座標空間と同じ．
     * @tparam T 座標値の型
     * @param pos タッチした位置
     * @return 発見したUIへの参照かend()の値．
     */
    template<class T>
    area_iterator FindWidgetWithPos(const std::tuple<T,T> pos) noexcept {
        const auto relative_pos = pos - target_area.area_position();
        if(!IsHit(target_area, pos)){
            return target_area.end();
        }
        return std::find_if(target_area.begin(), target_area.end(), [&pos](const typename area_type::widget_type& w){
            return IsHit(w, pos);
        });
    };

    template<class T>
    static const bool IsHit(const typename area_type::widget_type& widget, const std::tuple<T,T> pos) noexcept {
        const auto [w_wigth, w_height] = std::visit(widget, [](const auto& w){ return w.area_size(); });
        const auto [w_x, w_y] = std::visit(widget, [](const auto& w){return w.area_position(); });
        const auto [x, y] = pos;

        return
            w_x <= x && x <= (w_x + w_wigth) &&
            w_y <= y && y <= (w_y + w_height);
    }
};