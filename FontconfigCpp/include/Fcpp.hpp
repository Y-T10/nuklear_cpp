#pragma once

#include <type_traits>
#include <filesystem>
#include <string>
#include <map>

#include "fontconfig/fontconfig.h"
#include "boost/smart_ptr/intrusive_ptr.hpp"

void intrusive_ptr_add_ref(FcPattern *p);
void intrusive_ptr_release(FcPattern *p);
void intrusive_ptr_add_ref(FcConfig *p);
void intrusive_ptr_release(FcConfig *p);

namespace Fcpp {
    template<class T>
    using Fc_ptr = boost::intrusive_ptr<T>;

    using Config = Fc_ptr<FcConfig>;
    using Pattern = Fc_ptr<FcPattern>;

    template<auto Func, class ...Args>
    auto CreateFcPtr(Args ...args) noexcept {
        static_assert(std::is_invocable_v<decltype(Func), Args...>);
        using return_type = std::invoke_result_t<decltype(Func), Args...>;
        static_assert(std::is_pointer_v<return_type>);
        using return_type_raw = std::remove_pointer_t<return_type>;
        return Fc_ptr<return_type_raw>(Func(args...), false);
    }

    const std::filesystem::path SearchFont(const Config& conf, const Pattern& pattern) noexcept;
    const Config CurrentDefaultConfig() noexcept;
    const Pattern CreatePattern(const std::map<std::string, std::basic_string<FcChar8>>& param) noexcept;
}