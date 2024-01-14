#include "fmt/core.h"

#include "AtmWidgetArea.hpp"
#include "AtmButton.hpp"
#include "AtmTypes.hpp"

int main(int argc, char* argv[]) {
    fmt::println("sample program with fmt!");

    WidgetArea2<Button> sampleArea{
        .widgets = {},
        .area_boundary = boundary_t{point_t{0,0},point_t{800,600}}
    };

    sampleArea.push_back(
        Button{
            .text = "hello",
            .button_area = boundary_t{point_t{10, 10}, point_t{110, 30}},
            .is_pressed = false
        }
    );

    return 0;
}
