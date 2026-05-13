#include "backend_driver.h"

#include "codegen_cpp.h"

#include "../ast/others/program.h"
#include "../common/diagnosticEngine.hpp"
#include "../common/diagnosticRepository.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../parser/parser_driver.hpp"
#include "../semantic/analyzer.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Hulk::Backend {

BackendResult BackendDriver::run(const BackendOptions& options) {
    BackendResult result;

    try {
        hulk::common::DiagnosticRepository repo;
        repo.load("lib/es_errors.json");
        hulk::common::DiagnosticEngine engine(repo);

        const std::string source = read_file(options.input_path);
        hulk::lexer::Lexer lexer(source, engine);
        hulk::parser::ParserDriver parser_driver(lexer, engine);
        hulk::parser::Parser parser(parser_driver);

        const int parse_rc = parser.parse();
        if (engine.has_errors() || parse_rc != 0) {
            engine.print_all();
            return result;
        }

        Hulk::ASTnode* root = parser_driver.result();
        if (!root) {
            std::cerr << "Backend: parseo vacio.\n";
            return result;
        }

        auto* program = dynamic_cast<Hulk::Program*>(root);
        if (!program) {
            std::cerr << "Backend: el AST raiz no es Program.\n";
            return result;
        }

        Hulk::SemanticAnalyzer sem(engine);
        const bool sem_ok = sem.analyze(*program);
        bool has_blocking_error = false;
        for (const auto& diagnostic : engine.diagnostics()) {
            if (diagnostic.severity != hulk::common::Severity::Error) continue;
            if (diagnostic.message == "Los atributos son privados. Solo se pueden acceder mediante 'self'.") {
                continue;
            }
            if (diagnostic.message.find("'is' no plausible:") != std::string::npos) {
                continue;
            }
            has_blocking_error = true;
            break;
        }
        if ((!sem_ok || engine.has_errors()) && has_blocking_error) {
            engine.print_all();
            return result;
        }

        CppCodegen codegen(sem.tables(), sem.resolution_map(), sem.type_map());
        const std::string cpp = codegen.generate(*program);

        result.generated_cpp_path = options.emit_cpp ? default_output_path(options) : temp_cpp_path();
        if (!write_file(result.generated_cpp_path, cpp)) {
            std::cerr << "Backend: no se pudo escribir " << result.generated_cpp_path << "\n";
            return result;
        }

        if (options.emit_cpp) {
            result.ok = true;
            return result;
        }

        result.executable_path = default_output_path(options);
        if (!compile_cpp(result.generated_cpp_path, result.executable_path)) {
            return result;
        }

        if (!options.keep_temp) {
            std::error_code ec;
            std::filesystem::remove(result.generated_cpp_path, ec);
        }

        result.ok = true;
        return result;
    } catch (const CodegenError& err) {
        std::cerr << err.what() << "\n";
        return result;
    } catch (const std::exception& err) {
        std::cerr << "Backend: error inesperado: " << err.what() << "\n";
        return result;
    }
}

std::string BackendDriver::read_file(const std::string& path) const {
    std::ifstream input(path, std::ios::in | std::ios::binary);
    if (!input) throw std::runtime_error("No se pudo abrir: " + path);
    std::ostringstream buf;
    buf << input.rdbuf();
    return buf.str();
}

std::string BackendDriver::default_output_path(const BackendOptions& options) const {
    if (!options.output_path.empty()) {
        return options.output_path;
    }
    if (options.emit_cpp) {
        return options.input_path + ".cpp";
    }
    return "a.out";
}

std::string BackendDriver::temp_cpp_path() const {
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::filesystem::path path = std::filesystem::temp_directory_path();
    path /= "hulk_backend_" + std::to_string(now) + ".cpp";
    return path.string();
}

bool BackendDriver::write_file(const std::string& path, const std::string& content) const {
    std::ofstream output(path, std::ios::out | std::ios::binary);
    if (!output) return false;
    output << content;
    return static_cast<bool>(output);
}

bool BackendDriver::compile_cpp(const std::string& cpp_path, const std::string& output_path) const {
    const std::string cmd = "g++ -std=c++20 -Isrc " + shell_quote(cpp_path) +
                            " -o " + shell_quote(output_path);
    const int rc = std::system(cmd.c_str());
    if (rc != 0) {
        std::cerr << "Backend: fallo la compilacion C++.\n";
        return false;
    }
    return true;
}

std::string BackendDriver::shell_quote(const std::string& value) const {
    std::string out = "'";
    for (char ch : value) {
        if (ch == '\'') {
            out += "'\\''";
        } else {
            out.push_back(ch);
        }
    }
    out.push_back('\'');
    return out;
}

} // namespace Hulk::Backend
