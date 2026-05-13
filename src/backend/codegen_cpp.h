#ifndef HULK_BACKEND_CODEGEN_CPP_H
#define HULK_BACKEND_CODEGEN_CPP_H

#include "codegen_context.h"
#include "name_mangler.h"

#include "../binding/symbol_resolver.h"
#include "../eval/visitor.h"
#include "../inference/hulk_type.h"
#include "../semantic/semantic_tables.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace Hulk {
class Program;
class Expr;
class Decl;
class FunctionDecl;
class TypeDecl;
class TypeMemberMethod;
}

namespace Hulk::Backend {

class CodegenError : public std::runtime_error {
public:
    explicit CodegenError(const std::string& msg) : std::runtime_error(msg) {}
};

class CppCodegen : public ExprVisitor, public DeclVisitor {
public:
    CppCodegen(const SemanticTables& tables,
               const std::unordered_map<Expr*, ResolutionResult>& resolution_map,
               const std::unordered_map<Expr*, HulkType>& type_map);

    std::string generate(Program& program);

private:
    const SemanticTables& tables_;
    const std::unordered_map<Expr*, ResolutionResult>& resolution_map_;
    const std::unordered_map<Expr*, HulkType>& type_map_;

    NameMangler mangler_;
    CodegenContext context_;
    std::string expr_result_;

    std::unordered_map<const FunctionDecl*, std::string> function_names_;
    std::unordered_map<std::string, const TypeDecl*> type_decls_;
    std::unordered_map<std::string, std::string> ctor_names_;
    std::unordered_map<std::string, std::string> init_names_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> method_names_;

    std::string current_type_name_;
    std::string current_method_name_;

    void collect_declarations(Program& program);
    std::string emit_program(Program& program);
    std::string emit_prototypes();
    std::string emit_function(FunctionDecl& fn);
    std::string emit_type(TypeDecl& type);
    std::string emit_method(const std::string& type_name, TypeMemberMethod& method);
    std::string emit_register_types();
    std::string emit_main(Program& program);

    std::string emit_expr(Expr* expr);
    std::string emit_args_vector(const std::vector<std::unique_ptr<Expr>>& args,
                                 const std::string& vec_name);
    std::string emit_base_call(const std::vector<std::unique_ptr<Expr>>& args);

    std::string new_temp(const std::string& hint);
    std::string cpp_string(const std::string& text) const;
    std::string lookup_symbol(Expr& node, const std::string& fallback_name);
    [[noreturn]] void unsupported(const std::string& feature);

    void visit(Number& node) override;
    void visit(String& node) override;
    void visit(Boolean& node) override;
    void visit(ArithmeticBinOp& node) override;
    void visit(LogicBinOp& node) override;
    void visit(StringBinOp& node) override;
    void visit(ArithmeticUnaryOp& node) override;
    void visit(LogicUnaryOp& node) override;
    void visit(VariableReference& node) override;
    void visit(VariableBinding& node) override;
    void visit(LetIn& node) override;
    void visit(DestructiveAssign& node) override;
    void visit(DestructiveAssignMember& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(For& node) override;
    void visit(FunctionCall& node) override;
    void visit(Lambda& node) override;
    void visit(Print& node) override;
    void visit(BuiltinCall& node) override;
    void visit(ExprBlock& node) override;
    void visit(Group& node) override;
    void visit(SelfRef& node) override;
    void visit(BaseCall& node) override;
    void visit(NewExpr& node) override;
    void visit(MemberAccess& node) override;
    void visit(MethodCall& node) override;
    void visit(IsExpr& node) override;
    void visit(AsExpr& node) override;

    void visit(FunctionDecl& node) override;
    void visit(TypeDecl& node) override;
    void visit(TypeMemberAttribute& node) override;
    void visit(TypeMemberMethod& node) override;
    void visit(ProtocolDecl& node) override;
};

} // namespace Hulk::Backend

#endif
