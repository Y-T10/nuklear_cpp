#pragma once

#include <memory>
#include <string>
#include <optional>

#include "SDLCpp_video.hpp"
#include "SDL_ttf.h"

namespace SDL2Cpp::TTF {
    using Font = std::unique_ptr<TTF_Font, SDL2Cpp::SDL_deleter<TTF_Font, TTF_CloseFont>>;

    const Font OpenFont(const std::string& file, int point_size = 12) noexcept;

    const std::optional<std::pair<int, int>> UTF8TextSize(const Font& font, const std::string text) noexcept;

    const SDL2Cpp::Surface WriteText(const Font& font, const std::string& text, const SDL_Color& fg, const SDL_Color& bg) noexcept;
}