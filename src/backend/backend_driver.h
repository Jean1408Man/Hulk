#ifndef HULK_BACKEND_DRIVER_H
#define HULK_BACKEND_DRIVER_H

#include <string>

namespace Hulk::Backend {

struct BackendOptions {
    std::string input_path;
    std::string output_path;
    bool emit_cpp = false;
    bool keep_temp = false;
};

struct BackendResult {
    bool ok = false;
    std::string generated_cpp_path;
    std::string executable_path;
};

class BackendDriver {
public:
    BackendResult run(const BackendOptions& options);

private:
    std::string read_file(const std::string& path) const;
    std::string default_output_path(const BackendOptions& options) const;
    std::string temp_cpp_path() const;
    bool write_file(const std::string& path, const std::string& content) const;
    bool compile_cpp(const std::string& cpp_path, const std::string& output_path) const;
    std::string shell_quote(const std::string& value) const;
};

} // namespace Hulk::Backend

#endif
