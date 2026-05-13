#ifndef HULK_BACKEND_CODEGEN_CONTEXT_H
#define HULK_BACKEND_CODEGEN_CONTEXT_H

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Hulk::Backend {

class CodegenContext {
public:
    void push_scope();
    void pop_scope();
    void bind(const void* key, std::string cpp_name);
    std::optional<std::string> lookup(const void* key) const;

private:
    std::vector<std::unordered_map<const void*, std::string>> scopes_;
};

} // namespace Hulk::Backend

#endif
