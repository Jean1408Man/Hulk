#ifndef HULK_BACKEND_IR_GEN_H
#define HULK_BACKEND_IR_GEN_H

#include "codegen_context.h"
#include "codegen_error.h"
#include "name_mangler.h"

#include "../binding/symbol_resolver.h"
#include "../eval/visitor.h"
#include "../inference/hulk_type.h"
#include "../ir/ir.h"
#include "../semantic/semantic_tables.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Hulk {
class Program;
class Expr;
class FunctionDecl;
class TypeDecl;
class TypeMemberMethod;
}

namespace Hulk::Backend {

class IRGen : public ExprVisitor, public DeclVisitor {
public:
    IRGen(const SemanticTables& tables,
          const std::unordered_map<Expr*, ResolutionResult>& resolution_map,
          const std::unordered_map<Expr*, HulkType>& type_map);

    IR::IRProgram generate(Program& program);

private:
    const SemanticTables& tables_;
    const std::unordered_map<Expr*, ResolutionResult>& resolution_map_;
    const std::unordered_map<Expr*, HulkType>& type_map_;

    NameMangler mangler_;
    CodegenContext context_;
    IR::IRProgram program_;
    IR::IRFunction* current_function_ = nullptr;
    std::string expr_result_;

    std::unordered_map<const FunctionDecl*, std::string> function_names_;
    std::unordered_map<std::string, const TypeDecl*> type_decls_;
    std::unordered_map<std::string, std::string> init_names_;
    std::unordered_map<std::string, std::string> ctor_names_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> method_names_;

    std::string current_type_name_;
    std::string current_method_name_;
    std::string current_self_name_;

    std::size_t temp_counter_ = 0;
    std::size_t label_counter_ = 0;
    std::size_t data_counter_ = 0;

    void collect_declarations(Program& program);
    void emit_type_metadata(Program& program);
    void emit_global_function(FunctionDecl& fn);
    void emit_type_functions(TypeDecl& type);
    void emit_type_initializer(TypeDecl& type);
    void emit_method(const std::string& type_name, TypeMemberMethod& method);
    void emit_entry(Program& program);

    std::string lower_expr(Expr* expr);
    std::vector<std::string> lower_args(const std::vector<std::unique_ptr<Expr>>& args);
    std::string emit_base_call(const std::vector<std::unique_ptr<Expr>>& args);
    std::string lookup_symbol(Expr& node, const std::string& fallback_name);

    IR::IRFunction start_function(std::string name,
                                  std::string source_name,
                                  IR::IRFunctionKind kind);
    void finish_function(IR::IRFunction&& fn);
    void add_param(const std::string& name);
    void add_local(const std::string& name);
    IR::IRInstr make_instr(IR::IROp op) const;
    void emit(IR::IRInstr instr);
    void emit_return(const std::string& value);
    void emit_move(const std::string& dest, const std::string& src);
    void emit_binary(IR::IROp op,
                     const std::string& dest,
                     const std::string& left,
                     const std::string& right);
    void emit_unary(IR::IROp op, const std::string& dest, const std::string& src);
    void emit_label(const std::string& label);
    void emit_jump(const std::string& label);
    void emit_jump_if(IR::IROp op, const std::string& cond, const std::string& label);
    void emit_const_bool_to(const std::string& dest, bool value);
    std::string new_temp(const std::string& hint);
    std::string new_label(const std::string& hint);
    std::string add_string_data(const std::string& value);
    std::string const_nil();
    std::string const_bool(bool value);
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
