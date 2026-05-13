#ifndef HULK_BACKEND_NAME_MANGLER_H
#define HULK_BACKEND_NAME_MANGLER_H

#include <cstddef>
#include <string>

namespace Hulk::Backend {

class NameMangler {
public:
    std::string make_unique(const std::string& prefix, const std::string& hint);
    static std::string sanitize(const std::string& text);

private:
    std::size_t counter_ = 0;
};

} // namespace Hulk::Backend

#endif
