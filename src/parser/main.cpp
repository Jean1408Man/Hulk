#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../ast/assignments/desctructiveAssign.h"
#include "../ast/binOps/arithmeticBinOp.h"
#include "../ast/binOps/logicBinOp.h"
#include "../ast/binOps/stringBinOp.h"
#include "../ast/domainFunctions/builtinCall.h"
#include "../ast/domainFunctions/print.h"
#include "../ast/literales/boolean.h"
#include "../ast/literales/number.h"
#include "../ast/literales/string.h"
#include "../ast/unaryOps/arithmeticUnaryOp.h"
#include "../ast/unaryOps/logicUnaryOp.h"
#include "../ast/variables/letIn.h"
#include "../ast/variables/variableBinding.h"
#include "../ast/variables/variableReference.h"
#include "../common/diagnosticEngine.hpp"
#include "../common/diagnosticRepository.hpp"
#include "../lexer/lexer.hpp"
#include "parser.hpp"
#include "parser_driver.hpp"

namespace {

std::string read_file(const std::string& path) {
    std::ifstream input(path, std::ios::in | std::ios::binary);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string indent(int depth) {
    return std::string(static_cast<std::size_t>(depth) * 2, ' ');
}

const char* arithmetic_op_name(Hulk::ArithmeticOp op) {
    switch (op) {
        case Hulk::ArithmeticOp::Plus: return "Plus";
        case Hulk::ArithmeticOp::Minus: return "Minus";
        case Hulk::ArithmeticOp::Mult: return "Mult";
        case Hulk::ArithmeticOp::Div: return "Div";
        case Hulk::ArithmeticOp::Pow: return "Pow";
    }
    return "UnknownArithmeticOp";
}

const char* logic_op_name(Hulk::LogicOp op) {
    switch (op) {
        case Hulk::LogicOp::And: return "And";
        case Hulk::LogicOp::Or: return "Or";
        case Hulk::LogicOp::Greater: return "Greater";
        case Hulk::LogicOp::Less: return "Less";
        case Hulk::LogicOp::Equal: return "Equal";
        case Hulk::LogicOp::NotEqual: return "NotEqual";
        case Hulk::LogicOp::GreaterEqual: return "GreaterEqual";
        case Hulk::LogicOp::LessEqual: return "LessEqual";
    }
    return "UnknownLogicOp";
}

const char* string_op_name(Hulk::StringOp op) {
    switch (op) {
        case Hulk::StringOp::Concat: return "Concat";
        case Hulk::StringOp::SpaceConcat: return "SpaceConcat";
    }
    return "UnknownStringOp";
}

const char* arith_unary_name(Hulk::ArithUnaryType op) {
    switch (op) {
        case Hulk::ArithUnaryType::Minus: return "Minus";
    }
    return "UnknownArithUnary";
}

const char* builtin_name(Hulk::BuiltinFunc func) {
    switch (func) {
        case Hulk::BuiltinFunc::Exp: return "Exp";
        case Hulk::BuiltinFunc::Log: return "Log";
        case Hulk::BuiltinFunc::Rand: return "Rand";
        case Hulk::BuiltinFunc::Range: return "Range";
    }
    return "UnknownBuiltin";
}

void dump_expr(const Hulk::Expr* expr, int depth = 0) {
    if (expr == nullptr) {
        std::cout << indent(depth) << "<null>\n";
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::Number*>(expr)) {
        std::cout << indent(depth) << "Number(value=" << node->GetValue() << ")\n";
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::String*>(expr)) {
        std::cout << indent(depth) << "String(value=\"" << node->GetValue() << "\")\n";
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::Boolean*>(expr)) {
        std::cout << indent(depth) << "Boolean(value=" << (node->GetValue() ? "true" : "false") << ")\n";
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::VariableReference*>(expr)) {
        std::cout << indent(depth) << "VariableReference(name=" << node->GetName() << ")\n";
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::VariableBinding*>(expr)) {
        std::cout << indent(depth) << "VariableBinding(name=" << node->GetName();
        if (node->HasTypeAnnotation()) {
            std::cout << ", type=" << node->GetTypeAnnotation();
        }
        std::cout << ")\n";
        dump_expr(node->GetInitializer(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::LetIn*>(expr)) {
        std::cout << indent(depth) << "LetIn\n";
        std::cout << indent(depth + 1) << "bindings\n";
        for (const auto& binding : node->GetBindings()) {
            dump_expr(binding.get(), depth + 2);
        }
        std::cout << indent(depth + 1) << "body\n";
        dump_expr(node->GetBody(), depth + 2);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::DestructiveAssign*>(expr)) {
        std::cout << indent(depth) << "DestructiveAssign(name=" << node->GetName() << ")\n";
        dump_expr(node->GetValue(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::ArithmeticBinOp*>(expr)) {
        std::cout << indent(depth) << "ArithmeticBinOp(op=" << arithmetic_op_name(node->GetOperator()) << ")\n";
        dump_expr(node->GetLeft(), depth + 1);
        dump_expr(node->GetRight(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::LogicBinOp*>(expr)) {
        std::cout << indent(depth) << "LogicBinOp(op=" << logic_op_name(node->GetOperator()) << ")\n";
        dump_expr(node->GetLeft(), depth + 1);
        dump_expr(node->GetRight(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::StringBinOp*>(expr)) {
        std::cout << indent(depth) << "StringBinOp(op=" << string_op_name(node->GetOperator()) << ")\n";
        dump_expr(node->GetLeft(), depth + 1);
        dump_expr(node->GetRight(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::ArithmeticUnaryOp*>(expr)) {
        std::cout << indent(depth) << "ArithmeticUnaryOp(op=" << arith_unary_name(node->GetType()) << ")\n";
        dump_expr(node->GetOperand(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::LogicUnaryOp*>(expr)) {
        std::cout << indent(depth) << "LogicUnaryOp\n";
        dump_expr(node->GetOperand(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::Print*>(expr)) {
        std::cout << indent(depth) << "Print\n";
        dump_expr(node->GetExpr(), depth + 1);
        return;
    }

    if (const auto* node = dynamic_cast<const Hulk::BuiltinCall*>(expr)) {
        std::cout << indent(depth) << "BuiltinCall(func=" << builtin_name(node->GetFunc()) << ")\n";
        for (const auto& arg : node->GetArgs()) {
            dump_expr(arg.get(), depth + 1);
        }
        return;
    }

    std::cout << indent(depth) << "UnknownExpr\n";
    std::cout << indent(depth + 1) << expr->ToString() << "\n";
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: ./hulk_parser_demo <archivo.hulk>\n";
        return 1;
    }

    try {
        hulk::common::DiagnosticRepository repo;
        if (!repo.load("lib/es_errors.json")) {
            std::cerr << "Advertencia: no se pudo cargar lib/es_errors.json.\n";
        }

        hulk::common::DiagnosticEngine engine(repo);
        const std::string source = read_file(argv[1]);

        hulk::lexer::Lexer lexer(source, engine);
        hulk::parser::ParserDriver driver(lexer, engine);
        hulk::parser::Parser parser(driver);

        const int parse_rc = parser.parse();

        if (driver.result() != nullptr) {
            std::cout << "=== AST ===\n";
            std::cout << driver.result()->ToString() << "\n";
            std::cout << "\n=== TREE ===\n";
            dump_expr(dynamic_cast<Hulk::Expr*>(driver.result()));
        } else {
            std::cout << "=== AST ===\n";
            std::cout << "<sin resultado>\n";
        }

        if (!engine.diagnostics().empty()) {
            std::cerr << "\n=== DIAGNOSTICS ===\n";
            engine.print_all();
        }

        if (parse_rc != 0 || engine.has_errors()) {
            return 2;
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fallo: " << ex.what() << "\n";
        return 1;
    }
}
