#include "evaluator.h"
#include "visitor.h"

// AST includes — todos los nodos concretos que visita este evaluador
#include "../ast/literales/number.h"
#include "../ast/literales/string.h"
#include "../ast/literales/boolean.h"
#include "../ast/binOps/arithmeticBinOp.h"
#include "../ast/binOps/logicBinOp.h"
#include "../ast/binOps/stringBinOp.h"
#include "../ast/unaryOps/arithmeticUnaryOp.h"
#include "../ast/unaryOps/logicUnaryOp.h"
#include "../ast/variables/variableReference.h"
#include "../ast/variables/variableBinding.h"
#include "../ast/variables/letIn.h"
#include "../ast/assignments/desctructiveAssign.h"
#include "../ast/assignments/destructiveAssignMember.h"
#include "../ast/conditionals/ifStmt.h"
#include "../ast/loops/while.h"
#include "../ast/loops/for.h"
#include "../ast/functions/functionCall.h"
#include "../ast/functions/functionDecl.h"
#include "../ast/functions/lambda.h"
#include "../ast/domainFunctions/print.h"
#include "../ast/domainFunctions/builtinCall.h"
#include "../ast/others/exprBlock.h"
#include "../ast/others/group.h"
#include "../ast/others/selfRef.h"
#include "../ast/others/baseCall.h"
#include "../ast/others/program.h"
#include "../ast/types/newExpr.h"
#include "../ast/types/memberAccess.h"
#include "../ast/types/methodCall.h"
#include "../ast/types/isExpr.h"
#include "../ast/types/asExpr.h"
#include "../ast/types/typeDecl.h"
#include "../ast/types/typeMemberAttribute.h"
#include "../ast/types/typeMemberMethod.h"
#include "../ast/vectors/vectorLiteral.h"
#include "../ast/vectors/vectorIndex.h"
#include "../ast/vectors/vectorGenerator.h"
#include "../ast/protocols/protocolDecl.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Hulk {

// ============================================================================
// Constructor y punto de entrada
// ============================================================================

Evaluator::Evaluator(hulk::common::DiagnosticEngine& engine)
    : engine_(engine), env_(std::make_shared<Environment>())
{}

void Evaluator::run(Program& program) {
    // Primer pase: registrar todas las declaraciones (funciones y tipos)
    for (auto& decl : program.GetDeclarations())
        decl->accept(*this);

    // Segundo pase: evaluar la expresión global si existe
    if (Expr* global = program.GetGlobalExpr())
        eval(global);
}

// ============================================================================
// Helpers
// ============================================================================

HulkValue Evaluator::eval(Expr* node) {
    if (!node) return HulkValue{};
    node->accept(*this);
    return result_;
}

std::shared_ptr<Environment> Evaluator::make_child_env() {
    return std::make_shared<Environment>(env_);
}

const MethodDef* Evaluator::find_method(const std::string& type_name,
                                         const std::string& method_name) const {
    auto it = types_.find(type_name);
    if (it == types_.end()) return nullptr;
    const TypeDef& def = it->second;
    auto mit = def.methods.find(method_name);
    if (mit != def.methods.end()) return &mit->second;
    if (!def.parent_name.empty())
        return find_method(def.parent_name, method_name);
    return nullptr;
}

void Evaluator::init_object(HulkObject& obj, const TypeDef& def,
                             const std::vector<HulkValue>& ctor_args) {
    // Crear scope de inicialización con los parámetros del constructor
    // Este scope debe construirse ANTES de inicializar el padre, para que
    // las parentArgs (argumentos al padre) se evalúen con los params del hijo.
    auto init_env = std::make_shared<Environment>(env_);
    for (size_t i = 0; i < def.ctor_params.size() && i < ctor_args.size(); ++i)
        init_env->define(def.ctor_params[i].name, ctor_args[i]);

    // self disponible como variable en el scope de inicialización
    auto self_ptr = std::make_shared<HulkObject>(obj.type_name);
    self_ptr->fields = obj.fields;  // snapshot actual (puede estar vacío)
    HulkValue self_val(self_ptr);

    init_env->define("self", self_val);

    HulkValue prev_self = self_;
    self_ = self_val;
    auto prev_env = env_;
    env_ = init_env;

    // Inicializar padre primero (herencia) — evaluar parentArgs en el scope hijo
    if (!def.parent_name.empty()) {
        auto pit = types_.find(def.parent_name);
        if (pit != types_.end()) {
            // Evaluar los argumentos que el hijo pasa al constructor del padre
            std::vector<HulkValue> parent_args;
            for (Expr* parent_arg_expr : def.parent_args)
                parent_args.push_back(eval(parent_arg_expr));
            init_object(obj, pit->second, parent_args);
        }
    }

    // Evaluar cada inicializador de atributo de esta clase
    for (auto& [attr_name, init_expr] : def.attributes) {
        HulkValue val = eval(init_expr);
        obj.fields[attr_name] = val;
        // Mantener self sincronizado con los campos actuales
        self_.as_object()->fields[attr_name] = val;
    }

    env_ = prev_env;
    self_ = prev_self;
}

