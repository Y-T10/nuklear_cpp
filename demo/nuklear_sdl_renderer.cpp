#include "nuklear_sdl_renderer.hpp"
#include "nuklear-cpp.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>
#include <strings.h>

struct nk_sdl_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
};

NK_INTERN void
nk_sdl_device_upload_atlas(std::unique_ptr<nk_sdl::context>& ctx, SDL_Renderer* renderer, const void *image, int width, int height)
{
    SDL_Texture *g_SDLFontTexture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
        width, height
    );
    if (g_SDLFontTexture == NULL) {
        SDL_Log("error creating texture");
        return;
    }
    SDL_UpdateTexture(g_SDLFontTexture, NULL, image, 4 * width);
    SDL_SetTextureBlendMode(g_SDLFontTexture, SDL_BLENDMODE_BLEND);
    ctx->device.font_tex = g_SDLFontTexture;
}

static void
nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    const char *text = SDL_GetClipboardText();
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

static void
nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len)
{
    char *str = 0;
    (void)usr;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    SDL_SetClipboardText(str);
    free(str);
}

namespace nk_sdl {
    std::unique_ptr<context> init() noexcept {
        auto ctx = std::make_unique<context>();

        nk_init_default(&(ctx->context), 0);
        ctx->context.clip = nk_clipboard{
            .userdata = nk_handle_ptr(0),
            .paste = nk_sdl_clipboard_paste,
            .copy = nk_sdl_clipboard_copy,
        };
        nk_buffer_init_default(&(ctx->device.cmds));
        return ctx;
    }

    void font_stash_begin(std::unique_ptr<context>& ctx) noexcept {
        nk_font_atlas_init_default(&(ctx->atlas));
        nk_font_atlas_begin(&(ctx->atlas));
    }

    void font_stash_end(std::unique_ptr<context>& ctx, SDL_Renderer* renderer) noexcept {
        const void *image; int w, h;
        image = nk_font_atlas_bake(&(ctx->atlas), &w, &h, NK_FONT_ATLAS_RGBA32);
        nk_sdl_device_upload_atlas(ctx, renderer, image, w, h);
        nk_font_atlas_end(&(ctx->atlas), nk_handle_ptr(ctx->device.font_tex), &(ctx->device.tex_null));
        if (ctx->atlas.default_font)
            nk_style_set_font(&(ctx->context), &(ctx->atlas.default_font->handle));
    }

    int handle_event(std::unique_ptr<context>& ctx, SDL_Window* window, SDL_Event *evt) noexcept {
        /* optional grabbing behavior */
        if (ctx->context.input.mouse.grab) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            ctx->context.input.mouse.grab = 0;
        } else if (ctx->context.input.mouse.ungrab) {
            const int x = (int)ctx->context.input.mouse.prev.x;
            const int y = (int)ctx->context.input.mouse.prev.y;
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_WarpMouseInWindow(window, x, y);
            ctx->context.input.mouse.ungrab = 0;
        }

