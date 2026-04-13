#include "lexer.hpp"
#include <cctype>
#include <string_view>
#include "keywords.hpp"

namespace hulk::lexer {
namespace {

bool is_alpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool is_digit(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool is_alnum(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

}

Lexer::Lexer(std::string source, hulk::common::DiagnosticEngine& engine)
    : cursor_(std::move(source)), engine_(engine) {}

Token Lexer::next_token() {
    skip_whitespace_and_comments();

    const hulk::common::Position start = cursor_.position();

    if (cursor_.eof()) {
        return make_token(TokenKind::EndOfFile, "", start, start);
    }

    const char c = cursor_.peek();

    if (is_alpha(c)) {
        return scan_identifier_or_keyword();
    }

    if (is_digit(c)) {
        return scan_number();
    }

    if (c == '"') {
        return scan_string();
    }

    return scan_operator_or_delimiter();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        Token token = next_token();
        tokens.push_back(token);

        if (token.kind == TokenKind::EndOfFile) {
            break;
        }
    }

    return tokens;
}

void Lexer::skip_whitespace_and_comments() {
    while (!cursor_.eof()) {
        const char c = cursor_.peek();

        if (is_whitespace(c)) {
            cursor_.advance();
            continue;
        }

        if (cursor_.peek() == '/' && cursor_.peek(1) == '/') {
            while (!cursor_.eof() && cursor_.peek() != '\n') {
                cursor_.advance();
            }
            continue;
        }

        break;
    }
}

Token Lexer::scan_identifier_or_keyword() {
    const hulk::common::Position start = cursor_.position();
    std::string lexeme;

    lexeme += cursor_.advance();

    while (!cursor_.eof()) {
        const char c = cursor_.peek();
        if (is_alnum(c) || c == '_') {
            lexeme += cursor_.advance();
        } else {
            break;
        }
    }

    const hulk::common::Position end = cursor_.position();

    const auto it = kKeywords.find(std::string_view(lexeme));
    if (it != kKeywords.end()) {
        return make_token(it->second, lexeme, start, end);
    }

    return make_token(TokenKind::Identifier, lexeme, start, end);
}

Token Lexer::scan_number() {
    const hulk::common::Position start = cursor_.position();
    std::string lexeme;

    while (!cursor_.eof() && is_digit(cursor_.peek())) {
        lexeme += cursor_.advance();
    }

    // Parte decimal opcional: solo si '.' va seguida de dígito.
    if (cursor_.peek() == '.' && is_digit(cursor_.peek(1))) {
        lexeme += cursor_.advance(); // consume '.'

        while (!cursor_.eof() && is_digit(cursor_.peek())) {
            lexeme += cursor_.advance();
        }
    }

    const hulk::common::Position end = cursor_.position();
    return make_token(TokenKind::Number, lexeme, start, end);
}

Token Lexer::scan_string() {
    const hulk::common::Position start = cursor_.position();
    std::string lexeme;

    // Consume comilla inicial
    lexeme += cursor_.advance();

    while (!cursor_.eof()) {
        const char c = cursor_.peek();

        if (c == '\\') {
            lexeme += cursor_.advance();

            if (cursor_.eof()) {
                const hulk::common::Position end = cursor_.position();
                return error_token("INCOMPLETE_ESCAPE", start, end, lexeme);
            }

            lexeme += cursor_.advance();
            continue;
        }

        if (c == '"') {
            lexeme += cursor_.advance();
            const hulk::common::Position end = cursor_.position();
            return make_token(TokenKind::String, lexeme, start, end);
        }

        if (c == '\n') {
            const hulk::common::Position end = cursor_.position();
            return error_token("UNTERMINATED_STRING_EOL", start, end, lexeme);
        }

        lexeme += cursor_.advance();
    }

    const hulk::common::Position end = cursor_.position();
    return error_token("UNTERMINATED_STRING_EOF", start, end, lexeme);
}

Token Lexer::scan_operator_or_delimiter() {
    const hulk::common::Position start = cursor_.position();

    // Longest-match primero
    if (cursor_.match(":=")) {
        return make_token(TokenKind::DestructiveAssign, ":=", start, cursor_.position());
    }
    if (cursor_.match("==")) {
        return make_token(TokenKind::EqualEqual, "==", start, cursor_.position());
    }
    if (cursor_.match("!=")) {
        return make_token(TokenKind::NotEqual, "!=", start, cursor_.position());
    }
    if (cursor_.match("<=")) {
        return make_token(TokenKind::LessEqual, "<=", start, cursor_.position());
    }
    if (cursor_.match(">=")) {
        return make_token(TokenKind::GreaterEqual, ">=", start, cursor_.position());
    }
    if (cursor_.match("=>")) {
        return make_token(TokenKind::FatArrow, "=>", start, cursor_.position());
    }

    const char c = cursor_.advance();
    const hulk::common::Position end = cursor_.position();

    switch (c) {
        case '+': return make_token(TokenKind::Plus, "+", start, end);
        case '-': return make_token(TokenKind::Minus, "-", start, end);
        case '*': return make_token(TokenKind::Star, "*", start, end);
        case '/': return make_token(TokenKind::Slash, "/", start, end);
        case '%': return make_token(TokenKind::Percent, "%", start, end);
        case '^': return make_token(TokenKind::Caret, "^", start, end);

        case '=': return make_token(TokenKind::Assign, "=", start, end);
        case '<': return make_token(TokenKind::Less, "<", start, end);
        case '>': return make_token(TokenKind::Greater, ">", start, end);

        case '&': return make_token(TokenKind::And, "&", start, end);
        case '|': return make_token(TokenKind::Or, "|", start, end);
        case '!': return make_token(TokenKind::Not, "!", start, end);
        case '@': return make_token(TokenKind::Concat, "@", start, end);

        case '(': return make_token(TokenKind::LParen, "(", start, end);
        case ')': return make_token(TokenKind::RParen, ")", start, end);
        case '{': return make_token(TokenKind::LBrace, "{", start, end);
        case '}': return make_token(TokenKind::RBrace, "}", start, end);

        case ',': return make_token(TokenKind::Comma, ",", start, end);
        case ';': return make_token(TokenKind::Semicolon, ";", start, end);
        case ':': return make_token(TokenKind::Colon, ":", start, end);
        case '.': return make_token(TokenKind::Dot, ".", start, end);

        default:
            return error_token("INVALID_CHAR", start, end, std::string(1, c));
    }
}

Token Lexer::make_token(TokenKind kind,
                        const std::string& lexeme,
                        hulk::common::Position start,
                        hulk::common::Position end) {
    return Token {
        .kind = kind,
        .lexeme = lexeme,
        .span = hulk::common::Span {
            .start = start,
            .end = end,
        },
    };
}

Token Lexer::error_token(const std::string& error_id,
                         hulk::common::Position start,
                         hulk::common::Position end,
                         const std::string& lexeme) {
    const hulk::common::Span span { .start = start, .end = end };

    engine_.report(error_id,
                   hulk::common::DiagnosticLevel::Lexical,
                   hulk::common::Severity::Error,
                   span,
                   lexeme);

    return Token {
        .kind   = TokenKind::Error,
        .lexeme = lexeme,
        .span   = span,
    };
}

}