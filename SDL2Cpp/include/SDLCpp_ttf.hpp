#pragma once

#include "SDLCpp.hpp"
#include "SDL_ttf.h"

namespace SDL2Cpp::TTF {
    using Font = std::unique_ptr<TTF_Font, decltype([](TTF_Font* ptr){TTF_CloseFont(ptr);})>;

    const Font OpenFont(const std::string& file, int point_size = 12) noexcept;

    const std::optional<std::pair<int, int>> UTF8TextSize(const Font& font, const std::string text) noexcept;

    const Surface WriteText(const Font& font, const std::string& text, const SDL_Color& fg, const SDL_Color& bg) noexcept;
}