// ============================================================================
// CORTE 4 — Literales, operaciones, builtins, bloques, let, if, while
// ============================================================================

void Evaluator::visit(Number& n) {
    result_ = HulkValue(n.GetValue());
}

void Evaluator::visit(String& n) {
    result_ = HulkValue(n.GetValue());
}

void Evaluator::visit(Boolean& n) {
    result_ = HulkValue(n.GetValue());
}

// --- Aritmética --------------------------------------------------------------

void Evaluator::visit(ArithmeticBinOp& n) {
    HulkValue left  = eval(n.GetLeft());
    HulkValue right = eval(n.GetRight());

    if (!left.is_number() || !right.is_number())
        report_error(n.span, "EVAL_TYPE_ARITH",
                     left.is_number() ? right.to_string() : left.to_string());

    double l = left.as_number();
    double r = right.as_number();

    switch (n.GetOperator()) {
        case ArithmeticOp::Plus:  result_ = HulkValue(l + r); break;
        case ArithmeticOp::Minus: result_ = HulkValue(l - r); break;
        case ArithmeticOp::Mult:  result_ = HulkValue(l * r); break;
        case ArithmeticOp::Div:
            if (r == 0) report_error(n.span, "EVAL_DIV_ZERO");
            result_ = HulkValue(l / r);
            break;
        case ArithmeticOp::Mod:
            if (r == 0) report_error(n.span, "EVAL_MOD_ZERO");
            result_ = HulkValue(std::fmod(l, r));
            break;
        case ArithmeticOp::Pow:
            result_ = HulkValue(std::pow(l, r));
            break;
    }
}

void Evaluator::visit(ArithmeticUnaryOp& n) {
    HulkValue val = eval(n.GetOperand());
    if (!val.is_number())
        report_error(n.span, "EVAL_TYPE_ARITH", val.to_string());
    result_ = HulkValue(-val.as_number());
}

// --- Lógica y comparación ---------------------------------------------------

void Evaluator::visit(LogicBinOp& n) {
    // Evaluación en corto-circuito para and/or
    switch (n.GetOperator()) {
        case LogicOp::And: {
            HulkValue left = eval(n.GetLeft());
            if (!left.is_truthy()) { result_ = HulkValue(false); return; }
            HulkValue right = eval(n.GetRight());
            result_ = HulkValue(right.is_truthy());
            return;
        }
        case LogicOp::Or: {
            HulkValue left = eval(n.GetLeft());
            if (left.is_truthy()) { result_ = HulkValue(true); return; }
            HulkValue right = eval(n.GetRight());
            result_ = HulkValue(right.is_truthy());
            return;
        }
        default: break;
    }

    HulkValue left  = eval(n.GetLeft());
    HulkValue right = eval(n.GetRight());

    switch (n.GetOperator()) {
        case LogicOp::Equal:
            if (left.is_number() && right.is_number())
                result_ = HulkValue(left.as_number() == right.as_number());
            else if (left.is_string() && right.is_string())
                result_ = HulkValue(left.as_string() == right.as_string());
            else if (left.is_bool() && right.is_bool())
                result_ = HulkValue(left.as_bool() == right.as_bool());
            else
                result_ = HulkValue(false);
            break;
        case LogicOp::NotEqual:
            if (left.is_number() && right.is_number())
                result_ = HulkValue(left.as_number() != right.as_number());
            else if (left.is_string() && right.is_string())
                result_ = HulkValue(left.as_string() != right.as_string());
            else
                result_ = HulkValue(true);
            break;
        case LogicOp::Greater:
            if (!left.is_number() || !right.is_number())
                report_error(n.span, "EVAL_TYPE_ARITH",
                             left.is_number() ? right.to_string() : left.to_string());
            result_ = HulkValue(left.as_number() > right.as_number());
            break;
        case LogicOp::Less:
            if (!left.is_number() || !right.is_number())
                report_error(n.span, "EVAL_TYPE_ARITH",
                             left.is_number() ? right.to_string() : left.to_string());
            result_ = HulkValue(left.as_number() < right.as_number());
            break;
        case LogicOp::GreaterEqual:
            if (!left.is_number() || !right.is_number())
                report_error(n.span, "EVAL_TYPE_ARITH",
                             left.is_number() ? right.to_string() : left.to_string());
            result_ = HulkValue(left.as_number() >= right.as_number());
            break;
        case LogicOp::LessEqual:
            if (!left.is_number() || !right.is_number())
                report_error(n.span, "EVAL_TYPE_ARITH",
                             left.is_number() ? right.to_string() : left.to_string());
            result_ = HulkValue(left.as_number() <= right.as_number());
            break;
        default: break;
    }
}

