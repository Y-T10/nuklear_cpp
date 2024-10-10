#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdio>

#include <SDL2/SDL.h>
#include <memory>

#include "nuklear-cpp.hpp"
#include "nuklear_sdl_renderer.hpp"

template <class T, void (*deleter)(T*)>
using custom_unique = std::unique_ptr<T, decltype([](T* ptr){deleter(ptr);})>;

using Window = custom_unique<SDL_Window, SDL_DestroyWindow>;
using Renderer = custom_unique<SDL_Renderer, SDL_DestroyRenderer>;

template <class read_type = float>
const std::pair<read_type, read_type> FitRenderScale(const Window& window, const Renderer& renderer) {
    int render_w, render_h;
    int window_w, window_h;
    SDL_GetRendererOutputSize(renderer.get(), &render_w, &render_h);
    SDL_GetWindowSize(window.get(), &window_w, &window_h);

    const read_type scale_x = (read_type)(render_w) / (read_type)(window_w);
    const read_type scale_y = (read_type)(render_h) / (read_type)(window_h);
    SDL_RenderSetScale(renderer.get(), scale_x, scale_y);
    return {scale_x, scale_y};
}

int main(int argc, char* argv[]) {
    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO);

    auto window = Window(SDL_CreateWindow(
        "Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI)
    );

    if (!window) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        exit(-1);
    }

    auto renderer = Renderer(SDL_CreateRenderer(
        window.get(), -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
    );

    if (!renderer) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        exit(-1);
    }

    const auto [_, FontScale] = FitRenderScale(window, renderer);

    auto ctx = nk_sdl::init();

    {
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        /* set up the font atlas and add desired font; note that font sizes are
         * multiplied by font_scale to produce better results at higher DPIs */
        nk_sdl::font_stash_begin(ctx);
        font = nk_font_atlas_add_default(&(ctx->atlas), 13 * FontScale, &config);
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale, &config);*/
        nk_sdl::font_stash_end(ctx, renderer.get());

        /* this hack makes the font appear to be scaled down to the desired
         * size and is only necessary when font_scale > 1 */
        font->handle.height /= FontScale;
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        nk_style_set_font(&(ctx->context), &font->handle);
    }

    while (true)
    {
        /* Input */
        nk_input_begin(&(ctx->context));
        for (SDL_Event evt; SDL_PollEvent(&evt);) {
            if (evt.type == SDL_QUIT) goto cleanup;
            nk_sdl::handle_event(ctx, window.get(), &evt);
        }
        nk_input_end(&(ctx->context));

        nk_colorf bg = { .r = 0.10f, .g = 0.18f, .b = 0.24f, .a = 1.0f };

        /* GUI */
        if (nk_begin(&(ctx->context), "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(&(ctx->context), 30, 80, 1);
            if (nk_button_label(&(ctx->context), "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(&(ctx->context), 30, 2);
            if (nk_option_label(&(ctx->context), "easy", op == EASY)) op = EASY;
            if (nk_option_label(&(ctx->context), "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(&(ctx->context), 25, 1);
            nk_property_int(&(ctx->context), "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(&(ctx->context), 20, 1);
            nk_label(&(ctx->context), "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(&(ctx->context), 25, 1);
            if (nk_combo_begin_color(&(ctx->context), nk_rgb_cf(bg), nk_vec2(nk_widget_width(&(ctx->context)),400))) {
                nk_layout_row_dynamic(&(ctx->context), 120, 1);
                bg = nk_color_picker(&(ctx->context), bg, NK_RGBA);
                nk_layout_row_dynamic(&(ctx->context), 25, 1);
                bg.r = nk_propertyf(&(ctx->context), "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(&(ctx->context), "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(&(ctx->context), "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(&(ctx->context), "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(&(ctx->context));
            }
        }
        nk_end(&(ctx->context));

        /* -------------- EXAMPLES ---------------- */
        #ifdef INCLUDE_CALCULATOR
          calculator(ctx);
        #endif
        #ifdef INCLUDE_CANVAS
        canvas(ctx);
        #endif
        #ifdef INCLUDE_OVERVIEW
          overview(ctx);
        #endif
        #ifdef INCLUDE_NODE_EDITOR
          node_editor(ctx);
        #endif
        /* ----------------------------------------- */

        SDL_SetRenderDrawColor(renderer.get(), bg.r * 255, bg.g * 255, bg.b * 255, bg.a * 255);
        SDL_RenderClear(renderer.get());

        nk_sdl::render(ctx, renderer.get(), NK_ANTI_ALIASING_ON);

        SDL_RenderPresent(renderer.get());
    }

cleanup:
    nk_sdl::shutdown(ctx);
    SDL_Quit();
    return 0;
}
