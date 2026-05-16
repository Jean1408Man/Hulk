#ifndef HULK_BACKEND_CODEGEN_ERROR_H
#define HULK_BACKEND_CODEGEN_ERROR_H

#include <stdexcept>
#include <string>

namespace Hulk::Backend {

class CodegenError : public std::runtime_error {
public:
    explicit CodegenError(const std::string& msg) : std::runtime_error(msg) {}
};

} // namespace Hulk::Backend

#endif
