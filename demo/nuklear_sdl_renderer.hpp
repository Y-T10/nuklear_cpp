/*
 * Nuklear - 4.9.4 - public domain
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#pragma once

#define NK_IMPLEMENTATION
#include "nuklear-cpp.hpp"
#include <SDL2/SDL.h>
#include <memory>

namespace nk_sdl {
    struct device {
        nk_buffer cmds;
        nk_draw_null_texture tex_null;
        SDL_Texture *font_tex;
    };

    struct context {
        device device;
        nk_context context;
        nk_font_atlas atlas;
    };

    std::unique_ptr<context> init() noexcept;
    void font_stash_begin(std::unique_ptr<context>& ctx) noexcept;
    void font_stash_end(std::unique_ptr<context>& ctx, SDL_Renderer* renderer) noexcept;
    int  handle_event(std::unique_ptr<context>& ctx, SDL_Window* window, SDL_Event *evt) noexcept;
    void render(std::unique_ptr<nk_sdl::context>& ctx, SDL_Renderer* renderer, enum nk_anti_aliasing AA) noexcept;
    void shutdown(std::unique_ptr<context>& ctx) noexcept;
}
