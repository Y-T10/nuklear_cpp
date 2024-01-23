#include "fmt/core.h"
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS
#include "boost/scope_exit.hpp"

#include "SDLCpp.hpp"
#include "SDLCpp_ttf.hpp"

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

#include "fontconfig/fontconfig.h"
#include <filesystem>

namespace FontconfigCpp {
    template<class T, void(*deleter)(T*)>
    struct Fc_deleter {
        void operator()(T* ptr) {
            deleter(ptr);
        };
    };

    template<class T, auto deleter>
    using Fc_ptr = std::unique_ptr<T, Fc_deleter<T, deleter>>;

    using Config = Fc_ptr<FcConfig, FcConfigDestroy>;
    using Pattern = Fc_ptr<FcPattern, FcPatternDestroy>;

    const Pattern FontMatch(const Config& conf, const Pattern& pattern) noexcept {
        FcResult result;
        Pattern fontPattern = Pattern(FcFontMatch(conf.get(), pattern.get(), &result));
        if(fontPattern.get() == nullptr) {
            return nullptr;
        }
        if(result != FcResult::FcResultMatch) {
            return nullptr;
        }
        return fontPattern;
    }

    const std::filesystem::path SearchFont(const Config& conf, const Pattern& pattern) noexcept {
        /// TODO: この処理は別関数に分けるのが良さそう?
        FcConfigSubstitute(conf.get(), pattern.get(), FcMatchPattern);
        FcDefaultSubstitute(pattern.get());

        const Pattern fontPattern = FontMatch(conf, pattern);
        if(!fontPattern.get()) {
            return "";
        }

        FcChar8* filePaht;
	    FcPatternGetString(fontPattern.get(), FC_FILE, 0, &filePaht);
        return std::filesystem::path((char*)filePaht);
    };

    const Config CurrentDefaultConfig() noexcept {
        return Config(FcConfigGetCurrent());
    }

    const Pattern CreatePattern(const std::map<std::string, std::basic_string<FcChar8>>& param) noexcept {
        Pattern pattern = Pattern(FcPatternCreate());
        if(pattern.get() == nullptr) {
            return nullptr;
        }

        if(param.empty()) {
            return pattern;
        }

        for(const auto& p: param) {
            if(!FcPatternAddString(pattern.get(), p.first.c_str(), p.second.c_str())) {
                return nullptr;
            }
        }
        return pattern;
    }
}

template <class T>
const SDL_Rect Boundary2Rect(const boundary_t<T>& box) noexcept {
    return SDL_Rect{
        .x = box.min_corner().x(), .y = box.min_corner().y(),
        .w = box.max_corner().x() - box.min_corner().x(),
        .h = box.max_corner().y() - box.min_corner().y(),
    };
}

using namespace SDL2Cpp;
using namespace SDL2Cpp::TTF;

using render_context = std::tuple<const Renderer&, const Font&>;

template<class T>
void DrawButton(const render_context& ctx, const Button<T>& button) noexcept {
    auto& [renderer, font] = ctx;
    const SDL_Color bg = button.pressed()?
        SDL_Color{.r = 200, .g = 200, .b = 0, .a = 255}:
        SDL_Color{.r = 0, .g = 100, .b = 100, .a = 255};
    SDL_SetRenderDrawColor(renderer.get(), bg.r, bg.g, bg.b, bg.a);
    const SDL_Rect drawRect = Boundary2Rect(button.boundary_area());
    SDL_RenderFillRect(renderer.get(), &drawRect);
    const auto text_image = WriteText(
        font, button.text, SDL_Color{.r = 0, .g = 0, .b = 0, .a =255}, bg
    );
    RenderSurface(renderer, text_image, drawRect.x + 5, drawRect.y + 5);
};

