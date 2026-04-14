#include "parser_driver.hpp"

namespace hulk::parser {

ParserDriver::ParserDriver(hulk::lexer::Lexer& lexer,
                           hulk::common::DiagnosticEngine& engine)
    : lexer_(lexer), engine_(engine) {}

hulk::lexer::Token ParserDriver::next_token() {
    last_token_ = lexer_.next_token();
    return last_token_;
}

void ParserDriver::report_syntax_error(const std::string& message) {
    report_syntax_error(message, last_token_.span);
}

void ParserDriver::report_syntax_error(const std::string& message,
                                       const hulk::common::Span& span) {
    engine_.report_raw(hulk::common::DiagnosticLevel::Syntactic,
                       hulk::common::Severity::Error,
                       span,
                       message);
}

void ParserDriver::set_result(std::unique_ptr<Hulk::ASTnode> root) {
    result_ = std::move(root);
}

Hulk::ASTnode* ParserDriver::result() const {
    return result_.get();
}

std::unique_ptr<Hulk::ASTnode> ParserDriver::take_result() {
    return std::move(result_);
}

} // namespace hulk::parser