void Evaluator::visit(LogicUnaryOp& n) {
    HulkValue val = eval(n.GetOperand());
    result_ = HulkValue(!val.is_truthy());
}

// --- Strings ----------------------------------------------------------------

void Evaluator::visit(StringBinOp& n) {
    HulkValue left  = eval(n.GetLeft());
    HulkValue right = eval(n.GetRight());
    std::string l = left.to_string();
    std::string r = right.to_string();

    switch (n.GetOperator()) {
        case StringOp::Concat:      result_ = HulkValue(l + r);        break;
        case StringOp::SpaceConcat: result_ = HulkValue(l + " " + r);  break;
    }
}

// --- Builtins ---------------------------------------------------------------

void Evaluator::visit(Print& n) {
    HulkValue val = eval(n.GetExpr());
    std::cout << val.to_string() << "\n";
    result_ = val;
}

void Evaluator::visit(BuiltinCall& n) {
    const auto& args = n.GetArgs();

    auto num_arg = [&](size_t i) -> double {
        HulkValue v = eval(args[i].get());
        if (!v.is_number())
            report_error(n.span, "EVAL_TYPE_ARITH", v.to_string());
        return v.as_number();
    };

    switch (n.GetFunc()) {
        case BuiltinFunc::Sqrt:  result_ = HulkValue(std::sqrt(num_arg(0)));          break;
        case BuiltinFunc::Sin:   result_ = HulkValue(std::sin(num_arg(0)));           break;
        case BuiltinFunc::Cos:   result_ = HulkValue(std::cos(num_arg(0)));           break;
        case BuiltinFunc::Exp:   result_ = HulkValue(std::exp(num_arg(0)));           break;
        case BuiltinFunc::Log:   result_ = HulkValue(std::log(num_arg(1)) /
                                                      std::log(num_arg(0)));          break;
        case BuiltinFunc::Rand:  result_ = HulkValue((double)rand() / RAND_MAX);      break;
        case BuiltinFunc::Range: {
            double from = num_arg(0), to = num_arg(1);
            auto vec = std::make_shared<HulkVector>();
            for (double i = from; i < to; i += 1.0)
                vec->push_back(HulkValue(i));
            result_ = HulkValue(vec);
            break;
        }
    }
}

// --- Bloques y agrupación ---------------------------------------------------

void Evaluator::visit(ExprBlock& n) {
    auto block_env = make_child_env();
    auto prev_env  = env_;
    env_ = block_env;

    result_ = HulkValue{};
    for (auto& expr : n.GetExprs())
        result_ = eval(expr.get());

    env_ = prev_env;
}

void Evaluator::visit(Group& n) {
    result_ = eval(n.GetExpr());
}

// --- Let/in -----------------------------------------------------------------

void Evaluator::visit(LetIn& n) {
    // Cada binding ve los anteriores del mismo let (scopes encadenados)
    auto prev_env = env_;
    env_ = make_child_env();

    for (auto& binding : n.GetBindings()) {
        HulkValue val = eval(binding->GetInitializer());
        env_->define(binding->GetName(), std::move(val));
    }

    result_ = eval(n.GetBody());
    env_ = prev_env;
}

