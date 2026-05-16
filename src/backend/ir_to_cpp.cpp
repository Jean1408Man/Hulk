#include "ir_to_cpp.h"

#include "codegen_error.h"
#include "name_mangler.h"

#include <iomanip>
#include <sstream>

namespace Hulk::Backend {

std::string IRToCppEmitter::emit(const IR::IRProgram& program) const {
    std::ostringstream out;
    out << "#include <backend/runtime/hulk_runtime.hpp>\n\n";
    out << emit_data(program);
    out << emit_prototypes(program);
    for (const auto& fn : program.functions) {
        out << emit_function(fn) << "\n";
    }
    out << emit_register_types(program) << "\n";
    out << emit_main(program);
    return out.str();
}

std::string IRToCppEmitter::emit_data(const IR::IRProgram& program) const {
    if (program.data.empty()) return "";

    std::ostringstream out;
    out << "namespace {\n";
    for (const auto& data : program.data) {
        out << "const std::string " << data_name(data.label) << " = "
            << cpp_string(data.value) << ";\n";
    }
    out << "} // namespace\n\n";
    return out.str();
}

std::string IRToCppEmitter::emit_prototypes(const IR::IRProgram& program) const {
    std::ostringstream out;
    for (const auto& fn : program.functions) {
        if (fn.kind == IR::IRFunctionKind::Method) {
            out << "hulk_rt::Value " << fn.name
                << "(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args);\n";
        } else {
            out << "hulk_rt::Value " << fn.name
                << "(const std::vector<hulk_rt::Value>& args);\n";
        }
    }
    out << "\n";
    return out.str();
}

std::string IRToCppEmitter::emit_function(const IR::IRFunction& fn) const {
    std::ostringstream out;
    const bool is_method = fn.kind == IR::IRFunctionKind::Method;
    if (is_method) {
        out << "hulk_rt::Value " << fn.name
            << "(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args) {\n";
    } else {
        out << "hulk_rt::Value " << fn.name
            << "(const std::vector<hulk_rt::Value>& args) {\n";
    }

    const std::size_t arity = is_method && !fn.params.empty()
                                  ? fn.params.size() - 1
                                  : fn.params.size();
    out << "    if (args.size() != " << arity
        << ") hulk_rt::fail(\"Runtime error: aridad incorrecta en "
        << NameMangler::sanitize(fn.source_name.empty() ? fn.name : fn.source_name)
        << ".\");\n";

    std::size_t arg_index = 0;
    if (is_method) {
        if (fn.params.empty()) {
            throw CodegenError("Backend IR: metodo sin parametro self.");
        }
        out << "    hulk_rt::Value " << fn.params[0] << "(self);\n";
        arg_index = 1;
    }

    for (std::size_t i = arg_index; i < fn.params.size(); ++i) {
        out << "    hulk_rt::Value " << fn.params[i] << " = args["
            << (i - arg_index) << "];\n";
    }
    for (const auto& local : fn.locals) {
        out << "    hulk_rt::Value " << local << ";\n";
    }
    if (!fn.params.empty() || !fn.locals.empty()) out << "\n";

    for (const auto& instr : fn.body) {
        out << emit_instr(instr);
    }
    out << "    return hulk_rt::Value();\n";
    out << "}\n";
    return out.str();
}

std::string IRToCppEmitter::emit_instr(const IR::IRInstr& instr) const {
    std::ostringstream out;
    switch (instr.op) {
        case IR::IROp::Nop:
            out << "    (void)0;\n";
            break;
        case IR::IROp::ConstNil:
            out << "    " << instr.dest << " = hulk_rt::Value();\n";
            break;
        case IR::IROp::ConstNumber:
            out << "    " << instr.dest << " = hulk_rt::Value("
                << cpp_number(instr.number_value) << ");\n";
            break;
        case IR::IROp::ConstBool:
            out << "    " << instr.dest << " = hulk_rt::Value("
                << (instr.bool_value ? "true" : "false") << ");\n";
            break;
        case IR::IROp::LoadData:
            out << "    " << instr.dest << " = hulk_rt::Value(" << data_name(instr.src1) << ");\n";
            break;
        case IR::IROp::Move:
            out << "    " << instr.dest << " = " << instr.src1 << ";\n";
            break;
        case IR::IROp::Add:
            out << "    " << instr.dest << " = hulk_rt::add(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Sub:
            out << "    " << instr.dest << " = hulk_rt::sub(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Mul:
            out << "    " << instr.dest << " = hulk_rt::mul(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Div:
            out << "    " << instr.dest << " = hulk_rt::div(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Mod:
            out << "    " << instr.dest << " = hulk_rt::mod(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Pow:
            out << "    " << instr.dest << " = hulk_rt::pow(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Neg:
            out << "    " << instr.dest << " = hulk_rt::neg(" << instr.src1 << ");\n";
            break;
        case IR::IROp::And:
            out << "    " << instr.dest << " = hulk_rt::Value(hulk_rt::truthy(" << instr.src1
                << ") && hulk_rt::truthy(" << instr.src2 << "));\n";
            break;
        case IR::IROp::Or:
            out << "    " << instr.dest << " = hulk_rt::Value(hulk_rt::truthy(" << instr.src1
                << ") || hulk_rt::truthy(" << instr.src2 << "));\n";
            break;
        case IR::IROp::Not:
            out << "    " << instr.dest << " = hulk_rt::logical_not(" << instr.src1 << ");\n";
            break;
        case IR::IROp::Equal:
            out << "    " << instr.dest << " = hulk_rt::equal(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::NotEqual:
            out << "    " << instr.dest << " = hulk_rt::not_equal(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Less:
            out << "    " << instr.dest << " = hulk_rt::less(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Greater:
            out << "    " << instr.dest << " = hulk_rt::greater(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::LessEqual:
            out << "    " << instr.dest << " = hulk_rt::less_equal(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::GreaterEqual:
            out << "    " << instr.dest << " = hulk_rt::greater_equal(" << instr.src1 << ", "
                << instr.src2 << ");\n";
            break;
        case IR::IROp::Concat:
            out << "    " << instr.dest << " = hulk_rt::concat(" << instr.src1 << ", "
                << instr.src2 << ", false);\n";
            break;
        case IR::IROp::ConcatSpace:
            out << "    " << instr.dest << " = hulk_rt::concat(" << instr.src1 << ", "
                << instr.src2 << ", true);\n";
            break;
        case IR::IROp::Label:
            out << instr.label << ":\n";
            out << "    (void)0;\n";
            break;
        case IR::IROp::Jump:
            out << "    goto " << instr.label << ";\n";
            break;
        case IR::IROp::JumpIfTrue:
            out << "    if (hulk_rt::truthy(" << instr.src1 << ")) goto "
                << instr.label << ";\n";
            break;
        case IR::IROp::JumpIfFalse:
            out << "    if (!hulk_rt::truthy(" << instr.src1 << ")) goto "
                << instr.label << ";\n";
            break;
        case IR::IROp::Call:
            out << "    " << instr.dest << " = " << instr.callee << "("
                << args_vector(instr.args) << ");\n";
            break;
        case IR::IROp::Return:
            out << "    return " << instr.src1 << ";\n";
            break;
        case IR::IROp::NewObject:
            out << "    " << instr.dest
                << " = hulk_rt::Value(std::make_shared<hulk_rt::Object>("
                << cpp_string(instr.type_name) << "));\n";
            break;
        case IR::IROp::GetField:
            out << "    " << instr.dest << " = hulk_rt::get_field(" << instr.src1
                << ", " << cpp_string(instr.field_name) << ");\n";
            break;
        case IR::IROp::DefineField:
            out << "    hulk_rt::as_object(" << instr.src1 << ")->fields["
                << cpp_string(instr.field_name) << "] = " << instr.src2 << ";\n";
            out << "    " << instr.dest << " = " << instr.src2 << ";\n";
            break;
        case IR::IROp::SetField:
            out << "    " << instr.dest << " = hulk_rt::set_field(" << instr.src1
                << ", " << cpp_string(instr.field_name) << ", " << instr.src2 << ");\n";
            break;
        case IR::IROp::VCall:
            out << "    " << instr.dest << " = hulk_rt::call_method(" << instr.src1
                << ", " << cpp_string(instr.method_name) << ", "
                << args_vector(instr.args) << ");\n";
            break;
        case IR::IROp::SCallMethod:
            out << "    " << instr.dest << " = hulk_rt::call_method_from(" << instr.src1
                << ", " << cpp_string(instr.type_name) << ", "
                << cpp_string(instr.method_name) << ", " << args_vector(instr.args) << ");\n";
            break;
        case IR::IROp::IsType:
            out << "    " << instr.dest << " = hulk_rt::Value(hulk_rt::is_instance("
                << instr.src1 << ", " << cpp_string(instr.type_name) << "));\n";
            break;
        case IR::IROp::AsType:
            out << "    " << instr.dest << " = hulk_rt::checked_cast(" << instr.src1
                << ", " << cpp_string(instr.type_name) << ");\n";
            break;
        case IR::IROp::BuiltinPrint:
            out << "    " << instr.dest << " = hulk_rt::hulk_print(" << instr.args.at(0) << ");\n";
            break;
        case IR::IROp::BuiltinSqrt:
            out << "    " << instr.dest << " = hulk_rt::hulk_sqrt(" << instr.args.at(0) << ");\n";
            break;
        case IR::IROp::BuiltinSin:
            out << "    " << instr.dest << " = hulk_rt::hulk_sin(" << instr.args.at(0) << ");\n";
            break;
        case IR::IROp::BuiltinCos:
            out << "    " << instr.dest << " = hulk_rt::hulk_cos(" << instr.args.at(0) << ");\n";
            break;
        case IR::IROp::BuiltinExp:
            out << "    " << instr.dest << " = hulk_rt::hulk_exp(" << instr.args.at(0) << ");\n";
            break;
        case IR::IROp::BuiltinLog:
            out << "    " << instr.dest << " = hulk_rt::hulk_log(" << instr.args.at(0)
                << ", " << instr.args.at(1) << ");\n";
            break;
        case IR::IROp::BuiltinRand:
            out << "    " << instr.dest << " = hulk_rt::hulk_rand();\n";
            break;
    }
    return out.str();
}

std::string IRToCppEmitter::emit_register_types(const IR::IRProgram& program) const {
    std::ostringstream out;
    out << "void hulk_register_types() {\n";
    for (const auto& type : program.types) {
        out << "    hulk_rt::register_type(" << cpp_string(type.name)
            << ", " << cpp_string(type.parent) << ");\n";
    }
    for (const auto& type : program.types) {
        for (const auto& method : type.methods) {
            out << "    hulk_rt::register_method(" << cpp_string(type.name)
                << ", " << cpp_string(method.name)
                << ", " << method.function_name
                << ", " << method.arity << ");\n";
        }
    }
    out << "}\n";
    return out.str();
}

std::string IRToCppEmitter::emit_main(const IR::IRProgram& program) const {
    std::ostringstream out;
    out << "int main() {\n";
    out << "    try {\n";
    out << "        hulk_register_types();\n";
    out << "        (void)" << program.entry_function << "({});\n";
    out << "        return 0;\n";
    out << "    } catch (const hulk_rt::RuntimeError& err) {\n";
    out << "        std::cerr << err.what() << \"\\n\";\n";
    out << "        return 1;\n";
    out << "    }\n";
    out << "}\n";
    return out.str();
}

std::string IRToCppEmitter::cpp_string(const std::string& value) const {
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

std::string IRToCppEmitter::cpp_number(double value) const {
    std::ostringstream out;
    out << std::setprecision(17) << value;
    std::string literal = out.str();
    if (literal.find('.') == std::string::npos &&
        literal.find('e') == std::string::npos &&
        literal.find('E') == std::string::npos) {
        literal += ".0";
    }
    return literal;
}

std::string IRToCppEmitter::args_vector(const std::vector<std::string>& args) const {
    std::ostringstream out;
    out << "std::vector<hulk_rt::Value>{";
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i > 0) out << ", ";
        out << args[i];
    }
    out << "}";
    return out.str();
}

std::string IRToCppEmitter::data_name(const std::string& label) const {
    return "hulk_data_" + NameMangler::sanitize(label);
}

} // namespace Hulk::Backend
