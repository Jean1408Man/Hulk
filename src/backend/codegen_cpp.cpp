#include "codegen_cpp.h"

#include "../ast/assignments/desctructiveAssign.h"
#include "../ast/assignments/destructiveAssignMember.h"
#include "../ast/binOps/arithmeticBinOp.h"
#include "../ast/binOps/logicBinOp.h"
#include "../ast/binOps/stringBinOp.h"
#include "../ast/conditionals/ifStmt.h"
#include "../ast/domainFunctions/builtinCall.h"
#include "../ast/domainFunctions/print.h"
#include "../ast/functions/functionCall.h"
#include "../ast/functions/functionDecl.h"
#include "../ast/functions/lambda.h"
#include "../ast/literales/boolean.h"
#include "../ast/literales/number.h"
#include "../ast/literales/string.h"
#include "../ast/loops/for.h"
#include "../ast/loops/while.h"
#include "../ast/others/baseCall.h"
#include "../ast/others/exprBlock.h"
#include "../ast/others/group.h"
#include "../ast/others/program.h"
#include "../ast/others/selfRef.h"
#include "../ast/protocols/protocolDecl.h"
#include "../ast/types/asExpr.h"
#include "../ast/types/isExpr.h"
#include "../ast/types/memberAccess.h"
#include "../ast/types/methodCall.h"
#include "../ast/types/newExpr.h"
#include "../ast/types/typeDecl.h"
#include "../ast/types/typeMemberAttribute.h"
#include "../ast/types/typeMemberMethod.h"
#include "../ast/unaryOps/arithmeticUnaryOp.h"
#include "../ast/unaryOps/logicUnaryOp.h"
#include "../ast/variables/letIn.h"
#include "../ast/variables/variableBinding.h"
#include "../ast/variables/variableReference.h"

#include <iomanip>
#include <utility>

