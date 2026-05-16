#include "ir_printer.h"

#include <iomanip>
#include <sstream>

namespace Hulk::IR {

std::string IRPrinter::print(const IRProgram& program) const {
    std::ostringstream out;

    out << ".TYPES\n";
    for (const auto& type : program.types) {
        out << "type " << type.name << " parent " << type.parent << " {\n";
        for (const auto& field : type.fields) {
            out << "  field " << field.lowered_name << " : " << field.type_name
                << " slot " << field.slot << "\n";
        }
        for (const auto& method : type.methods) {
            out << "  method " << method.name << " -> " << method.function_name
                << " slot " << method.slot << "\n";
        }
        out << "}\n\n";
    }

    out << ".DATA\n";
    for (const auto& data : program.data) {
        out << data.label << " = " << escape_string(data.value) << "\n";
    }

    out << "\n.CODE\n";
    for (const auto& fn : program.functions) {
        out << "function " << fn.name << "(" << join_args(fn.params) << ")"
            << " ; kind=" << function_kind_name(fn.kind) << "\n";
        out << "{\n";
        for (const auto& local : fn.locals) {
            out << "  local " << local << "\n";
        }
        for (const auto& instr : fn.body) {
            out << "  " << print_instr(instr) << "\n";
        }
        out << "}\n\n";
    }

    return out.str();
}

std::string IRPrinter::print_instr(const IRInstr& instr) const {
    std::ostringstream out;
    switch (instr.op) {
        case IROp::Nop:
            out << "nop";
            break;
        case IROp::ConstNil:
            out << instr.dest << " = const_nil";
            break;
        case IROp::ConstNumber:
            out << instr.dest << " = const_number " << std::setprecision(17)
                << instr.number_value;
            break;
        case IROp::ConstBool:
            out << instr.dest << " = const_bool " << (instr.bool_value ? "true" : "false");
            break;
        case IROp::LoadData:
            out << instr.dest << " = load_data " << instr.src1;
            break;
        case IROp::Move:
            out << instr.dest << " = move " << instr.src1;
            break;
        case IROp::Add:
        case IROp::Sub:
        case IROp::Mul:
        case IROp::Div:
        case IROp::Mod:
        case IROp::Pow:
        case IROp::And:
        case IROp::Or:
        case IROp::Equal:
        case IROp::NotEqual:
        case IROp::Less:
        case IROp::Greater:
        case IROp::LessEqual:
        case IROp::GreaterEqual:
        case IROp::Concat:
        case IROp::ConcatSpace:
            out << instr.dest << " = " << op_name(instr.op) << " "
                << instr.src1 << " " << instr.src2;
            break;
        case IROp::Neg:
        case IROp::Not:
            out << instr.dest << " = " << op_name(instr.op) << " " << instr.src1;
            break;
        case IROp::Label:
            out << "label " << instr.label;
            break;
        case IROp::Jump:
            out << "jump " << instr.label;
            break;
        case IROp::JumpIfTrue:
        case IROp::JumpIfFalse:
            out << op_name(instr.op) << " " << instr.src1 << " " << instr.label;
            break;
        case IROp::Call:
            out << instr.dest << " = call " << instr.callee << "("
                << join_args(instr.args) << ")";
            break;
        case IROp::Return:
            out << "return " << instr.src1;
            break;
        case IROp::NewObject:
            out << instr.dest << " = new_object " << instr.type_name;
            break;
        case IROp::GetField:
            out << instr.dest << " = getfield " << instr.src1 << " " << instr.field_name;
            break;
        case IROp::DefineField:
            out << instr.dest << " = definefield " << instr.src1 << " "
                << instr.field_name << " " << instr.src2;
            break;
        case IROp::SetField:
            out << instr.dest << " = setfield " << instr.src1 << " "
                << instr.field_name << " " << instr.src2;
            break;
        case IROp::VCall:
            out << instr.dest << " = vcall " << instr.src1 << "." << instr.method_name
                << "(" << join_args(instr.args) << ")";
            break;
        case IROp::SCallMethod:
            out << instr.dest << " = scall_method " << instr.type_name << "."
                << instr.method_name << "(" << instr.src1;
            if (!instr.args.empty()) out << ", " << join_args(instr.args);
            out << ")";
            break;
        case IROp::IsType:
        case IROp::AsType:
            out << instr.dest << " = " << op_name(instr.op) << " "
                << instr.src1 << " " << instr.type_name;
            break;
        case IROp::BuiltinPrint:
        case IROp::BuiltinSqrt:
        case IROp::BuiltinSin:
        case IROp::BuiltinCos:
        case IROp::BuiltinExp:
        case IROp::BuiltinLog:
        case IROp::BuiltinRand:
            out << instr.dest << " = " << op_name(instr.op) << "("
                << join_args(instr.args) << ")";
            break;
    }
    return out.str();
}

std::string IRPrinter::escape_string(const std::string& value) const {
    std::ostringstream out;
    out << '"';
    for (unsigned char ch : value) {
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

std::string IRPrinter::join_args(const std::vector<std::string>& args) const {
    std::ostringstream out;
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i > 0) out << ", ";
        out << args[i];
    }
    return out.str();
}

} // namespace Hulk::IR
