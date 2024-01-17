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
#include "SDL_rect.h"

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

template <class T>
const SDL_Rect Boundary2Rect(const boundary_t<T>& box) noexcept {
    return SDL_Rect{
        .x = box.min_corner().x(), .y = box.min_corner().x(),
        .w = box.max_corner().x() - box.min_corner().x(),
        .h = box.max_corner().y() - box.min_corner().y(),
    };
}

template<class T>
void DrawButton(const SDL::Renderer& renderer, const Button<T>& button) noexcept {
    if(button.pressed()) {
        SDL_SetRenderDrawColor(renderer.get(), 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer.get(), 0, 100, 100, 255);
    }
    const SDL_Rect drawRect = Boundary2Rect(button.boundary_area());
    SDL_RenderFillRect(renderer.get(), &drawRect);
};

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

    using SampleButton = Button<int>;
    using SampleArea = WidgetArea2<int, SampleButton>;

    SampleArea sampleArea{
        .widget_array = {},
        .area_boundary = boundary_t<int>{point_t<int>{0,0},point_t<int>{800,600}}
    };

    sampleArea.emplace_back(
        SampleButton{
            .text = "hello",
            .button_area = boundary_t<int>{point_t<int>{10,10},point_t<int>{110,30}},
            .is_pressed = false
        }
    );

    using ctx_type = std::tuple<SampleArea &, bool &>;
    using ButtonDown = SDL::EventFunctor<SDL_MOUSEBUTTONDOWN, decltype([](SDL_Event&& e, ctx_type&& ctx){
            auto widget = std::get<SampleArea&>(ctx).under<int>({e.button.x, e.button.y});
            if(widget == std::get<SampleArea&>(ctx).end()){
                return;
            }
            std::visit(decltype([]<class T>(T& w) {
                if constexpr (std::is_same_v<T, SampleButton>) {
                    w.push();
                    return;
                }
            }){}, *widget);
        })>;
    using ButtonUp = SDL::EventFunctor<SDL_MOUSEBUTTONUP, decltype([](SDL_Event&& e, ctx_type&& ctx){
            SampleArea& sampleArea = std::get<SampleArea&>(ctx);
            auto widget = sampleArea.under<int>({e.button.x, e.button.y});
            if(widget == sampleArea.end()){
                return;
            }
            std::visit(decltype([]<class T>(T& w) {
                if constexpr (std::is_same_v<T, SampleButton>) {
                    w.release();
                    return;
                }
            }){}, *widget);
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
            >(std::forward<SDL_Event>(e), std::tie(sampleArea, isRunning));
        }

        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 100, 255);
        SDL_RenderClear(renderer.get());
        for(const auto& widget: sampleArea) {
            std::visit([&renderer]<class T>(const T& w) {
                if constexpr (std::is_same_v<T, SampleButton>) {
                    DrawButton(renderer, w);
                    return;
                }
            }, widget);
        }
        SDL_RenderPresent(renderer.get());
        SDL_Delay(1);
    }


    return 0;
}
