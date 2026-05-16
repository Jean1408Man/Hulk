#include "ir_gen.h"

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
#include <sstream>
#include <utility>

namespace Hulk::Backend {

IRGen::IRGen(const SemanticTables& tables,
             const std::unordered_map<Expr*, ResolutionResult>& resolution_map,
             const std::unordered_map<Expr*, HulkType>& type_map)
    : tables_(tables), resolution_map_(resolution_map), type_map_(type_map) {}

IR::IRProgram IRGen::generate(Program& program) {
    (void)type_map_;
    program_ = IR::IRProgram{};
    temp_counter_ = 0;
    label_counter_ = 0;
    data_counter_ = 0;
    function_names_.clear();
    type_decls_.clear();
    init_names_.clear();
    ctor_names_.clear();
    method_names_.clear();

    collect_declarations(program);
    emit_type_metadata(program);

    for (const auto& decl : program.GetDeclarations()) {
        if (auto* fn = dynamic_cast<FunctionDecl*>(decl.get())) {
            emit_global_function(*fn);
        }
    }
    for (const auto& decl : program.GetDeclarations()) {
        if (auto* type = dynamic_cast<TypeDecl*>(decl.get())) {
            emit_type_functions(*type);
        }
    }
    emit_entry(program);
    return std::move(program_);
}

void IRGen::collect_declarations(Program& program) {
    for (const auto& decl : program.GetDeclarations()) {
        if (auto* fn = dynamic_cast<FunctionDecl*>(decl.get())) {
            function_names_[fn] = mangler_.make_unique("hulk_fn", fn->GetName());
        } else if (auto* type = dynamic_cast<TypeDecl*>(decl.get())) {
            const std::string& type_name = type->GetName();
            type_decls_[type_name] = type;
            init_names_[type_name] = mangler_.make_unique("hulk_init", type_name);
            ctor_names_[type_name] = mangler_.make_unique("hulk_ctor", type_name);
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

void IRGen::emit_type_metadata(Program& program) {
    for (const auto& decl : program.GetDeclarations()) {
        auto* type = dynamic_cast<TypeDecl*>(decl.get());
        if (!type) continue;

        IR::IRType ir_type;
        ir_type.name = type->GetName();
        ir_type.parent = type->HasParent() ? type->GetParentName() : "Object";
        ir_type.init_name = init_names_.at(ir_type.name);
        ir_type.ctor_name = ctor_names_.at(ir_type.name);

        int field_slot = 0;
        int method_slot = 0;
        for (const auto& member : type->GetMembers()) {
            if (member.kind == TypeMember::Kind::Attribute) {
                auto* attr = static_cast<TypeMemberAttribute*>(member.node.get());
                IR::IRField field;
                field.owner_type = ir_type.name;
                field.name = attr->GetName();
                field.lowered_name = NameMangler::sanitize(ir_type.name + "_" + attr->GetName());
                field.type_name = attr->HasTypeAnnotation() ? attr->GetTypeAnnotation() : "Object";
                field.slot = field_slot++;
                ir_type.fields.push_back(std::move(field));
            } else if (member.kind == TypeMember::Kind::Method) {
                auto* method = static_cast<TypeMemberMethod*>(member.node.get());
                IR::IRMethod ir_method;
                ir_method.owner_type = ir_type.name;
                ir_method.name = method->GetName();
                ir_method.function_name = method_names_.at(ir_type.name).at(method->GetName());
                ir_method.arity = method->GetParams().size();
                ir_method.slot = method_slot++;
                ir_type.methods.push_back(std::move(ir_method));
            }
        }
        program_.types.push_back(std::move(ir_type));
    }
}

IR::IRFunction IRGen::start_function(std::string name,
                                     std::string source_name,
                                     IR::IRFunctionKind kind) {
    IR::IRFunction fn;
    fn.name = std::move(name);
    fn.source_name = std::move(source_name);
    fn.kind = kind;
    current_function_ = &fn;
    context_ = CodegenContext{};
    context_.push_scope();
    return fn;
}

void IRGen::finish_function(IR::IRFunction&& fn) {
    context_.pop_scope();
    current_function_ = nullptr;
    program_.functions.push_back(std::move(fn));
}

void IRGen::add_param(const std::string& name) {
    current_function_->params.push_back(name);
}

void IRGen::add_local(const std::string& name) {
    if (!current_function_) return;
    for (const auto& param : current_function_->params) {
        if (param == name) return;
    }
    for (const auto& local : current_function_->locals) {
        if (local == name) return;
    }
    current_function_->locals.push_back(name);
}

IR::IRInstr IRGen::make_instr(IR::IROp op) const {
    IR::IRInstr instr;
    instr.op = op;
    return instr;
}

void IRGen::emit(IR::IRInstr instr) {
    if (!current_function_) {
        throw CodegenError("Backend IR: intento de emitir instruccion fuera de funcion.");
    }
    current_function_->body.push_back(std::move(instr));
}

void IRGen::emit_return(const std::string& value) {
    auto instr = make_instr(IR::IROp::Return);
    instr.src1 = value;
    emit(std::move(instr));
}

void IRGen::emit_move(const std::string& dest, const std::string& src) {
    auto instr = make_instr(IR::IROp::Move);
    instr.dest = dest;
    instr.src1 = src;
    emit(std::move(instr));
}

void IRGen::emit_binary(IR::IROp op,
                        const std::string& dest,
                        const std::string& left,
                        const std::string& right) {
    auto instr = make_instr(op);
    instr.dest = dest;
    instr.src1 = left;
    instr.src2 = right;
    emit(std::move(instr));
}

void IRGen::emit_unary(IR::IROp op, const std::string& dest, const std::string& src) {
    auto instr = make_instr(op);
    instr.dest = dest;
    instr.src1 = src;
    emit(std::move(instr));
}

void IRGen::emit_label(const std::string& label) {
    auto instr = make_instr(IR::IROp::Label);
    instr.label = label;
    emit(std::move(instr));
}

void IRGen::emit_jump(const std::string& label) {
    auto instr = make_instr(IR::IROp::Jump);
    instr.label = label;
    emit(std::move(instr));
}

void IRGen::emit_jump_if(IR::IROp op, const std::string& cond, const std::string& label) {
    auto instr = make_instr(op);
    instr.src1 = cond;
    instr.label = label;
    emit(std::move(instr));
}

void IRGen::emit_const_bool_to(const std::string& dest, bool value) {
    auto instr = make_instr(IR::IROp::ConstBool);
    instr.dest = dest;
    instr.bool_value = value;
    emit(std::move(instr));
}

std::string IRGen::new_temp(const std::string& hint) {
    const std::string name = "hulk_tmp_" + NameMangler::sanitize(hint) + "_" +
                             std::to_string(temp_counter_++);
    add_local(name);
    return name;
}

std::string IRGen::new_label(const std::string& hint) {
    return "L_" + NameMangler::sanitize(hint) + "_" + std::to_string(label_counter_++);
}

std::string IRGen::add_string_data(const std::string& value) {
    const std::string label = "s" + std::to_string(data_counter_++);
    program_.data.push_back(IR::IRData{label, value});
    return label;
}

std::string IRGen::const_nil() {
    const std::string dest = new_temp("nil");
    auto instr = make_instr(IR::IROp::ConstNil);
    instr.dest = dest;
    emit(std::move(instr));
    return dest;
}

std::string IRGen::const_bool(bool value) {
    const std::string dest = new_temp("bool");
    IR::IRInstr instr;
    instr.op = IR::IROp::ConstBool;
    instr.dest = dest;
    instr.bool_value = value;
    emit(std::move(instr));
    return dest;
}

[[noreturn]] void IRGen::unsupported(const std::string& feature) {
    throw CodegenError("Backend no soporta todavia: " + feature);
}

void IRGen::emit_global_function(FunctionDecl& fn) {
    auto ir_fn = start_function(function_names_.at(&fn), fn.GetName(), IR::IRFunctionKind::Global);

    for (const auto& param : fn.GetParams()) {
        const std::string name = mangler_.make_unique("hulk_param", param.name);
        context_.bind(&param, name);
        add_param(name);
    }

    const std::string result = lower_expr(fn.GetBody());
    emit_return(result);
    finish_function(std::move(ir_fn));
}

void IRGen::emit_type_functions(TypeDecl& type) {
    emit_type_initializer(type);
    for (const auto& member : type.GetMembers()) {
        if (member.kind != TypeMember::Kind::Method) continue;
        auto* method = static_cast<TypeMemberMethod*>(member.node.get());
        emit_method(type.GetName(), *method);
    }
}

void IRGen::emit_type_initializer(TypeDecl& type) {
    auto ir_fn = start_function(init_names_.at(type.GetName()),
                                type.GetName() + ".__init__",
                                IR::IRFunctionKind::Initializer);

    const std::string self_name = "hulk_self_value";
    current_self_name_ = self_name;
    add_param(self_name);

    context_.push_scope();
    for (const auto& param : type.GetCtorParams()) {
        const std::string name = mangler_.make_unique("hulk_ctor_param", param.name);
        context_.bind(&param, name);
        add_param(name);
    }

    if (type.HasParent()) {
        const std::string& parent_name = type.GetParentName();
        if (init_names_.count(parent_name)) {
            std::vector<std::string> args;
            args.push_back(self_name);
            if (!type.GetParentArgs().empty()) {
                const auto lowered = lower_args(type.GetParentArgs());
                args.insert(args.end(), lowered.begin(), lowered.end());
            } else if (!type.HasExplicitConstructor()) {
                for (const auto& param : type.GetCtorParams()) {
                    auto name = context_.lookup(&param);
                    if (!name) {
                        throw CodegenError("Backend IR: parametro de constructor sin nombre.");
                    }
                    args.push_back(*name);
                }
            }

            const std::string ignored = new_temp("parent_init");
            IR::IRInstr instr;
            instr.op = IR::IROp::Call;
            instr.dest = ignored;
            instr.callee = init_names_.at(parent_name);
            instr.args = std::move(args);
            emit(std::move(instr));
        }
    }

    for (const auto& member : type.GetMembers()) {
        if (member.kind != TypeMember::Kind::Attribute) continue;
        auto* attr = static_cast<TypeMemberAttribute*>(member.node.get());
        const std::string value = lower_expr(attr->GetInitializer());
        const std::string ignored = new_temp("field_init");
        IR::IRInstr instr;
        instr.op = IR::IROp::DefineField;
        instr.dest = ignored;
        instr.src1 = self_name;
        instr.src2 = value;
        instr.field_name = attr->GetName();
        emit(std::move(instr));
    }

    context_.pop_scope();
    emit_return(self_name);
    current_self_name_.clear();
    finish_function(std::move(ir_fn));
}

void IRGen::emit_method(const std::string& type_name, TypeMemberMethod& method) {
    auto ir_fn = start_function(method_names_.at(type_name).at(method.GetName()),
                                type_name + "." + method.GetName(),
                                IR::IRFunctionKind::Method);

    const std::string prev_type = current_type_name_;
    const std::string prev_method = current_method_name_;
    const std::string prev_self = current_self_name_;
    current_type_name_ = type_name;
    current_method_name_ = method.GetName();
    current_self_name_ = "hulk_self_value";

    add_param(current_self_name_);
    for (const auto& param : method.GetParams()) {
        const std::string name = mangler_.make_unique("hulk_method_param", param.name);
        context_.bind(&param, name);
        add_param(name);
    }

    const std::string result = lower_expr(method.GetBody());
    emit_return(result);

    current_type_name_ = prev_type;
    current_method_name_ = prev_method;
    current_self_name_ = prev_self;
    finish_function(std::move(ir_fn));
}

void IRGen::emit_entry(Program& program) {
    auto ir_fn = start_function(program_.entry_function, "program", IR::IRFunctionKind::Entry);
    const std::string result = program.GetGlobalExpr() ? lower_expr(program.GetGlobalExpr()) : const_nil();
    emit_return(result);
    finish_function(std::move(ir_fn));
}

std::string IRGen::lower_expr(Expr* expr) {
    if (!expr) return const_nil();
    expr->accept(*this);
    return expr_result_;
}

std::vector<std::string> IRGen::lower_args(const std::vector<std::unique_ptr<Expr>>& args) {
    std::vector<std::string> lowered;
    lowered.reserve(args.size());
    for (const auto& arg : args) {
        lowered.push_back(lower_expr(arg.get()));
    }
    return lowered;
}

std::string IRGen::emit_base_call(const std::vector<std::unique_ptr<Expr>>& args) {
    if (current_type_name_.empty() || current_method_name_.empty()) {
        unsupported("base() fuera de metodo");
    }

    const auto* type = tables_.lookup_type(current_type_name_);
    if (!type || type->parent_name.empty()) {
        unsupported("base() en tipo sin padre");
    }

    const std::string dest = new_temp("base_call");
    IR::IRInstr instr;
    instr.op = IR::IROp::SCallMethod;
    instr.dest = dest;
    instr.src1 = current_self_name_;
    instr.type_name = type->parent_name;
    instr.method_name = current_method_name_;
    instr.args = lower_args(args);
    emit(std::move(instr));
    return dest;
}

std::string IRGen::lookup_symbol(Expr& node, const std::string& fallback_name) {
    auto it = resolution_map_.find(&node);
    if (it == resolution_map_.end()) {
        if (fallback_name == "self" && !current_self_name_.empty()) return current_self_name_;
        throw CodegenError("Backend IR: referencia sin resolver '" + fallback_name + "'.");
    }

    const ResolutionResult& res = it->second;
    if (res.kind == ResolutionKind::Variable) {
        auto name = context_.lookup(res.binding);
        if (!name) throw CodegenError("Backend IR: binding sin nombre.");
        return *name;
    }
    if (res.kind == ResolutionKind::Param) {
        auto name = context_.lookup(res.param);
        if (!name) throw CodegenError("Backend IR: parametro sin nombre.");
        return *name;
    }
    if (res.kind == ResolutionKind::Synthetic) {
        if (res.synthetic->kind == SyntheticKind::Self) return current_self_name_;
        unsupported("for/range");
    }

    throw CodegenError("Backend IR: simbolo no soportado en referencia '" + fallback_name + "'.");
}

void IRGen::visit(Number& node) {
    const std::string dest = new_temp("number");
    IR::IRInstr instr;
    instr.op = IR::IROp::ConstNumber;
    instr.dest = dest;
    instr.number_value = node.GetValue();
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(String& node) {
    const std::string dest = new_temp("string");
    const std::string label = add_string_data(node.GetValue());
    IR::IRInstr instr;
    instr.op = IR::IROp::LoadData;
    instr.dest = dest;
    instr.src1 = label;
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(Boolean& node) {
    expr_result_ = const_bool(node.GetValue());
}

void IRGen::visit(ArithmeticBinOp& node) {
    const std::string left = lower_expr(node.GetLeft());
    const std::string right = lower_expr(node.GetRight());
    const std::string dest = new_temp("arith");
    IR::IROp op = IR::IROp::Add;
    switch (node.GetOperator()) {
        case ArithmeticOp::Plus: op = IR::IROp::Add; break;
        case ArithmeticOp::Minus: op = IR::IROp::Sub; break;
        case ArithmeticOp::Mult: op = IR::IROp::Mul; break;
        case ArithmeticOp::Div: op = IR::IROp::Div; break;
        case ArithmeticOp::Mod: op = IR::IROp::Mod; break;
        case ArithmeticOp::Pow: op = IR::IROp::Pow; break;
    }
    emit_binary(op, dest, left, right);
    expr_result_ = dest;
}

void IRGen::visit(LogicBinOp& node) {
    if (node.GetOperator() == LogicOp::And || node.GetOperator() == LogicOp::Or) {
        const std::string dest = new_temp("logic");
        const std::string true_label = new_label("logic_true");
        const std::string false_label = new_label("logic_false");
        const std::string end_label = new_label("logic_end");
        const std::string left = lower_expr(node.GetLeft());

        if (node.GetOperator() == LogicOp::And) {
            emit_jump_if(IR::IROp::JumpIfFalse, left, false_label);
            const std::string right = lower_expr(node.GetRight());
            emit_jump_if(IR::IROp::JumpIfFalse, right, false_label);
            emit_jump(true_label);
        } else {
            emit_jump_if(IR::IROp::JumpIfTrue, left, true_label);
            const std::string right = lower_expr(node.GetRight());
            emit_jump_if(IR::IROp::JumpIfTrue, right, true_label);
            emit_jump(false_label);
        }

        emit_label(true_label);
        emit_const_bool_to(dest, true);
        emit_jump(end_label);
        emit_label(false_label);
        emit_const_bool_to(dest, false);
        emit_label(end_label);
        expr_result_ = dest;
        return;
    }

    const std::string left = lower_expr(node.GetLeft());
    const std::string right = lower_expr(node.GetRight());
    const std::string dest = new_temp("cmp");
    IR::IROp op = IR::IROp::Equal;
    switch (node.GetOperator()) {
        case LogicOp::Equal: op = IR::IROp::Equal; break;
        case LogicOp::NotEqual: op = IR::IROp::NotEqual; break;
        case LogicOp::Greater: op = IR::IROp::Greater; break;
        case LogicOp::Less: op = IR::IROp::Less; break;
        case LogicOp::GreaterEqual: op = IR::IROp::GreaterEqual; break;
        case LogicOp::LessEqual: op = IR::IROp::LessEqual; break;
        default: break;
    }
    emit_binary(op, dest, left, right);
    expr_result_ = dest;
}

void IRGen::visit(StringBinOp& node) {
    const std::string left = lower_expr(node.GetLeft());
    const std::string right = lower_expr(node.GetRight());
    const std::string dest = new_temp("concat");
    const IR::IROp op = node.GetOperator() == StringOp::SpaceConcat
                            ? IR::IROp::ConcatSpace
                            : IR::IROp::Concat;
    emit_binary(op, dest, left, right);
    expr_result_ = dest;
}

void IRGen::visit(ArithmeticUnaryOp& node) {
    const std::string value = lower_expr(node.GetOperand());
    const std::string dest = new_temp("neg");
    emit_unary(IR::IROp::Neg, dest, value);
    expr_result_ = dest;
}

void IRGen::visit(LogicUnaryOp& node) {
    const std::string value = lower_expr(node.GetOperand());
    const std::string dest = new_temp("not");
    emit_unary(IR::IROp::Not, dest, value);
    expr_result_ = dest;
}

void IRGen::visit(VariableReference& node) {
    auto it = resolution_map_.find(&node);
    if (it != resolution_map_.end() && it->second.kind == ResolutionKind::BuiltinConstant) {
        const std::string dest = new_temp("builtin_const");
        double value = 0.0;
        if (it->second.builtin_const->name == "PI") value = 3.14159265358979323846;
        else if (it->second.builtin_const->name == "E") value = 2.71828182845904523536;
        else throw CodegenError("Backend IR: constante builtin no soportada.");
        IR::IRInstr instr;
        instr.op = IR::IROp::ConstNumber;
        instr.dest = dest;
        instr.number_value = value;
        emit(std::move(instr));
        expr_result_ = dest;
        return;
    }

    expr_result_ = lookup_symbol(node, node.GetName());
}

void IRGen::visit(VariableBinding& node) {
    expr_result_ = lower_expr(node.GetInitializer());
}

void IRGen::visit(LetIn& node) {
    context_.push_scope();
    for (const auto& binding : node.GetBindings()) {
        const std::string init = lower_expr(binding->GetInitializer());
        const std::string local = mangler_.make_unique("hulk_local", binding->GetName());
        add_local(local);
        context_.bind(binding.get(), local);
        emit_move(local, init);
    }
    expr_result_ = lower_expr(node.GetBody());
    context_.pop_scope();
}

void IRGen::visit(DestructiveAssign& node) {
    auto it = resolution_map_.find(&node);
    if (it == resolution_map_.end()) throw CodegenError("Backend IR: asignacion sin resolver.");

    std::optional<std::string> target;
    if (it->second.kind == ResolutionKind::Variable) target = context_.lookup(it->second.binding);
    if (it->second.kind == ResolutionKind::Param) target = context_.lookup(it->second.param);
    if (!target) throw CodegenError("Backend IR: destino de asignacion no disponible.");

    const std::string value = lower_expr(node.GetValue());
    const std::string result = new_temp("assign");
    emit_move(result, value);
    emit_move(*target, result);
    expr_result_ = result;
}

void IRGen::visit(DestructiveAssignMember& node) {
    const std::string obj = lower_expr(node.GetObject());
    const std::string value = lower_expr(node.GetValue());
    const std::string dest = new_temp("member_assign");
    IR::IRInstr instr;
    instr.op = IR::IROp::SetField;
    instr.dest = dest;
    instr.src1 = obj;
    instr.src2 = value;
    instr.field_name = node.GetMemberName();
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(IfStmt& node) {
    const std::string dest = new_temp("if_result");
    const std::string end_label = new_label("if_end");
    const std::string first_else = new_label("if_next");

    const std::string cond = lower_expr(node.GetCondition());
    emit_jump_if(IR::IROp::JumpIfFalse, cond, first_else);
    const std::string then_value = lower_expr(node.GetThenBranch());
    emit_move(dest, then_value);
    emit_jump(end_label);
    emit_label(first_else);

    const auto& elifs = node.GetElifBranches();
    for (std::size_t i = 0; i < elifs.size(); ++i) {
        const std::string next_label = new_label("if_next");
        const std::string elif_cond = lower_expr(elifs[i].condition.get());
        emit_jump_if(IR::IROp::JumpIfFalse, elif_cond, next_label);
        const std::string elif_value = lower_expr(elifs[i].body.get());
        emit_move(dest, elif_value);
        emit_jump(end_label);
        emit_label(next_label);
    }

    const std::string else_value = node.GetElseBranch() ? lower_expr(node.GetElseBranch()) : const_nil();
    emit_move(dest, else_value);
    emit_label(end_label);
    expr_result_ = dest;
}

void IRGen::visit(WhileStmt& node) {
    const std::string dest = new_temp("while_result");
    const std::string start_label = new_label("while_start");
    const std::string end_label = new_label("while_end");
    const std::string initial = const_nil();
    emit_move(dest, initial);
    emit_label(start_label);
    const std::string cond = lower_expr(node.GetCondition());
    emit_jump_if(IR::IROp::JumpIfFalse, cond, end_label);
    const std::string body = lower_expr(node.GetBody());
    emit_move(dest, body);
    emit_jump(start_label);
    emit_label(end_label);
    expr_result_ = dest;
}

void IRGen::visit(For&) {
    unsupported("for/range");
}

void IRGen::visit(FunctionCall& node) {
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
        throw CodegenError("Backend IR: llamada sin resolver a '" + node.GetName() + "'.");
    }

    auto fn_name = function_names_.find(res_it->second.func_decl);
    if (fn_name == function_names_.end()) {
        throw CodegenError("Backend IR: funcion sin nombre generado '" + node.GetName() + "'.");
    }

    const std::string dest = new_temp("call");
    IR::IRInstr instr;
    instr.op = IR::IROp::Call;
    instr.dest = dest;
    instr.callee = fn_name->second;
    instr.args = lower_args(node.GetArgs());
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(Lambda&) {
    unsupported("lambda");
}

void IRGen::visit(Print& node) {
    const std::string value = lower_expr(node.GetExpr());
    const std::string dest = new_temp("print");
    IR::IRInstr instr;
    instr.op = IR::IROp::BuiltinPrint;
    instr.dest = dest;
    instr.args = {value};
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(BuiltinCall& node) {
    IR::IROp op = IR::IROp::BuiltinSqrt;
    switch (node.GetFunc()) {
        case BuiltinFunc::Sqrt: op = IR::IROp::BuiltinSqrt; break;
        case BuiltinFunc::Sin: op = IR::IROp::BuiltinSin; break;
        case BuiltinFunc::Cos: op = IR::IROp::BuiltinCos; break;
        case BuiltinFunc::Exp: op = IR::IROp::BuiltinExp; break;
        case BuiltinFunc::Log: op = IR::IROp::BuiltinLog; break;
        case BuiltinFunc::Rand: op = IR::IROp::BuiltinRand; break;
        case BuiltinFunc::Range: unsupported("range");
    }

    const std::string dest = new_temp("builtin");
    IR::IRInstr instr;
    instr.op = op;
    instr.dest = dest;
    instr.args = lower_args(node.GetArgs());
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(ExprBlock& node) {
    context_.push_scope();
    const auto& exprs = node.GetExprs();
    if (exprs.empty()) {
        expr_result_ = const_nil();
    } else {
        std::string result;
        for (const auto& expr : exprs) {
            result = lower_expr(expr.get());
        }
        expr_result_ = result;
    }
    context_.pop_scope();
}

void IRGen::visit(Group& node) {
    expr_result_ = lower_expr(node.GetExpr());
}

void IRGen::visit(SelfRef&) {
    if (current_self_name_.empty()) unsupported("self fuera de metodo");
    expr_result_ = current_self_name_;
}

void IRGen::visit(BaseCall& node) {
    expr_result_ = emit_base_call(node.GetArgs());
}

void IRGen::visit(NewExpr& node) {
    const auto init = init_names_.find(node.GetTypeName());
    if (init == init_names_.end()) {
        throw CodegenError("Backend IR: constructor no encontrado para '" + node.GetTypeName() + "'.");
    }

    const std::string object = new_temp("object");
    IR::IRInstr new_instr;
    new_instr.op = IR::IROp::NewObject;
    new_instr.dest = object;
    new_instr.type_name = node.GetTypeName();
    emit(std::move(new_instr));

    std::vector<std::string> args;
    args.push_back(object);
    const auto lowered = lower_args(node.GetArgs());
    args.insert(args.end(), lowered.begin(), lowered.end());

    const std::string ignored = new_temp("init_call");
    IR::IRInstr call;
    call.op = IR::IROp::Call;
    call.dest = ignored;
    call.callee = init->second;
    call.args = std::move(args);
    emit(std::move(call));
    expr_result_ = object;
}

void IRGen::visit(MemberAccess& node) {
    const std::string object = lower_expr(node.GetObject());
    const std::string dest = new_temp("field");
    IR::IRInstr instr;
    instr.op = IR::IROp::GetField;
    instr.dest = dest;
    instr.src1 = object;
    instr.field_name = node.GetMemberName();
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(MethodCall& node) {
    const std::string object = lower_expr(node.GetObject());
    const std::string dest = new_temp("method_call");
    IR::IRInstr instr;
    instr.op = IR::IROp::VCall;
    instr.dest = dest;
    instr.src1 = object;
    instr.method_name = node.GetMethodName();
    instr.args = lower_args(node.GetArgs());
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(IsExpr& node) {
    const std::string value = lower_expr(node.GetExpr());
    const std::string dest = new_temp("is");
    IR::IRInstr instr;
    instr.op = IR::IROp::IsType;
    instr.dest = dest;
    instr.src1 = value;
    instr.type_name = node.GetTypeName();
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(AsExpr& node) {
    const std::string value = lower_expr(node.GetExpr());
    const std::string dest = new_temp("as");
    IR::IRInstr instr;
    instr.op = IR::IROp::AsType;
    instr.dest = dest;
    instr.src1 = value;
    instr.type_name = node.GetTypeName();
    emit(std::move(instr));
    expr_result_ = dest;
}

void IRGen::visit(FunctionDecl&) {}
void IRGen::visit(TypeDecl&) {}
void IRGen::visit(TypeMemberAttribute&) {}
void IRGen::visit(TypeMemberMethod&) {}
void IRGen::visit(ProtocolDecl&) {
    unsupported("protocol");
}

} // namespace Hulk::Backend