template <class coord_type>
const Button<coord_type> CreateTextButton(const std::string& text, const point_t<coord_type>& min_corner, const coord_type text_width, const coord_type text_height) noexcept {
    const int button_width = text_width + 10;
    const int button_height = text_height + 10;
    return {
        .text = text,
        .button_area = boundary_t<coord_type>{
            min_corner,
            point_t<coord_type>{
                min_corner.x() + button_width,
                min_corner.y() + button_height}
            },
        .is_pressed = false
    };
}

template <class coord_type>
void PlaceKeyboardButtons(WidgetArea2<coord_type, Button<coord_type>>& area) noexcept {
    for(char c = ' '; c < std::numeric_limits<char>::max(); c++) {
        const size_t idx = c - ' ';
        const point_t<coord_type> min = {
            50+100*(idx%15),50+100*(idx/15)
        };
        const char str[2] = {c, '\0'};
        area.emplace_back(CreateTextButton(str, min, 50, 50));
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

    const Window window = Create<Window, SDL_CreateWindow>("demo", 0, 0, 800, 600, SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(window.get() == nullptr){
        return __LINE__;
    }

    const auto renderer = Create<Renderer, SDL_CreateRenderer>(window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
    if(renderer.get() == nullptr){
        return __LINE__;
    }

    const auto scale = FitRenderOutput(renderer, window);
    if(!scale) {
        return __LINE__;
    }

    const Font JPFont = OpenFont("/usr/share/fonts/opentype/ipafont-mincho/ipam.ttf", 18);
    if(JPFont.get() == nullptr) {
        return __LINE__;
    }

    using SampleButton = Button<int>;
    using SampleArea = WidgetArea2<int, SampleButton>;
    int win_width = 0, win_height = 0;
    SDL_GetWindowSize(window.get(), &win_width, &win_height);
    SampleArea sampleArea{
        .widget_array = {},
        .area_boundary = boundary_t<int>{
            point_t<int>{0,0},
            point_t<int>{win_width,win_height}
        }
    };

    {
        constexpr char buttton_text[] = "hello world, こんにちは世界";
        const auto text_area_size = UTF8TextSize(JPFont, buttton_text);
        if (!text_area_size.has_value()) {
            return __LINE__;
        }
        for(int i = 0; i < 20; ++i) {
            const int button_width = text_area_size->first + 10;
            const int button_height = text_area_size->second + 10;
            const point_t<int> min_corner = point_t<int>{button_width,button_height + button_height * i};
            const SampleButton button = {
                .text = buttton_text,
                .button_area = boundary_t<int>{min_corner, point_t<int>{min_corner.x() + button_width,min_corner.y() + button_height}},
                .is_pressed = false
            };
            sampleArea.emplace_back(button);
        }
    }
    PlaceKeyboardButtons(sampleArea);

    using ctx_type = std::tuple<SampleArea &, bool &>;
    using ButtonDown = EventFunctor<SDL_MOUSEBUTTONDOWN, decltype([](SDL_Event&& e, ctx_type&& ctx){
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
    using ButtonUp = EventFunctor<SDL_MOUSEBUTTONUP, decltype([](SDL_Event&& e, ctx_type&& ctx){
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
    using KeyDown = EventFunctor<SDL_KEYDOWN, decltype([](SDL_Event&& e, ctx_type&& ctx){
            if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                std::get<bool&>(ctx) = false;
            }
        })>;

    for(bool isRunning=true; isRunning;) {
        for (SDL_Event e; SDL_PollEvent(&e);) {
            DispatchEvent<ctx_type,
                ButtonDown, ButtonUp, KeyDown
            >(std::forward<SDL_Event>(e), std::tie(sampleArea, isRunning));
        }

        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 100, 255);
        SDL_RenderClear(renderer.get());
        for(const auto& widget: sampleArea) {
            std::visit([&renderer, &JPFont]<class T>(const T& w) {
                if constexpr (std::is_same_v<T, SampleButton>) {
                    DrawButton(std::tie(renderer, JPFont), w);
                    return;
                }
            }, widget);
        }
        SDL_RenderPresent(renderer.get());
        SDL_Delay(1);
    }


    return 0;
}
