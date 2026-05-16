#ifndef HULK_BACKEND_IR_TO_CPP_H
#define HULK_BACKEND_IR_TO_CPP_H

#include "../ir/ir.h"

#include <string>
#include <vector>

namespace Hulk::Backend {

class IRToCppEmitter {
public:
    std::string emit(const IR::IRProgram& program) const;

private:
    std::string emit_prototypes(const IR::IRProgram& program) const;
    std::string emit_data(const IR::IRProgram& program) const;
    std::string emit_function(const IR::IRFunction& fn) const;
    std::string emit_register_types(const IR::IRProgram& program) const;
    std::string emit_main(const IR::IRProgram& program) const;
    std::string emit_instr(const IR::IRInstr& instr) const;

    std::string cpp_string(const std::string& value) const;
    std::string cpp_number(double value) const;
    std::string args_vector(const std::vector<std::string>& args) const;
    std::string data_name(const std::string& label) const;
};

} // namespace Hulk::Backend

#endif