void Evaluator::visit(VariableBinding& n) {
    // Usado directamente desde LetIn; también puede aparecer solo
    result_ = eval(n.GetInitializer());
}

// --- Control de flujo -------------------------------------------------------

void Evaluator::visit(IfStmt& n) {
    HulkValue cond = eval(n.GetCondition());
    if (cond.is_truthy()) {
        result_ = eval(n.GetThenBranch());
        return;
    }
    for (auto& elif : n.GetElifBranches()) {
        HulkValue elif_cond = eval(elif.condition.get());
        if (elif_cond.is_truthy()) {
            result_ = eval(elif.body.get());
            return;
        }
    }
    if (Expr* else_b = n.GetElseBranch())
        result_ = eval(else_b);
    else
        result_ = HulkValue{};
}

void Evaluator::visit(WhileStmt& n) {
    result_ = HulkValue{};
    while (eval(n.GetCondition()).is_truthy())
        result_ = eval(n.GetBody());
}

void Evaluator::visit(For& n) {
    HulkValue iterable = eval(n.GetIterable());

    if (!iterable.is_vector())
        report_error(n.span, "EVAL_TYPE_NOT_VECTOR", iterable.to_string());

    auto prev_env = env_;
    result_ = HulkValue{};
    for (auto& item : *iterable.as_vector()) {
        env_ = std::make_shared<Environment>(prev_env);
        env_->define(n.GetVarName(), item);
        result_ = eval(n.GetBody());
    }
    env_ = prev_env;
}

// ============================================================================
// CORTE 5 — Variables, scopes, funciones, recursión, :=
// ============================================================================

void Evaluator::visit(VariableReference& n) {
    if (!env_->contains(n.GetName()))
        report_error(n.span, "EVAL_UNDECLARED_VAR", n.GetName());
    result_ = env_->get(n.GetName());
}

void Evaluator::visit(DestructiveAssign& n) {
    if (!env_->contains(n.GetName()))
        report_error(n.span, "EVAL_ASSIGN_UNDECLARED", n.GetName());
    HulkValue val = eval(n.GetValue());
    env_->assign(n.GetName(), val);
    result_ = val;
}

void Evaluator::visit(DestructiveAssignMember& n) {
    HulkValue obj_val = eval(n.GetObject());
    if (!obj_val.is_object())
        report_error(n.span, "EVAL_TYPE_NOT_OBJECT", obj_val.to_string());
    HulkValue val = eval(n.GetValue());
    obj_val.as_object()->fields[n.GetMemberName()] = val;
    result_ = val;
}

// Registro de función (pase de declaraciones en run())
void Evaluator::visit(FunctionDecl& n) {
    funcs_[n.GetName()] = &n;
    result_ = HulkValue{};
}

void Evaluator::visit(FunctionCall& n) {
    // Buscar en tabla global
    auto it = funcs_.find(n.GetName());
    if (it == funcs_.end())
        report_error(n.span, "EVAL_UNDECLARED_FUNC", n.GetName());

    FunctionDecl* decl = it->second;
    const auto& params = decl->GetParams();
    const auto& args   = n.GetArgs();

    if (params.size() != args.size())
        report_error(n.span, "EVAL_ARITY_FUNC",
                     n.GetName(), params.size(), args.size());

    // Evaluar argumentos en el scope actual
    std::vector<HulkValue> arg_vals;
    arg_vals.reserve(args.size());
    for (auto& arg : args)
        arg_vals.push_back(eval(arg.get()));

    // Crear scope de la función sobre el scope GLOBAL (closures léxicos simples)
    auto func_env = std::make_shared<Environment>(env_);
    for (size_t i = 0; i < params.size(); ++i)
        func_env->define(params[i].name, arg_vals[i]);

    auto prev_env = env_;
    env_ = func_env;
    result_ = eval(decl->GetBody());
    env_ = prev_env;
}

void Evaluator::visit(Lambda& n) {
    // Lambdas de primera clase no soportadas en esta versión.
    // El parser desazucara la mayoría de lambdas inline antes de llegar aquí.
    report_error_raw(n.span, "Lambda de primera clase no soportada en esta versión");
}

