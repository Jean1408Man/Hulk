#ifndef HULK_IR_IR_PRINTER_H
#define HULK_IR_IR_PRINTER_H

#include "ir.h"

#include <string>

namespace Hulk::IR {

class IRPrinter {
public:
    std::string print(const IRProgram& program) const;

private:
    std::string print_instr(const IRInstr& instr) const;
    std::string escape_string(const std::string& value) const;
    std::string join_args(const std::vector<std::string>& args) const;
};

} // namespace Hulk::IR

#endif
