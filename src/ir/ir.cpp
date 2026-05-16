#include "ir.h"

namespace Hulk::IR {

std::string op_name(IROp op) {
    switch (op) {
        case IROp::Nop: return "nop";
        case IROp::ConstNil: return "const_nil";
        case IROp::ConstNumber: return "const_number";
        case IROp::ConstBool: return "const_bool";
        case IROp::LoadData: return "load_data";
        case IROp::Move: return "move";
        case IROp::Add: return "add";
        case IROp::Sub: return "sub";
        case IROp::Mul: return "mul";
        case IROp::Div: return "div";
        case IROp::Mod: return "mod";
        case IROp::Pow: return "pow";
        case IROp::Neg: return "neg";
        case IROp::And: return "and";
        case IROp::Or: return "or";
        case IROp::Not: return "not";
        case IROp::Equal: return "eq";
        case IROp::NotEqual: return "neq";
        case IROp::Less: return "lt";
        case IROp::Greater: return "gt";
        case IROp::LessEqual: return "leq";
        case IROp::GreaterEqual: return "geq";
        case IROp::Concat: return "concat";
        case IROp::ConcatSpace: return "concat_sp";
        case IROp::Label: return "label";
        case IROp::Jump: return "jump";
        case IROp::JumpIfTrue: return "jump_if_true";
        case IROp::JumpIfFalse: return "jump_if_false";
        case IROp::Call: return "call";
        case IROp::Return: return "return";
        case IROp::NewObject: return "new_object";
        case IROp::GetField: return "getfield";
        case IROp::DefineField: return "definefield";
        case IROp::SetField: return "setfield";
        case IROp::VCall: return "vcall";
        case IROp::SCallMethod: return "scall_method";
        case IROp::IsType: return "is_type";
        case IROp::AsType: return "as_type";
        case IROp::BuiltinPrint: return "builtin_print";
        case IROp::BuiltinSqrt: return "builtin_sqrt";
        case IROp::BuiltinSin: return "builtin_sin";
        case IROp::BuiltinCos: return "builtin_cos";
        case IROp::BuiltinExp: return "builtin_exp";
        case IROp::BuiltinLog: return "builtin_log";
        case IROp::BuiltinRand: return "builtin_rand";
    }
    return "unknown";
}

std::string function_kind_name(IRFunctionKind kind) {
    switch (kind) {
        case IRFunctionKind::Entry: return "entry";
        case IRFunctionKind::Global: return "global";
        case IRFunctionKind::Initializer: return "initializer";
        case IRFunctionKind::Method: return "method";
    }
    return "unknown";
}

} // namespace Hulk::IR
