#include "SDLCpp_ttf.hpp"

namespace SDL2Cpp::TTF {
    const Font OpenFont(const std::string& file, int point_size) noexcept {
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

    const SDL2Cpp::Surface WriteText(const Font& font, const std::string& text, const SDL_Color& fg, const SDL_Color& bg) noexcept {
        return SDL2Cpp::Surface(TTF_RenderUTF8_LCD(font.get(), text.c_str(), fg, bg));
    }
}