// ============================================================================
// CORTE 6 — OOP: tipos, instancias, miembros, herencia, self, base, is, as
// ============================================================================

// Registro de tipo (pase de declaraciones en run())
void Evaluator::visit(TypeDecl& n) {
    TypeDef def;
    def.name       = n.GetName();
    def.parent_name = n.HasParent() ? n.GetParentName() : "";

    for (auto& p : n.GetCtorParams())
        def.ctor_params.push_back(p);

    for (auto& expr : n.GetParentArgs())
        def.parent_args.push_back(expr.get());

    for (auto& member : n.GetMembers()) {
        if (member.kind == TypeMember::Kind::Attribute) {
            auto* attr = static_cast<TypeMemberAttribute*>(member.node.get());
            def.attributes.emplace_back(attr->GetName(), attr->GetInitializer());
        } else {
            auto* method = static_cast<TypeMemberMethod*>(member.node.get());
            MethodDef mdef;
            mdef.params = method->GetParams();
            mdef.body   = method->GetBody();
            def.methods[method->GetName()] = std::move(mdef);
        }
    }

    types_[def.name] = std::move(def);
    result_ = HulkValue{};
}

void Evaluator::visit(TypeMemberAttribute& n) { (void)n; result_ = HulkValue{}; }
void Evaluator::visit(TypeMemberMethod& n)    { (void)n; result_ = HulkValue{}; }
void Evaluator::visit(ProtocolDecl& n)        { (void)n; result_ = HulkValue{}; }

void Evaluator::visit(NewExpr& n) {
    auto it = types_.find(n.GetTypeName());
    if (it == types_.end())
        report_error(n.span, "EVAL_UNDECLARED_TYPE", n.GetTypeName());

    const TypeDef& def = it->second;
    const auto& args   = n.GetArgs();

    if (def.ctor_params.size() != args.size())
        report_error(n.span, "EVAL_ARITY_CTOR",
                     n.GetTypeName(), def.ctor_params.size(), args.size());

    // Evaluar argumentos del constructor
    std::vector<HulkValue> ctor_vals;
    ctor_vals.reserve(args.size());
    for (auto& arg : args)
        ctor_vals.push_back(eval(arg.get()));

    auto obj = std::make_shared<HulkObject>(n.GetTypeName());
    init_object(*obj, def, ctor_vals);

    result_ = HulkValue(obj);
}

void Evaluator::visit(MemberAccess& n) {
    HulkValue obj_val = eval(n.GetObject());
    if (!obj_val.is_object())
        report_error(n.span, "EVAL_TYPE_NOT_OBJECT", obj_val.to_string());

    auto& obj = *obj_val.as_object();
    auto it = obj.fields.find(n.GetMemberName());
    if (it == obj.fields.end())
        report_error(n.span, "EVAL_UNDECLARED_MEMBER",
                     n.GetMemberName(), obj.type_name);

    result_ = it->second;
}

void Evaluator::visit(MethodCall& n) {
    HulkValue obj_val = eval(n.GetObject());
    if (!obj_val.is_object())
        report_error(n.span, "EVAL_TYPE_NOT_OBJECT", obj_val.to_string());

    auto obj_ptr = obj_val.as_object();
    const std::string& type_name = obj_ptr->type_name;

    const MethodDef* method = find_method(type_name, n.GetMethodName());
    if (!method)
        report_error(n.span, "EVAL_UNDECLARED_MEMBER",
                     n.GetMethodName(), type_name);

    const auto& args = n.GetArgs();
    if (method->params.size() != args.size())
        report_error(n.span, "EVAL_ARITY_METHOD",
                     n.GetMethodName(), method->params.size(), args.size());

    // Evaluar argumentos
    std::vector<HulkValue> arg_vals;
    arg_vals.reserve(args.size());
    for (auto& arg : args)
        arg_vals.push_back(eval(arg.get()));

    // Crear scope del método: parámetros + self como variable
    auto method_env = std::make_shared<Environment>(env_);
    for (size_t i = 0; i < method->params.size(); ++i)
        method_env->define(method->params[i].name, arg_vals[i]);
    method_env->define("self", obj_val);

    HulkValue prev_self = self_;
    self_ = obj_val;
    auto prev_env = env_;
    env_ = method_env;

    result_ = eval(method->body);

    env_  = prev_env;
    self_ = prev_self;
}

