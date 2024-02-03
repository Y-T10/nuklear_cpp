#include "Fcpp.hpp"

void intrusive_ptr_add_ref(FcPattern *p) {
    FcPatternReference(p);
}
void intrusive_ptr_release(FcPattern *p) {
    FcPatternDestroy(p);
}
void intrusive_ptr_add_ref(FcConfig *p) {
    FcConfigReference(p);
}
void intrusive_ptr_release(FcConfig *p) {
    FcConfigDestroy(p);
}

namespace {
    using namespace Fcpp;

    const Pattern FontMatch(const Config& conf, const Pattern& pattern) noexcept {
        FcResult result;
        Pattern fontPattern = CreateFcPtr<FcFontMatch>(conf.get(), pattern.get(), &result);
        if(fontPattern.get() == nullptr) {
            return nullptr;
        }
        if(result != FcResult::FcResultMatch) {
            return nullptr;
        }
        return fontPattern;
    }
}

const std::filesystem::path Fcpp::SearchFont(const Config& conf, const Pattern& pattern) noexcept {
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

const Fcpp::Config Fcpp::CurrentDefaultConfig() noexcept {
    return Config(FcConfigGetCurrent());
}

const Fcpp::Pattern Fcpp::CreatePattern(const std::map<std::string, std::basic_string<FcChar8>>& param) noexcept {
    Pattern pattern = CreateFcPtr<FcPatternCreate>();
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