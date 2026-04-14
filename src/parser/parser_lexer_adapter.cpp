#include "parser.hpp"

#include <exception>
#include <string>

#include "../common/span.hpp"
#include "../lexer/token_kind.hpp"
#include "parser_driver.hpp"

namespace {

hulk::parser::Parser::location_type to_location(const hulk::common::Span& span) {
    hulk::parser::Parser::location_type loc;
    loc.begin.line = static_cast<int>(span.start.line);
    loc.begin.column = static_cast<int>(span.start.column);
    loc.end.line = static_cast<int>(span.end.line);
    loc.end.column = static_cast<int>(span.end.column);
    return loc;
}

double parse_number_lexeme(const std::string& lexeme) {
    try {
        return std::stod(lexeme);
    } catch (const std::exception&) {
        return 0.0;
    }
}

} // namespace

namespace hulk::parser {

Parser::symbol_type yylex(ParserDriver& driver) {
    using TK = hulk::lexer::TokenKind;

    const auto token = driver.next_token();
    const auto loc = to_location(token.span);

    switch (token.kind) {
        case TK::EndOfFile: return Parser::make_END(loc);
        case TK::Error: return Parser::make_ERROR_TOKEN(token.lexeme, loc);

        case TK::Identifier: return Parser::make_IDENTIFIER(token.lexeme, loc);
        case TK::Number: return Parser::make_NUMBER_LITERAL(parse_number_lexeme(token.lexeme), loc);
        case TK::String: return Parser::make_STRING_LITERAL(token.lexeme, loc);

        case TK::True: return Parser::make_TRUE(loc);
        case TK::False: return Parser::make_FALSE(loc);

        case TK::Print: return Parser::make_PRINT(loc);
        case TK::Sqrt: return Parser::make_SQRT(loc);
        case TK::Sin: return Parser::make_SIN(loc);
        case TK::Cos: return Parser::make_COS(loc);
        case TK::Exp: return Parser::make_EXP(loc);
        case TK::Log: return Parser::make_LOG(loc);
        case TK::Rand: return Parser::make_RAND(loc);
        case TK::Pi: return Parser::make_PI_CONST(loc);
        case TK::E: return Parser::make_E_CONST(loc);

        case TK::Let: return Parser::make_LET(loc);
        case TK::In: return Parser::make_IN(loc);
        case TK::If: return Parser::make_IF(loc);
        case TK::Elif: return Parser::make_ELIF(loc);
        case TK::Else: return Parser::make_ELSE(loc);
        case TK::While: return Parser::make_WHILE(loc);
        case TK::For: return Parser::make_FOR(loc);
        case TK::Function: return Parser::make_FUNCTION(loc);
        case TK::Type: return Parser::make_TYPE(loc);
        case TK::Inherits: return Parser::make_INHERITS(loc);
        case TK::New: return Parser::make_NEW(loc);
        case TK::Is: return Parser::make_IS(loc);
        case TK::As: return Parser::make_AS(loc);

        case TK::Plus: return Parser::make_PLUS(loc);
        case TK::Minus: return Parser::make_MINUS(loc);
        case TK::Star: return Parser::make_STAR(loc);
        case TK::Slash: return Parser::make_SLASH(loc);
        case TK::Percent: return Parser::make_PERCENT(loc);
        case TK::Caret: return Parser::make_CARET(loc);
        case TK::Assign: return Parser::make_ASSIGN(loc);
        case TK::DestructiveAssign: return Parser::make_DESTRUCTIVE_ASSIGN(loc);
        case TK::EqualEqual: return Parser::make_EQUAL_EQUAL(loc);
        case TK::NotEqual: return Parser::make_NOT_EQUAL(loc);
        case TK::Less: return Parser::make_LESS(loc);
        case TK::LessEqual: return Parser::make_LESS_EQUAL(loc);
        case TK::Greater: return Parser::make_GREATER(loc);
        case TK::GreaterEqual: return Parser::make_GREATER_EQUAL(loc);
        case TK::And: return Parser::make_AND(loc);
        case TK::Or: return Parser::make_OR(loc);
        case TK::Not: return Parser::make_NOT(loc);
        case TK::Concat: return Parser::make_CONCAT(loc);
        case TK::DoubleConcat: return Parser::make_DOUBLECONCAT(loc);
        case TK::FatArrow: return Parser::make_FATARROW(loc);

        case TK::LParen: return Parser::make_LPAREN(loc);
        case TK::RParen: return Parser::make_RPAREN(loc);
        case TK::LBrace: return Parser::make_LBRACE(loc);
        case TK::RBrace: return Parser::make_RBRACE(loc);
        case TK::Comma: return Parser::make_COMMA(loc);
        case TK::Semicolon: return Parser::make_SEMICOLON(loc);
        case TK::Colon: return Parser::make_COLON(loc);
        case TK::Dot: return Parser::make_DOT(loc);
    }

    return Parser::make_ERROR_TOKEN("<unknown>", loc);
}

} // namespace hulk::parser
