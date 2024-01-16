#include <type_traits>
#include <tuple>
#include <utility>
#include <variant>
#include <memory>
#include <optional>
#include <string>

#include "fmt/core.h"
#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS
#include "boost/scope_exit.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_scancode.h"
#include "SDL_events.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_pixels.h"
#include "SDL_ttf.h"

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

    template <SDL_EventType event_type, class event_functor>
    struct EventFunctor {
        constexpr static auto type = event_type;
        using functor = event_functor;
    };

    template<class context_type>
    void DispatchEvent(const SDL_Event& event, context_type& ctx) {
        return;
    }

    template<class context_type, class handler, class ...rests>
    void DispatchEvent(SDL_Event&& event, context_type&& ctx) {
        static_assert(std::is_invocable_v<typename handler::functor, SDL_Event&&, context_type&&>);
        if(event.type == handler::type){
            typename handler::functor{}(std::forward<SDL_Event>(event), std::forward<context_type>(ctx));
            return;
        }
        if constexpr (sizeof...(rests) > 0) {
            DispatchEvent<context_type, rests...>(std::forward<SDL_Event>(event), std::forward<context_type>(ctx));
            return;
        }
        return;
    }

    using Surface = SDL_ptr<SDL_Surface, SDL_FreeSurface>;
}

namespace SDL::TTF {
    using Font = std::unique_ptr<TTF_Font, decltype([](TTF_Font* ptr){TTF_CloseFont(ptr);})>;

    const Font OpenFont(const std::string& file, int point_size = 12) noexcept {
        return Font(TTF_OpenFont(file.c_str(), point_size));
    }

    const std::optional<std::pair<int, int>> UTF8TextSize(const Font& font, const std::string text) noexcept {
        if(!font.get()) {
            return std::nullopt;
        }
        int w = 0, h = 0;
        if(TTF_SizeUTF8(font.get(), text.c_str(), &w, &h) == 0) {
            return std::make_optional(std::make_pair(w, h));
        }
        return std::nullopt;
    }

    const Surface WriteText(const Font& font, const std::string& text, const SDL_Color& fg, const SDL_Color& bg) noexcept {
        return Surface(TTF_RenderUTF8_LCD(font.get(), text.c_str(), fg, bg));
    }
}

int main(int argc, char* argv[]) {
    fmt::println("sample program with fmt!");

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    BOOST_SCOPE_EXIT_ALL(){
        TTF_Quit();
        SDL_Quit();
    };

    const SDL::Window window = SDL::Create<SDL::Window, SDL_CreateWindow>("demo", 0, 0, 800, 600, SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(window.get() == nullptr){
        return __LINE__;
    }

    const auto renderer = SDL::Create<SDL::Renderer, SDL_CreateRenderer>(window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
    if(renderer.get() == nullptr){
        return __LINE__;
    }

    const SDL::TTF::Font JPFont = SDL::TTF::OpenFont("/usr/share/fonts/opentype/ipafont-mincho/ipam.ttf");
    if(JPFont.get() == nullptr) {
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

    using ctx_type = std::tuple<decltype(sampleArea)&, bool&>;
    using ButtonDown = SDL::EventFunctor<SDL_MOUSEBUTTONDOWN, decltype([](SDL_Event&& e, ctx_type&& ctx){
            auto sampleArea = std::get<WidgetArea2<Button>&>(ctx);
            const auto widget = sampleArea.under({e.button.x, e.button.y});
            std::get<bool&>(ctx) = false;
            if(widget == sampleArea.end()){
                return;
            }
            // visitを使う実装に変える
            std::get<Button>(*widget).push();
        })>;
    using ButtonUp = SDL::EventFunctor<SDL_MOUSEBUTTONUP, decltype([](SDL_Event&& e, ctx_type&& ctx){
            auto sampleArea = std::get<WidgetArea2<Button>&>(ctx);
            const auto widget = sampleArea.under({e.button.x, e.button.y});
            if(widget == sampleArea.end()){
                return;
            }
            // visitを使う実装に変える
            std::get<Button>(*widget).release();
        })>;
    using KeyDown = SDL::EventFunctor<SDL_KEYDOWN, decltype([](SDL_Event&& e, ctx_type&& ctx){
            if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                std::get<bool&>(ctx) = false;
            }
        })>;

    for(bool isRunning=true; isRunning;) {
        for (SDL_Event e; SDL_PollEvent(&e);) {
            SDL::DispatchEvent<ctx_type,
                ButtonDown, ButtonUp, KeyDown
            >(std::forward<SDL_Event>(e), std::make_tuple(std::ref(sampleArea), std::ref(isRunning)));
        }

        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 100, 255);
        SDL_RenderClear(renderer.get());
        for(const auto& widget: sampleArea) {
            const auto& buton = std::get<SampleButton>(widget);
            // variantb likeな実装にする
            // getの値を受け取るとまずい
            if(buton.pressed()) {
                SDL_SetRenderDrawColor(renderer.get(), 200, 200, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer.get(), 0, 100, 100, 255);
            }
            const SDL_Rect drawRect = Boundary2Rect(buton.boundary_area());
            SDL_RenderFillRect(renderer.get(), &drawRect);
        }
        SDL_RenderPresent(renderer.get());
        SDL_Delay(1);
    }


    return 0;
}