namespace Hulk::Backend {

CppCodegen::CppCodegen(const SemanticTables& tables,
                       const std::unordered_map<Expr*, ResolutionResult>& resolution_map,
                       const std::unordered_map<Expr*, HulkType>& type_map)
    : tables_(tables), resolution_map_(resolution_map), type_map_(type_map) {}

std::string CppCodegen::generate(Program& program) {
    collect_declarations(program);
    return emit_program(program);
}

void CppCodegen::collect_declarations(Program& program) {
    for (const auto& decl : program.GetDeclarations()) {
        if (auto* fn = dynamic_cast<FunctionDecl*>(decl.get())) {
            function_names_[fn] = mangler_.make_unique("hulk_fn", fn->GetName());
        } else if (auto* type = dynamic_cast<TypeDecl*>(decl.get())) {
            const std::string& type_name = type->GetName();
            type_decls_[type_name] = type;
            ctor_names_[type_name] = mangler_.make_unique("hulk_ctor", type_name);
            init_names_[type_name] = mangler_.make_unique("hulk_init", type_name);

            for (const auto& member : type->GetMembers()) {
                if (member.kind == TypeMember::Kind::Method) {
                    auto* method = static_cast<TypeMemberMethod*>(member.node.get());
                    method_names_[type_name][method->GetName()] =
                        mangler_.make_unique("hulk_method_" + NameMangler::sanitize(type_name),
                                             method->GetName());
                }
            }
        }
    }
}

std::string CppCodegen::emit_program(Program& program) {
    std::ostringstream out;
    out << "#include <backend/runtime/hulk_runtime.hpp>\n\n";
    out << emit_prototypes();

    for (const auto& decl : program.GetDeclarations()) {
        if (auto* fn = dynamic_cast<FunctionDecl*>(decl.get())) {
            out << emit_function(*fn) << "\n";
        }
    }

    for (const auto& decl : program.GetDeclarations()) {
        if (auto* type = dynamic_cast<TypeDecl*>(decl.get())) {
            out << emit_type(*type) << "\n";
        }
    }

    out << emit_register_types() << "\n";
    out << emit_main(program);
    return out.str();
}

std::string CppCodegen::emit_prototypes() {
    std::ostringstream out;
    for (const auto& [fn, name] : function_names_) {
        (void)fn;
        out << "hulk_rt::Value " << name
            << "(const std::vector<hulk_rt::Value>& args);\n";
    }
    for (const auto& [type_name, _] : type_decls_) {
        out << "void " << init_names_.at(type_name)
            << "(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args);\n";
        out << "hulk_rt::ObjectPtr " << ctor_names_.at(type_name)
            << "(const std::vector<hulk_rt::Value>& args);\n";
        for (const auto& [method_name, cpp_name] : method_names_.at(type_name)) {
            (void)method_name;
            out << "hulk_rt::Value " << cpp_name
                << "(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args);\n";
        }
    }
    out << "\n";
    return out.str();
}

std::string CppCodegen::emit_function(FunctionDecl& fn) {
    std::ostringstream out;
    out << "hulk_rt::Value " << function_names_.at(&fn)
        << "(const std::vector<hulk_rt::Value>& args) {\n";
    out << "    if (args.size() != " << fn.GetParams().size()
        << ") hulk_rt::fail(\"Runtime error: aridad incorrecta en funcion "
        << NameMangler::sanitize(fn.GetName()) << ".\");\n";

    context_.push_scope();
    for (std::size_t i = 0; i < fn.GetParams().size(); ++i) {
        const auto& param = fn.GetParams()[i];
        const std::string name = mangler_.make_unique("hulk_param", param.name);
        context_.bind(&param, name);
        out << "    hulk_rt::Value " << name << " = args[" << i << "];\n";
    }
    out << "    return " << emit_expr(fn.GetBody()) << ";\n";
    context_.pop_scope();
    out << "}\n";
    return out.str();
}

std::string CppCodegen::emit_type(TypeDecl& type) {
    std::ostringstream out;
    const std::string& type_name = type.GetName();

    out << "void " << init_names_.at(type_name)
        << "(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args) {\n";
    context_.push_scope();
    for (std::size_t i = 0; i < type.GetCtorParams().size(); ++i) {
        const auto& param = type.GetCtorParams()[i];
        const std::string name = mangler_.make_unique("hulk_ctor_param", param.name);
        context_.bind(&param, name);
        out << "    hulk_rt::Value " << name << " = args[" << i << "];\n";
    }

    if (type.HasParent()) {
        const std::string& parent_name = type.GetParentName();
        if (init_names_.count(parent_name)) {
            const std::string parent_args = new_temp("parent_args");
            out << "    std::vector<hulk_rt::Value> " << parent_args << ";\n";
            if (!type.GetParentArgs().empty()) {
                out << "    " << parent_args << ".reserve(" << type.GetParentArgs().size() << ");\n";
                for (const auto& arg : type.GetParentArgs()) {
                    out << "    " << parent_args << ".push_back(" << emit_expr(arg.get()) << ");\n";
                }
            } else if (!type.HasExplicitConstructor()) {
                out << "    " << parent_args << " = args;\n";
            }
            out << "    " << init_names_.at(parent_name) << "(self, " << parent_args << ");\n";
        }
    }

    for (const auto& member : type.GetMembers()) {
        if (member.kind != TypeMember::Kind::Attribute) continue;
        auto* attr = static_cast<TypeMemberAttribute*>(member.node.get());
        out << "    self->fields[" << cpp_string(attr->GetName()) << "] = "
            << emit_expr(attr->GetInitializer()) << ";\n";
    }
    context_.pop_scope();
    out << "}\n\n";

    out << "hulk_rt::ObjectPtr " << ctor_names_.at(type_name)
        << "(const std::vector<hulk_rt::Value>& args) {\n";
    out << "    auto self = std::make_shared<hulk_rt::Object>(" << cpp_string(type_name) << ");\n";
    out << "    " << init_names_.at(type_name) << "(self, args);\n";
    out << "    return self;\n";
    out << "}\n\n";

    for (const auto& member : type.GetMembers()) {
        if (member.kind != TypeMember::Kind::Method) continue;
        auto* method = static_cast<TypeMemberMethod*>(member.node.get());
        out << emit_method(type_name, *method) << "\n";
    }
    return out.str();
}

std::string CppCodegen::emit_method(const std::string& type_name, TypeMemberMethod& method) {
    std::ostringstream out;
    out << "hulk_rt::Value " << method_names_.at(type_name).at(method.GetName())
        << "(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args) {\n";
    out << "    if (args.size() != " << method.GetParams().size()
        << ") hulk_rt::fail(\"Runtime error: aridad incorrecta en metodo "
        << NameMangler::sanitize(method.GetName()) << ".\");\n";
    out << "    hulk_rt::Value hulk_self_value(self);\n";

    const std::string prev_type = current_type_name_;
    const std::string prev_method = current_method_name_;
    current_type_name_ = type_name;
    current_method_name_ = method.GetName();

    context_.push_scope();
    for (std::size_t i = 0; i < method.GetParams().size(); ++i) {
        const auto& param = method.GetParams()[i];
        const std::string name = mangler_.make_unique("hulk_method_param", param.name);
        context_.bind(&param, name);
        out << "    hulk_rt::Value " << name << " = args[" << i << "];\n";
    }
    out << "    return " << emit_expr(method.GetBody()) << ";\n";
    context_.pop_scope();

    current_type_name_ = prev_type;
    current_method_name_ = prev_method;
    out << "}\n";
    return out.str();
}

std::string CppCodegen::emit_register_types() {
    std::ostringstream out;
    out << "void hulk_register_types() {\n";
    for (const auto& [type_name, type] : type_decls_) {
        const std::string parent = type->HasParent() ? type->GetParentName() : "Object";
        out << "    hulk_rt::register_type(" << cpp_string(type_name)
            << ", " << cpp_string(parent) << ");\n";
    }
    for (const auto& [type_name, type] : type_decls_) {
        for (const auto& member : type->GetMembers()) {
            if (member.kind != TypeMember::Kind::Method) continue;
            auto* method = static_cast<TypeMemberMethod*>(member.node.get());
            out << "    hulk_rt::register_method(" << cpp_string(type_name)
                << ", " << cpp_string(method->GetName())
                << ", " << method_names_.at(type_name).at(method->GetName())
                << ", " << method->GetParams().size() << ");\n";
        }
    }
    out << "}\n";
    return out.str();
}

std::string CppCodegen::emit_main(Program& program) {
    std::ostringstream out;
    out << "int main() {\n";
    out << "    try {\n";
    out << "        hulk_register_types();\n";
    if (program.GetGlobalExpr()) {
        out << "        (void)" << emit_expr(program.GetGlobalExpr()) << ";\n";
    }
    out << "        return 0;\n";
    out << "    } catch (const hulk_rt::RuntimeError& err) {\n";
    out << "        std::cerr << err.what() << \"\\n\";\n";
    out << "        return 1;\n";
    out << "    }\n";
    out << "}\n";
    return out.str();
}

std::string CppCodegen::emit_expr(Expr* expr) {
    if (!expr) return "hulk_rt::Value()";
    expr->accept(*this);
    return expr_result_;
}

std::string CppCodegen::emit_args_vector(const std::vector<std::unique_ptr<Expr>>& args,
                                         const std::string& vec_name) {
    std::ostringstream out;
    out << "        std::vector<hulk_rt::Value> " << vec_name << ";\n";
    out << "        " << vec_name << ".reserve(" << args.size() << ");\n";
    for (const auto& arg : args) {
        out << "        " << vec_name << ".push_back(" << emit_expr(arg.get()) << ");\n";
    }
    return out.str();
}

std::string CppCodegen::emit_base_call(const std::vector<std::unique_ptr<Expr>>& args) {
    if (current_type_name_.empty() || current_method_name_.empty()) {
        unsupported("base() fuera de metodo");
    }
    const auto* type = tables_.lookup_type(current_type_name_);
    if (!type || type->parent_name.empty()) {
        unsupported("base() en tipo sin padre");
    }

    const std::string vec = new_temp("base_args");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << emit_args_vector(args, vec);
    out << "        return hulk_rt::call_method_from(hulk_self_value, "
        << cpp_string(type->parent_name) << ", " << cpp_string(current_method_name_)
        << ", " << vec << ");\n";
    out << "    }())";
    return out.str();
}

std::string CppCodegen::new_temp(const std::string& hint) {
    return mangler_.make_unique("hulk_tmp", hint);
}

std::string CppCodegen::cpp_string(const std::string& text) const {
    std::ostringstream out;
    out << '"';
    for (unsigned char ch : text) {
        switch (ch) {
            case '\\': out << "\\\\"; break;
            case '"': out << "\\\""; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (ch < 32 || ch > 126) {
                    out << "\\x" << std::hex << std::setw(2) << std::setfill('0')
                        << static_cast<int>(ch) << std::dec;
                } else {
                    out << static_cast<char>(ch);
                }
                break;
        }
    }
    out << '"';
    return out.str();
}

std::string CppCodegen::lookup_symbol(Expr& node, const std::string& fallback_name) {
    auto it = resolution_map_.find(&node);
    if (it == resolution_map_.end()) {
        if (fallback_name == "self" && !current_type_name_.empty()) return "hulk_self_value";
        throw CodegenError("Backend: referencia sin resolver '" + fallback_name + "'.");
    }

    const ResolutionResult& res = it->second;
    if (res.kind == ResolutionKind::Variable) {
        auto name = context_.lookup(res.binding);
        if (!name) throw CodegenError("Backend: binding sin nombre C++.");
        return *name;
    }
    if (res.kind == ResolutionKind::Param) {
        auto name = context_.lookup(res.param);
        if (!name) throw CodegenError("Backend: parametro sin nombre C++.");
        return *name;
    }
    if (res.kind == ResolutionKind::Synthetic) {
        if (res.synthetic->kind == SyntheticKind::Self) return "hulk_self_value";
        unsupported("for/range");
    }
    if (res.kind == ResolutionKind::BuiltinConstant) {
        if (res.builtin_const->name == "PI") return "hulk_rt::Value(3.14159265358979323846)";
        if (res.builtin_const->name == "E") return "hulk_rt::Value(2.71828182845904523536)";
    }

    throw CodegenError("Backend: simbolo no soportado en referencia '" + fallback_name + "'.");
}

[[noreturn]] void CppCodegen::unsupported(const std::string& feature) {
    throw CodegenError("Backend no soporta todavia: " + feature);
}

void CppCodegen::visit(Number& node) {
    std::ostringstream out;
    out << std::setprecision(17) << node.GetValue();
    std::string literal = out.str();
    if (literal.find('.') == std::string::npos &&
        literal.find('e') == std::string::npos &&
        literal.find('E') == std::string::npos) {
        literal += ".0";
    }
    expr_result_ = "hulk_rt::Value(" + literal + ")";
}

void CppCodegen::visit(String& node) {
    expr_result_ = "hulk_rt::Value(std::string(" + cpp_string(node.GetValue()) + "))";
}

void CppCodegen::visit(Boolean& node) {
    expr_result_ = std::string("hulk_rt::Value(") + (node.GetValue() ? "true" : "false") + ")";
}

void CppCodegen::visit(ArithmeticBinOp& node) {
    const char* fn = "hulk_rt::add";
    switch (node.GetOperator()) {
        case ArithmeticOp::Plus: fn = "hulk_rt::add"; break;
        case ArithmeticOp::Minus: fn = "hulk_rt::sub"; break;
        case ArithmeticOp::Mult: fn = "hulk_rt::mul"; break;
        case ArithmeticOp::Div: fn = "hulk_rt::div"; break;
        case ArithmeticOp::Mod: fn = "hulk_rt::mod"; break;
        case ArithmeticOp::Pow: fn = "hulk_rt::pow"; break;
    }
    expr_result_ = std::string(fn) + "(" + emit_expr(node.GetLeft()) + ", " + emit_expr(node.GetRight()) + ")";
}

void CppCodegen::visit(LogicBinOp& node) {
    if (node.GetOperator() == LogicOp::And || node.GetOperator() == LogicOp::Or) {
        const std::string left = new_temp("logic_left");
        std::ostringstream out;
        out << "([&]() -> hulk_rt::Value {\n";
        out << "        hulk_rt::Value " << left << " = " << emit_expr(node.GetLeft()) << ";\n";
        if (node.GetOperator() == LogicOp::And) {
            out << "        if (!hulk_rt::truthy(" << left << ")) return hulk_rt::Value(false);\n";
            out << "        return hulk_rt::Value(hulk_rt::truthy(" << emit_expr(node.GetRight()) << "));\n";
        } else {
            out << "        if (hulk_rt::truthy(" << left << ")) return hulk_rt::Value(true);\n";
            out << "        return hulk_rt::Value(hulk_rt::truthy(" << emit_expr(node.GetRight()) << "));\n";
        }
        out << "    }())";
        expr_result_ = out.str();
        return;
    }

    const char* fn = "hulk_rt::equal";
    switch (node.GetOperator()) {
        case LogicOp::Equal: fn = "hulk_rt::equal"; break;
        case LogicOp::NotEqual: fn = "hulk_rt::not_equal"; break;
        case LogicOp::Greater: fn = "hulk_rt::greater"; break;
        case LogicOp::Less: fn = "hulk_rt::less"; break;
        case LogicOp::GreaterEqual: fn = "hulk_rt::greater_equal"; break;
        case LogicOp::LessEqual: fn = "hulk_rt::less_equal"; break;
        default: break;
    }
    expr_result_ = std::string(fn) + "(" + emit_expr(node.GetLeft()) + ", " + emit_expr(node.GetRight()) + ")";
}

void CppCodegen::visit(StringBinOp& node) {
    const bool with_space = node.GetOperator() == StringOp::SpaceConcat;
    expr_result_ = "hulk_rt::concat(" + emit_expr(node.GetLeft()) + ", " +
                   emit_expr(node.GetRight()) + ", " + (with_space ? "true" : "false") + ")";
}

void CppCodegen::visit(ArithmeticUnaryOp& node) {
    expr_result_ = "hulk_rt::neg(" + emit_expr(node.GetOperand()) + ")";
}

void CppCodegen::visit(LogicUnaryOp& node) {
    expr_result_ = "hulk_rt::logical_not(" + emit_expr(node.GetOperand()) + ")";
}

void CppCodegen::visit(VariableReference& node) {
    expr_result_ = lookup_symbol(node, node.GetName());
}

void CppCodegen::visit(VariableBinding& node) {
    expr_result_ = emit_expr(node.GetInitializer());
}

void CppCodegen::visit(LetIn& node) {
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    context_.push_scope();
    for (const auto& binding : node.GetBindings()) {
        const std::string init = emit_expr(binding->GetInitializer());
        const std::string name = mangler_.make_unique("hulk_local", binding->GetName());
        context_.bind(binding.get(), name);
        out << "        hulk_rt::Value " << name << " = " << init << ";\n";
    }
    out << "        return " << emit_expr(node.GetBody()) << ";\n";
    context_.pop_scope();
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(DestructiveAssign& node) {
    auto it = resolution_map_.find(&node);
    if (it == resolution_map_.end()) throw CodegenError("Backend: asignacion sin resolver.");
    std::optional<std::string> target;
    if (it->second.kind == ResolutionKind::Variable) target = context_.lookup(it->second.binding);
    if (it->second.kind == ResolutionKind::Param) target = context_.lookup(it->second.param);
    if (!target) throw CodegenError("Backend: destino de asignacion no disponible.");

    const std::string tmp = new_temp("assign");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << "        hulk_rt::Value " << tmp << " = " << emit_expr(node.GetValue()) << ";\n";
    out << "        " << *target << " = " << tmp << ";\n";
    out << "        return " << tmp << ";\n";
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(DestructiveAssignMember& node) {
    const std::string obj = new_temp("member_obj");
    const std::string val = new_temp("member_value");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << "        hulk_rt::Value " << obj << " = " << emit_expr(node.GetObject()) << ";\n";
    out << "        hulk_rt::Value " << val << " = " << emit_expr(node.GetValue()) << ";\n";
    out << "        return hulk_rt::set_field(" << obj << ", " << cpp_string(node.GetMemberName())
        << ", " << val << ");\n";
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(IfStmt& node) {
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << "        if (hulk_rt::truthy(" << emit_expr(node.GetCondition()) << ")) return "
        << emit_expr(node.GetThenBranch()) << ";\n";
    for (const auto& branch : node.GetElifBranches()) {
        out << "        else if (hulk_rt::truthy(" << emit_expr(branch.condition.get())
            << ")) return " << emit_expr(branch.body.get()) << ";\n";
    }
    if (node.GetElseBranch()) {
        out << "        else return " << emit_expr(node.GetElseBranch()) << ";\n";
    } else {
        out << "        return hulk_rt::Value();\n";
    }
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(WhileStmt& node) {
    const std::string result = new_temp("while_result");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << "        hulk_rt::Value " << result << ";\n";
    out << "        while (hulk_rt::truthy(" << emit_expr(node.GetCondition()) << ")) {\n";
    out << "            " << result << " = " << emit_expr(node.GetBody()) << ";\n";
    out << "        }\n";
    out << "        return " << result << ";\n";
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(For&) {
    unsupported("for/range");
}

void CppCodegen::visit(FunctionCall& node) {
    auto res_it = resolution_map_.find(&node);
    if (node.GetName() == "base" ||
        (res_it != resolution_map_.end() && res_it->second.kind == ResolutionKind::Method)) {
        expr_result_ = emit_base_call(node.GetArgs());
        return;
    }

    if (res_it != resolution_map_.end() && res_it->second.kind == ResolutionKind::BuiltinFunction) {
        unsupported("builtin function '" + node.GetName() + "'");
    }

    if (res_it == resolution_map_.end() || res_it->second.kind != ResolutionKind::Function) {
        throw CodegenError("Backend: llamada sin resolver a '" + node.GetName() + "'.");
    }

    auto fn_name = function_names_.find(res_it->second.func_decl);
    if (fn_name == function_names_.end()) {
        throw CodegenError("Backend: funcion sin nombre generado '" + node.GetName() + "'.");
    }

    const std::string vec = new_temp("call_args");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << emit_args_vector(node.GetArgs(), vec);
    out << "        return " << fn_name->second << "(" << vec << ");\n";
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(Lambda&) {
    unsupported("lambda");
}

void CppCodegen::visit(Print& node) {
    expr_result_ = "hulk_rt::hulk_print(" + emit_expr(node.GetExpr()) + ")";
}

void CppCodegen::visit(BuiltinCall& node) {
    switch (node.GetFunc()) {
        case BuiltinFunc::Sqrt:
            expr_result_ = "hulk_rt::hulk_sqrt(" + emit_expr(node.GetArgs()[0].get()) + ")";
            break;
        case BuiltinFunc::Sin:
            expr_result_ = "hulk_rt::hulk_sin(" + emit_expr(node.GetArgs()[0].get()) + ")";
            break;
        case BuiltinFunc::Cos:
            expr_result_ = "hulk_rt::hulk_cos(" + emit_expr(node.GetArgs()[0].get()) + ")";
            break;
        case BuiltinFunc::Exp:
            expr_result_ = "hulk_rt::hulk_exp(" + emit_expr(node.GetArgs()[0].get()) + ")";
            break;
        case BuiltinFunc::Log:
            expr_result_ = "hulk_rt::hulk_log(" + emit_expr(node.GetArgs()[0].get()) + ", " +
                           emit_expr(node.GetArgs()[1].get()) + ")";
            break;
        case BuiltinFunc::Rand:
            expr_result_ = "hulk_rt::hulk_rand()";
            break;
        case BuiltinFunc::Range:
            unsupported("range");
    }
}

void CppCodegen::visit(ExprBlock& node) {
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    context_.push_scope();
    const auto& exprs = node.GetExprs();
    for (std::size_t i = 0; i < exprs.size(); ++i) {
        if (i + 1 == exprs.size()) {
            out << "        return " << emit_expr(exprs[i].get()) << ";\n";
        } else {
            out << "        (void)" << emit_expr(exprs[i].get()) << ";\n";
        }
    }
    if (exprs.empty()) out << "        return hulk_rt::Value();\n";
    context_.pop_scope();
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(Group& node) {
    expr_result_ = emit_expr(node.GetExpr());
}

void CppCodegen::visit(SelfRef&) {
    expr_result_ = "hulk_self_value";
}

void CppCodegen::visit(BaseCall& node) {
    expr_result_ = emit_base_call(node.GetArgs());
}

void CppCodegen::visit(NewExpr& node) {
    const auto ctor = ctor_names_.find(node.GetTypeName());
    if (ctor == ctor_names_.end()) {
        throw CodegenError("Backend: constructor no encontrado para '" + node.GetTypeName() + "'.");
    }
    const std::string vec = new_temp("ctor_args");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << emit_args_vector(node.GetArgs(), vec);
    out << "        return hulk_rt::Value(" << ctor->second << "(" << vec << "));\n";
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(MemberAccess& node) {
    expr_result_ = "hulk_rt::get_field(" + emit_expr(node.GetObject()) + ", " +
                   cpp_string(node.GetMemberName()) + ")";
}

void CppCodegen::visit(MethodCall& node) {
    const std::string obj = new_temp("method_obj");
    const std::string vec = new_temp("method_args");
    std::ostringstream out;
    out << "([&]() -> hulk_rt::Value {\n";
    out << "        hulk_rt::Value " << obj << " = " << emit_expr(node.GetObject()) << ";\n";
    out << emit_args_vector(node.GetArgs(), vec);
    out << "        return hulk_rt::call_method(" << obj << ", "
        << cpp_string(node.GetMethodName()) << ", " << vec << ");\n";
    out << "    }())";
    expr_result_ = out.str();
}

void CppCodegen::visit(IsExpr& node) {
    expr_result_ = "hulk_rt::Value(hulk_rt::is_instance(" + emit_expr(node.GetExpr()) + ", " +
                   cpp_string(node.GetTypeName()) + "))";
}

void CppCodegen::visit(AsExpr& node) {
    expr_result_ = "hulk_rt::checked_cast(" + emit_expr(node.GetExpr()) + ", " +
                   cpp_string(node.GetTypeName()) + ")";
}

void CppCodegen::visit(FunctionDecl&) {}
void CppCodegen::visit(TypeDecl&) {}
void CppCodegen::visit(TypeMemberAttribute&) {}
void CppCodegen::visit(TypeMemberMethod&) {}
void CppCodegen::visit(ProtocolDecl&) {
    unsupported("protocol");
}

} // namespace Hulk::Backend
