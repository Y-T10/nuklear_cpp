#include <type_traits>
#include <memory>

#include "fmt/core.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_render.h"
#include "SDL_events.h"

#include "AtmWidgetArea.hpp"
#include "AtmButton.hpp"
#include "AtmTypes.hpp"

namespace SDL {
    template<class T, auto deleter>
    using SDL_ptr = std::unique_ptr<T, decltype([](T* ptr){deleter(ptr);})>;

    using Window = SDL_ptr<SDL_Window, SDL_DestroyWindow>;
    using Renderer = SDL_ptr<SDL_Renderer, SDL_DestroyRenderer>;

    /// 元関数と同じ引数を受け取れるようにする
    /// boost のargument typeを使うと良さそう
    template<class ptr_type, auto Func, class ...Args>
    const ptr_type Create(Args&& ...args) noexcept {
        /// static_assterのみがエラーとなるようにするため、評価結果の変数を用意した．
        constexpr bool chechFunc = std::is_invocable_r_v<typename ptr_type::pointer, decltype(Func), Args...>;
        static_assert(chechFunc, "Func or ptr_type is wrong.");
        if constexpr (chechFunc) {
            return ptr_type(Func(args...));
        }
        return nullptr;
    };
}

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
