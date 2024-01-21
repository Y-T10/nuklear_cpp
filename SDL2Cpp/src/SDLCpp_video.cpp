#include "SDLCpp_video.hpp"

#include <optional>
#include <memory>

#include "SDL2/SDL_render.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"

void SDL2Cpp::RenderSurface(const Renderer& renderer, const Surface& surface, const int x, const int y) noexcept {
    if(surface.get() == nullptr) {
        return;
    }
    const auto texture = Texture(SDL_CreateTextureFromSurface(renderer.get(), surface.get()));
    if(texture.get() == nullptr) {
        return;
    }
    const SDL_Rect src_area = {.x = 0, .y = 0, .w = surface->w, .h = surface->h};
    const SDL_Rect dst_area = {.x = x, .y = y, .w = surface->w, .h = surface->h};
    SDL_RenderCopy(renderer.get(), texture.get(), &src_area, &dst_area);
}

const std::optional<std::pair<float,float>> SDL2Cpp::FitRenderOutput(const Renderer& renderer, const Window& window) {
    int render_w = 0, render_h = 0;
    int window_w = 0, window_h = 0;
    SDL_GetWindowSize(window.get(), &window_w, &window_h);
    if(SDL_GetRendererOutputSize(renderer.get(), &render_w, &render_h) < 0) {
        return std::nullopt;
    }
    const float scale_x = (double)(render_w) / (double)(window_w);
    const float scale_y = (double)(render_h) / (double)(window_h);
    return
        SDL_RenderSetScale(renderer.get(), scale_x, scale_y) == 0?
        std::make_optional(std::pair<float, float>{scale_x, scale_y}):
        std::nullopt;
}