void Evaluator::visit(SelfRef&) {
    result_ = self_;
}

void Evaluator::visit(BaseCall& n) {
    if (!self_.is_object())
        report_error_raw(n.span, "base() solo puede usarse dentro de un método");

    const std::string& current_type = self_.as_object()->type_name;
    auto it = types_.find(current_type);
    if (it == types_.end() || it->second.parent_name.empty())
        report_error(n.span, "EVAL_NO_PARENT", current_type);

    const std::string& parent_name = it->second.parent_name;
    auto pit = types_.find(parent_name);
    if (pit == types_.end())
        report_error(n.span, "EVAL_UNDECLARED_TYPE", parent_name);

    std::vector<HulkValue> arg_vals;
    for (auto& arg : n.GetArgs())
        arg_vals.push_back(eval(arg.get()));

    init_object(*self_.as_object(), pit->second, arg_vals);
    result_ = self_;
}

void Evaluator::visit(IsExpr& n) {
    HulkValue val = eval(n.GetExpr());
    if (!val.is_object()) {
        // Tipos primitivos
        const std::string& t = n.GetTypeName();
        if (t == "Number")  result_ = HulkValue(val.is_number());
        else if (t == "String")  result_ = HulkValue(val.is_string());
        else if (t == "Boolean") result_ = HulkValue(val.is_bool());
        else result_ = HulkValue(false);
        return;
    }
    // Verificar si el objeto es del tipo o de un subtipo
    std::string type = val.as_object()->type_name;
    while (!type.empty()) {
        if (type == n.GetTypeName()) { result_ = HulkValue(true); return; }
        auto it = types_.find(type);
        if (it == types_.end()) break;
        type = it->second.parent_name;
    }
    result_ = HulkValue(false);
}

void Evaluator::visit(AsExpr& n) {
    HulkValue val = eval(n.GetExpr());
    // Verificar que el cast es válido (mismo check que is)
    bool valid = false;
    if (val.is_object()) {
        std::string type = val.as_object()->type_name;
        while (!type.empty()) {
            if (type == n.GetTypeName()) { valid = true; break; }
            auto it = types_.find(type);
            if (it == types_.end()) break;
            type = it->second.parent_name;
        }
    }
    if (!valid)
        report_error(n.span, "EVAL_INVALID_CAST",
                     val.is_object() ? val.as_object()->type_name : val.to_string(),
                     n.GetTypeName());
    result_ = val;
}

// ============================================================================
// Vectores
// ============================================================================

void Evaluator::visit(VectorLiteral& n) {
    auto vec = std::make_shared<HulkVector>();
    for (auto& elem : n.GetElements())
        vec->push_back(eval(elem.get()));
    result_ = HulkValue(vec);
}

void Evaluator::visit(VectorIndex& n) {
    HulkValue vec_val = eval(n.GetVector());
    HulkValue idx_val = eval(n.GetIndex());

    if (!vec_val.is_vector())
        report_error(n.span, "EVAL_TYPE_NOT_VECTOR", vec_val.to_string());
    if (!idx_val.is_number())
        report_error(n.span, "EVAL_TYPE_ARITH", idx_val.to_string());

    auto& vec = *vec_val.as_vector();
    long long idx = (long long)idx_val.as_number();
    if (idx < 0 || idx >= (long long)vec.size())
        report_error(n.span, "EVAL_INDEX_OUT_OF_RANGE", idx, (long long)vec.size());

    result_ = vec[idx];
}

void Evaluator::visit(VectorGenerator& n) {
    HulkValue iterable = eval(n.GetIterable());
    if (!iterable.is_vector())
        report_error(n.span, "EVAL_TYPE_NOT_VECTOR", iterable.to_string());

    auto result_vec = std::make_shared<HulkVector>();
    auto prev_env = env_;
    for (auto& item : *iterable.as_vector()) {
        env_ = std::make_shared<Environment>(prev_env);
        env_->define(n.GetVarName(), item);
        result_vec->push_back(eval(n.GetBody()));
    }
    env_ = prev_env;
    result_ = HulkValue(result_vec);
}

} // namespace Hulk
