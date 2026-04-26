#include "evaluator.h"
#include "../parser/parser.hpp"
#include "../parser/parser_driver.hpp"
#include "../common/diagnosticEngine.hpp"
#include "../common/diagnosticRepository.hpp"
#include "../lexer/lexer.hpp"
#include "../ast/others/program.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

static std::string read_file(const std::string& path) {
    std::ifstream input(path, std::ios::in | std::ios::binary);
    if (!input) throw std::runtime_error("No se pudo abrir: " + path);
    std::ostringstream buf;
    buf << input.rdbuf();
    return buf.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: hulk_eval <archivo.hulk>\n";
        return 1;
    }

    try {
        hulk::common::DiagnosticRepository repo;
        repo.load("lib/es_errors.json");

        hulk::common::DiagnosticEngine engine(repo);
        const std::string source = read_file(argv[1]);

        hulk::lexer::Lexer lexer(source, engine);
        hulk::parser::ParserDriver driver(lexer, engine);
        hulk::parser::Parser parser(driver);

        const int parse_rc = parser.parse();

        if (engine.has_errors() || parse_rc != 0) {
            engine.print_all();
            return 2;
        }

        // El resultado del parser es un ASTnode*; hacemos downcast a Program
        Hulk::ASTnode* root = driver.result();
        if (!root) { std::cerr << "Parseo vacío\n"; return 1; }

        auto* program = dynamic_cast<Hulk::Program*>(root);
        if (!program) { std::cerr << "El AST raíz no es un Program\n"; return 1; }

        Hulk::Evaluator ev;
        ev.run(*program);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
