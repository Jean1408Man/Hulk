#pragma once

#include <string>
#include <vector>
#include "cursor.hpp"
#include "token.hpp"
#include "../common/diagnosticEngine.hpp"

namespace hulk::lexer {

class Lexer {
public:
    // El engine se pasa por referencia: el Lexer acumula diagnósticos en él.
    // El llamador es responsable de la vida del engine (y su repositorio).
    explicit Lexer(std::string source, hulk::common::DiagnosticEngine& engine);

    Token next_token();
    std::vector<Token> tokenize();

    [[nodiscard]] const std::vector<hulk::common::Diagnostic>& diagnostics() const {
        return engine_.diagnostics();
    }

private:
    Cursor cursor_;
    hulk::common::DiagnosticEngine& engine_;

    void skip_whitespace_and_comments();

    Token scan_identifier_or_keyword();
    Token scan_number();
    Token scan_string();
    Token scan_operator_or_delimiter();

    Token make_token(TokenKind kind, const std::string& lexeme,
                     hulk::common::Position start,
                     hulk::common::Position end);

    // error_id: clave en es_errors.json (ej. "INVALID_CHAR")
    // lexeme:   texto que causó el error (se incluye en el token devuelto)
    Token error_token(const std::string& error_id,
                      hulk::common::Position start,
                      hulk::common::Position end,
                      const std::string& lexeme);
};

}