        switch(evt->type)
        {
            case SDL_KEYUP: /* KEYUP & KEYDOWN share same routine */
            case SDL_KEYDOWN:
                {
                    int down = evt->type == SDL_KEYDOWN;
                    const Uint8* state = SDL_GetKeyboardState(0);
                    switch(evt->key.keysym.sym)
                    {
                        case SDLK_RSHIFT: /* RSHIFT & LSHIFT share same routine */
                        case SDLK_LSHIFT:    nk_input_key(&(ctx->context), NK_KEY_SHIFT, down); break;
                        case SDLK_DELETE:    nk_input_key(&(ctx->context), NK_KEY_DEL, down); break;
                        case SDLK_RETURN:    nk_input_key(&(ctx->context), NK_KEY_ENTER, down); break;
                        case SDLK_TAB:       nk_input_key(&(ctx->context), NK_KEY_TAB, down); break;
                        case SDLK_BACKSPACE: nk_input_key(&(ctx->context), NK_KEY_BACKSPACE, down); break;
                        case SDLK_HOME:      nk_input_key(&(ctx->context), NK_KEY_TEXT_START, down);
                                             nk_input_key(&(ctx->context), NK_KEY_SCROLL_START, down); break;
                        case SDLK_END:       nk_input_key(&(ctx->context), NK_KEY_TEXT_END, down);
                                             nk_input_key(&(ctx->context), NK_KEY_SCROLL_END, down); break;
                        case SDLK_PAGEDOWN:  nk_input_key(&(ctx->context), NK_KEY_SCROLL_DOWN, down); break;
                        case SDLK_PAGEUP:    nk_input_key(&(ctx->context), NK_KEY_SCROLL_UP, down); break;
                        case SDLK_z:         nk_input_key(&(ctx->context), NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_r:         nk_input_key(&(ctx->context), NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_c:         nk_input_key(&(ctx->context), NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_v:         nk_input_key(&(ctx->context), NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_x:         nk_input_key(&(ctx->context), NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_b:         nk_input_key(&(ctx->context), NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_e:         nk_input_key(&(ctx->context), NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]); break;
                        case SDLK_UP:        nk_input_key(&(ctx->context), NK_KEY_UP, down); break;
                        case SDLK_DOWN:      nk_input_key(&(ctx->context), NK_KEY_DOWN, down); break;
                        case SDLK_LEFT:
                            if (state[SDL_SCANCODE_LCTRL])
                                nk_input_key(&(ctx->context), NK_KEY_TEXT_WORD_LEFT, down);
                            else
                                nk_input_key(&(ctx->context), NK_KEY_LEFT, down);
                            break;
                        case SDLK_RIGHT:
                            if (state[SDL_SCANCODE_LCTRL])
                                nk_input_key(&(ctx->context), NK_KEY_TEXT_WORD_RIGHT, down);
                            else
                                nk_input_key(&(ctx->context), NK_KEY_RIGHT, down);
                            break;
                    }
                }
                return 1;

            case SDL_MOUSEBUTTONUP: /* MOUSEBUTTONUP & MOUSEBUTTONDOWN share same routine */
            case SDL_MOUSEBUTTONDOWN:
                {
                    int down = evt->type == SDL_MOUSEBUTTONDOWN;
                    const int x = evt->button.x, y = evt->button.y;
                    switch(evt->button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            if (evt->button.clicks > 1) {
                                nk_input_button(&(ctx->context), NK_BUTTON_DOUBLE, x, y, down);
                            }
                            nk_input_button(&(ctx->context), NK_BUTTON_LEFT, x, y, down); break;
                        case SDL_BUTTON_MIDDLE: nk_input_button(&(ctx->context), NK_BUTTON_MIDDLE, x, y, down); break;
                        case SDL_BUTTON_RIGHT:  nk_input_button(&(ctx->context), NK_BUTTON_RIGHT, x, y, down); break;
                    }
                }
                return 1;

            case SDL_MOUSEMOTION:
                if (ctx->context.input.mouse.grabbed) {
                    int x = (int)ctx->context.input.mouse.prev.x;
                    int y = (int)ctx->context.input.mouse.prev.y;
                    nk_input_motion(&(ctx->context), x + evt->motion.xrel, y + evt->motion.yrel);
                }
                else
                    nk_input_motion(&(ctx->context), evt->motion.x, evt->motion.y);
                return 1;

            case SDL_TEXTINPUT:
                {
                    nk_glyph glyph;
                    memcpy(glyph, evt->text.text, NK_UTF_SIZE);
                    nk_input_glyph(&(ctx->context), glyph);
                }
                return 1;

            case SDL_MOUSEWHEEL:
                nk_input_scroll(&(ctx->context),nk_vec2((float)evt->wheel.x,(float)evt->wheel.y));
                return 1;
        }
        return 0;
    }

    void render(std::unique_ptr<nk_sdl::context>& ctx, SDL_Renderer* renderer, enum nk_anti_aliasing AA) noexcept {
        SDL_Rect saved_clip;
    #ifdef NK_SDL_CLAMP_CLIP_RECT
        SDL_Rect viewport;
    #endif
        SDL_bool clipping_enabled;
        const int vs = sizeof(struct nk_sdl_vertex);
        const size_t vp = offsetof(struct nk_sdl_vertex, position);
        const size_t vt = offsetof(struct nk_sdl_vertex, uv);
        const size_t vc = offsetof(struct nk_sdl_vertex, col);

        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command *cmd;
        const nk_draw_index *offset = NULL;
        struct nk_buffer vbuf, ebuf;

        /* fill converting configuration */
        struct nk_convert_config config;
        static const struct nk_draw_vertex_layout_element vertex_layout[] = {
            {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, position)},
            {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, uv)},
            {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_sdl_vertex, col)},
            {NK_VERTEX_LAYOUT_END}
        };
        NK_MEMSET(&config, 0, sizeof(config));
        config.vertex_layout = vertex_layout;
        config.vertex_size = sizeof(struct nk_sdl_vertex);
        config.vertex_alignment = NK_ALIGNOF(struct nk_sdl_vertex);
        config.tex_null = ctx->device.tex_null;
        config.circle_segment_count = 22;
        config.curve_segment_count = 22;
        config.arc_segment_count = 22;
        config.global_alpha = 1.0f;
        config.shape_AA = AA;
        config.line_AA = AA;

        /* convert shapes into vertexes */
        nk_buffer_init_default(&vbuf);
        nk_buffer_init_default(&ebuf);
        nk_convert(&(ctx->context), &(ctx->device.cmds), &vbuf, &ebuf, &config);

        /* iterate over and execute each draw command */
        offset = (const nk_draw_index*)nk_buffer_memory_const(&ebuf);

        clipping_enabled = SDL_RenderIsClipEnabled(renderer);
        SDL_RenderGetClipRect(renderer, &saved_clip);
    #ifdef NK_SDL_CLAMP_CLIP_RECT
        SDL_RenderGetViewport(sdl.renderer, &viewport);
    #endif

        nk_draw_foreach(cmd, &(ctx->context), &(ctx->device.cmds))
        {
            if (!cmd->elem_count) continue;

            {
                SDL_Rect r;
                r.x = cmd->clip_rect.x;
                r.y = cmd->clip_rect.y;
                r.w = cmd->clip_rect.w;
                r.h = cmd->clip_rect.h;
    #ifdef NK_SDL_CLAMP_CLIP_RECT
                if (r.x < 0) {
                    r.w += r.x;
                    r.x = 0;
                }
                if (r.y < 0) {
                    r.h += r.y;
                    r.y = 0;
                }
                if (r.h > viewport.h) {
                    r.h = viewport.h;
                }
                if (r.w > viewport.w) {
                    r.w = viewport.w;
                }
    #endif
                SDL_RenderSetClipRect(renderer, &r);
            }

            {
                const void *vertices = nk_buffer_memory_const(&vbuf);

                SDL_RenderGeometryRaw(renderer,
                        (SDL_Texture *)cmd->texture.ptr,
                        (const float*)((const nk_byte*)vertices + vp), vs,
                        (const SDL_Color*)((const nk_byte*)vertices + vc), vs,
                        (const float*)((const nk_byte*)vertices + vt), vs,
                        (vbuf.needed / vs),
                        (void *) offset, cmd->elem_count, 2);

                offset += cmd->elem_count;
            }
        }

        SDL_RenderSetClipRect(renderer, &saved_clip);
        if (!clipping_enabled) {
            SDL_RenderSetClipRect(renderer, NULL);
        }

        nk_clear(&(ctx->context));
        nk_buffer_clear(&(ctx->device.cmds));
        nk_buffer_free(&vbuf);
        nk_buffer_free(&ebuf);
    }

    void shutdown(std::unique_ptr<context>& ctx) noexcept {
        nk_font_atlas_clear(&(ctx->atlas));
        nk_free(&(ctx->context));
        SDL_DestroyTexture(ctx->device.font_tex);
        /* glDeleteTextures(1, &dev->font_tex); */
        nk_buffer_free(&(ctx->device.cmds));
        memset(ctx.get(), 0, sizeof(context));
    }
}