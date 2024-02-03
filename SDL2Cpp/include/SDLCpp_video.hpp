#pragma once

#include <optional>
#include <type_traits>
#include <utility>
#include <memory>

#include "SDL2/SDL_render.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"

namespace SDL2Cpp {
    /// 本来は ``decltype([](T*ptr){deleter(ptr)})`` としたかった．
    /// しかし2024/1/22時点では、g++がこれをうまく処理できない．
    /// そのためこの構造体を定義した
    template<class T, void(*deleter)(T*)>
    struct SDL_deleter {
        void operator()(T* ptr) {
            deleter(ptr);
        };
    };

    template<class T, auto deleter>
    using SDL_ptr = std::unique_ptr<T, SDL_deleter<T, deleter>>;

    using Window = SDL_ptr<SDL_Window, SDL_DestroyWindow>;
    using Renderer = SDL_ptr<SDL_Renderer, SDL_DestroyRenderer>;
    using Surface = SDL_ptr<SDL_Surface, SDL_FreeSurface>;
    using Texture = SDL_ptr<SDL_Texture, SDL_DestroyTexture>;

    int hello() noexcept;

    template<class ptr_type, auto Func, class ...Args>
    const ptr_type Create(Args&& ...args) noexcept {
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

    const std::optional<std::pair<float,float>> FitRenderOutput(const Renderer& renderer, const Window& window);
    void RenderSurface(const Renderer& renderer, const Surface& surface, const int x, const int y) noexcept;
}
