#pragma once

#include <memory>
#include <string>

#include "../common/diagnosticEngine.hpp"
#include "../common/span.hpp"
#include "../lexer/lexer.hpp"
#include "../lexer/token.hpp"
#include "../ast/abs_nodes/ast.h"

namespace hulk::parser {

class ParserDriver {
public:
    ParserDriver(hulk::lexer::Lexer& lexer,
                 hulk::common::DiagnosticEngine& engine);

    hulk::lexer::Token next_token();

    void report_syntax_error(const std::string& message);
    void report_syntax_error(const std::string& message,
                             const hulk::common::Span& span);

    void set_result(std::unique_ptr<Hulk::ASTnode> root);
    Hulk::ASTnode* result() const;
    std::unique_ptr<Hulk::ASTnode> take_result();

private:
    hulk::lexer::Lexer& lexer_;
    hulk::common::DiagnosticEngine& engine_;
    std::unique_ptr<Hulk::ASTnode> result_;
    hulk::lexer::Token last_token_ {};
};

} // namespace hulk::parser
