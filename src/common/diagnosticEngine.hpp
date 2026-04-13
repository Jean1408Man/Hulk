#pragma once
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include "diagnostic.hpp"
#include "diagnosticRepository.hpp"

namespace hulk::common {

class DiagnosticEngine {
private:
    const DiagnosticRepository& repo_;
    std::vector<Diagnostic> diagnostics_;

public:
    explicit DiagnosticEngine(const DiagnosticRepository& repo)
        : repo_(repo) {}

    template <typename... Args>
    void report(const std::string& error_id,
                DiagnosticLevel level,
                Severity severity,
                Span span,
                Args&&... args) {
        const std::string tmpl = repo_.get_template(error_id);
        std::string final_msg;
        
        if constexpr (sizeof...(args) > 0) {
            if (tmpl.find("{}") != std::string::npos) {
                final_msg = std::vformat(tmpl, std::make_format_args(args...));
            } else {
                final_msg = tmpl;
            }
        } else {
            final_msg = tmpl;
        }
        diagnostics_.push_back(Diagnostic {
            .level    = level,
            .severity = severity,
            .message  = std::move(final_msg),
            .span     = span,
        });
    }

    void report_raw(DiagnosticLevel level,
                    Severity severity,
                    Span span,
                    std::string message) {
        diagnostics_.push_back(Diagnostic {
            .level    = level,
            .severity = severity,
            .message  = std::move(message),
            .span     = span,
        });
    }

    [[nodiscard]] const std::vector<Diagnostic>& diagnostics() const {
        return diagnostics_;
    }

    [[nodiscard]] bool has_errors() const {
        for (const auto& d : diagnostics_) {
            if (d.severity == Severity::Error) return true;
        }
        return false;
    }

    void clear() { diagnostics_.clear(); }

    void print_all() const {
        for (const auto& d : diagnostics_) {
            const char* prefix = (d.severity == Severity::Error) ? "error" : "warning";
            std::cerr
                << prefix
                << " ["
                << d.span.start.line << ":" << d.span.start.column
                << " - "
                << d.span.end.line   << ":" << d.span.end.column
                << "]: "
                << d.message
                << "\n";
        }
    }

    void print(Severity filter) const {
        for (const auto& d : diagnostics_) {
            if (d.severity != filter) continue;
            const char* prefix = (d.severity == Severity::Error) ? "error" : "warning";
            std::cerr
                << prefix
                << " ["
                << d.span.start.line << ":" << d.span.start.column
                << " - "
                << d.span.end.line   << ":" << d.span.end.column
                << "]: "
                << d.message
                << "\n";
        }
    }
};

} // namespace hulk::common
