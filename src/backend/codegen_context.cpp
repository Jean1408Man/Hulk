#include "codegen_context.h"

namespace Hulk::Backend {

void CodegenContext::push_scope() {
    scopes_.emplace_back();
}

void CodegenContext::pop_scope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

void CodegenContext::bind(const void* key, std::string cpp_name) {
    if (scopes_.empty()) {
        push_scope();
    }
    scopes_.back()[key] = std::move(cpp_name);
}

std::optional<std::string> CodegenContext::lookup(const void* key) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(key);
        if (found != it->end()) {
            return found->second;
        }
    }
    return std::nullopt;
}

} // namespace Hulk::Backend
