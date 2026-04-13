#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.hpp"
#include "token_kind.hpp"
#include "../common/diagnosticRepository.hpp"
#include "../common/diagnosticEngine.hpp"

using hulk::lexer::Lexer;
using hulk::lexer::TokenKind;
using hulk::common::DiagnosticRepository;
using hulk::common::DiagnosticEngine;

namespace {

const char* to_string(TokenKind kind) {
    switch (kind) {
        case TokenKind::EndOfFile: return "EndOfFile";
        case TokenKind::Error: return "Error";

        case TokenKind::Identifier: return "Identifier";
        case TokenKind::Number: return "Number";
        case TokenKind::String: return "String";

        case TokenKind::True: return "True";
        case TokenKind::False: return "False";

        case TokenKind::Print: return "Print";
        case TokenKind::Sqrt: return "Sqrt";
        case TokenKind::Sin: return "Sin";
        case TokenKind::Cos: return "Cos";
        case TokenKind::Exp: return "Exp";
        case TokenKind::Log: return "Log";
        case TokenKind::Rand: return "Rand";
        case TokenKind::Pi: return "Pi";
        case TokenKind::E: return "E";

        case TokenKind::Let: return "Let";
        case TokenKind::In: return "In";
        case TokenKind::If: return "If";
        case TokenKind::Elif: return "Elif";
        case TokenKind::Else: return "Else";
        case TokenKind::While: return "While";
        case TokenKind::For: return "For";
        case TokenKind::Function: return "Function";
        case TokenKind::Type: return "Type";
        case TokenKind::Inherits: return "Inherits";
        case TokenKind::New: return "New";
        case TokenKind::Is: return "Is";
        case TokenKind::As: return "As";

        case TokenKind::Plus: return "Plus";
        case TokenKind::Minus: return "Minus";
        case TokenKind::Star: return "Star";
        case TokenKind::Slash: return "Slash";
        case TokenKind::Percent: return "Percent";
        case TokenKind::Caret: return "Caret";
        case TokenKind::Assign: return "Assign";
        case TokenKind::DestructiveAssign: return "DestructiveAssign";
        case TokenKind::EqualEqual: return "EqualEqual";
        case TokenKind::NotEqual: return "NotEqual";
        case TokenKind::Less: return "Less";
        case TokenKind::LessEqual: return "LessEqual";
        case TokenKind::Greater: return "Greater";
        case TokenKind::GreaterEqual: return "GreaterEqual";
        case TokenKind::And: return "And";
        case TokenKind::Or: return "Or";
        case TokenKind::Not: return "Not";
        case TokenKind::Concat: return "Concat";
        case TokenKind::FatArrow: return "FatArrow";

        case TokenKind::LParen: return "LParen";
        case TokenKind::RParen: return "RParen";
        case TokenKind::LBrace: return "LBrace";
        case TokenKind::RBrace: return "RBrace";
        case TokenKind::Comma: return "Comma";
        case TokenKind::Semicolon: return "Semicolon";
        case TokenKind::Colon: return "Colon";
        case TokenKind::Dot: return "Dot";
    }

    return "UnknownTokenKind";
}

std::string read_file(const std::string& path) {
    std::ifstream input(path, std::ios::in | std::ios::binary);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void print_token(const hulk::lexer::Token& token) {
    std::cout
        << to_string(token.kind)
        << " | lexeme: `" << token.lexeme << "`"
        << " | span: ["
        << token.span.start.line << ":" << token.span.start.column
        << " - "
        << token.span.end.line << ":" << token.span.end.column
        << "]\n";
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: ./hulk_lexer <archivo.hulk>\n";
        return 1;
    }

    try {
        // 1. Cargar el repositorio de mensajes
        DiagnosticRepository repo;
        if (!repo.load("lib/es_errors.json")) {
            std::cerr << "Advertencia: no se pudo cargar lib/es_errors.json. "
                         "Los mensajes de error serán genéricos.\n";
        }

        // 2. Crear el engine (único punto de acumulación de errores)
        DiagnosticEngine engine(repo);

        // 3. Ejecutar el lexer
        const std::string source = read_file(argv[1]);
        Lexer lexer(source, engine);
        const auto tokens = lexer.tokenize();

        std::cout << "=== TOKENS ===\n";
        for (const auto& token : tokens) {
            print_token(token);
        }

        // 4. Reportar todos los diagnósticos acumulados
        if (!engine.diagnostics().empty()) {
            std::cerr << "\n=== DIAGNOSTICS ===\n";
            engine.print_all();
        }

        return engine.has_errors() ? 2 : 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fallo: " << ex.what() << "\n";
        return 1;
    }
}