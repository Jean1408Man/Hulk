#pragma once
#include <string>
#include "span.hpp"

namespace hulk::common {

enum class DiagnosticLevel {
    Lexical,
    Syntactic,
    Semantic,
};

enum class Severity {
    Error,
    Warning,
};

struct Diagnostic {
    DiagnosticLevel level = DiagnosticLevel::Lexical;
    Severity severity = Severity::Error;
    std::string message;
    Span span {};
};

}