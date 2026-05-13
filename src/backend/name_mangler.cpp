#include "name_mangler.h"

#include <cctype>

namespace Hulk::Backend {

std::string NameMangler::sanitize(const std::string& text) {
    std::string out;
    out.reserve(text.size());
    for (unsigned char ch : text) {
        if (std::isalnum(ch) || ch == '_') {
            out.push_back(static_cast<char>(ch));
        } else {
            out.push_back('_');
        }
    }
    if (out.empty()) out = "anon";
    if (std::isdigit(static_cast<unsigned char>(out.front()))) {
        out.insert(out.begin(), '_');
    }
    return out;
}

std::string NameMangler::make_unique(const std::string& prefix, const std::string& hint) {
    return prefix + "_" + sanitize(hint) + "_" + std::to_string(counter_++);
}

} // namespace Hulk::Backend
