#pragma once

#include <memory>
#include <string>
#include <optional>

#include "SDLCpp_video.hpp"
#include "SDL_ttf.h"

namespace SDL2Cpp::TTF {
    using Font = std::unique_ptr<TTF_Font, decltype([](TTF_Font* ptr){TTF_CloseFont(ptr);})>;

    inline const Font OpenFont(const std::string& file, int point_size = 12) noexcept {
        return Font(TTF_OpenFont(file.c_str(), point_size));
    }

    inline const std::optional<std::pair<int, int>> UTF8TextSize(const Font& font, const std::string text) noexcept {
        if(!font.get()) {
            return std::nullopt;
        }
        int w = 0, h = 0;
        if(TTF_SizeUTF8(font.get(), text.c_str(), &w, &h) == 0) {
            return std::make_optional(std::make_pair(w, h));
        }
        return std::nullopt;
    }

    inline const SDL2Cpp::Surface WriteText(const Font& font, const std::string& text, const SDL_Color& fg, const SDL_Color& bg) noexcept {
        return SDL2Cpp::Surface(TTF_RenderUTF8_LCD(font.get(), text.c_str(), fg, bg));
    }
}