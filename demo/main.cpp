#include <type_traits>
#include <variant>
#include <memory>

#include "fmt/core.h"
#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS
#include "boost/scope_exit.hpp"

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

    SDL_Init(SDL_INIT_EVERYTHING);

    BOOST_SCOPE_EXIT_ALL(){
        SDL_Quit();
    };

    const SDL::Window window = SDL::Create<SDL::Window, SDL_CreateWindow>("demo", 0, 0, 800, 600, SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP & SDL_WindowFlags::SDL_WINDOW_SHOWN);
    if(window.get() == nullptr){
        return __LINE__;
    }

    const auto renderer = SDL::Create<SDL::Renderer, SDL_CreateRenderer>(window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
    if(renderer.get() == nullptr){
        return __LINE__;
    }

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

    while(true) {
        for (SDL_Event e; SDL_PollEvent(&e);) {
            /// unipue_ptrと同様に <SDL_MOUSEBUTTONDOWN, [](const SDL_Event& e, const context_type& ctx) {}>とかけるようにする．
            if(e.type == SDL_EventType::SDL_MOUSEBUTTONDOWN) {
                const auto widget = sampleArea.under({e.button.x, e.button.y});
                if(widget == sampleArea.end()){
                    continue;
                }
                // visitを使う実装に変える
                std::get<Button>(*widget).push();
            }

            if(e.type == SDL_EventType::SDL_MOUSEBUTTONUP) {
                const auto widget = sampleArea.under({e.button.x, e.button.y});
                if(widget == sampleArea.end()){
                    continue;
                }
                // visitを使う実装に変える
                std::get<Button>(*widget).release();
            }
        }
    }

    return 0;
}
