#ifndef HULK_IR_IR_H
#define HULK_IR_IR_H

#include <cstddef>
#include <string>
#include <vector>

namespace Hulk::IR {

enum class IROp {
    Nop,
    ConstNil,
    ConstNumber,
    ConstBool,
    LoadData,
    Move,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Pow,
    Neg,
    And,
    Or,
    Not,
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    Concat,
    ConcatSpace,
    Label,
    Jump,
    JumpIfTrue,
    JumpIfFalse,
    Call,
    Return,
    NewObject,
    GetField,
    DefineField,
    SetField,
    VCall,
    SCallMethod,
    IsType,
    AsType,
    BuiltinPrint,
    BuiltinSqrt,
    BuiltinSin,
    BuiltinCos,
    BuiltinExp,
    BuiltinLog,
    BuiltinRand
};

enum class IRFunctionKind {
    Entry,
    Global,
    Initializer,
    Method
};

struct IRField {
    std::string owner_type;
    std::string name;
    std::string lowered_name;
    std::string type_name = "Object";
    int slot = -1;
};

struct IRMethod {
    std::string owner_type;
    std::string name;
    std::string function_name;
    std::size_t arity = 0;
    int slot = -1;
};

struct IRType {
    std::string name;
    std::string parent = "Object";
    std::string init_name;
    std::string ctor_name;
    std::vector<IRField> fields;
    std::vector<IRMethod> methods;
};

struct IRData {
    std::string label;
    std::string value;
};

struct IRInstr {
    IROp op = IROp::Nop;
    std::string dest;
    std::string src1;
    std::string src2;
    std::string label;
    std::string callee;
    std::string type_name;
    std::string field_name;
    std::string method_name;
    std::vector<std::string> args;
    double number_value = 0.0;
    bool bool_value = false;
};

struct IRFunction {
    std::string name;
    std::string source_name;
    IRFunctionKind kind = IRFunctionKind::Global;
    std::vector<std::string> params;
    std::vector<std::string> locals;
    std::vector<IRInstr> body;
};

struct IRProgram {
    std::vector<IRType> types;
    std::vector<IRData> data;
    std::vector<IRFunction> functions;
    std::string entry_function = "hulk_main";
};

std::string op_name(IROp op);
std::string function_kind_name(IRFunctionKind kind);

} // namespace Hulk::IR

#endif
