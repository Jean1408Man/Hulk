#include "arithmeticUnaryOp.h"

namespace Hulk {

    ArithmeticUnaryOp::ArithmeticUnaryOp(ArithUnaryType opType, std::unique_ptr<ASTnode> arg)
        : UnaryOp(std::move(arg)), type(opType) {}

    std::string ArithmeticUnaryOp::ToString() const {
        std::string opName;
        switch (type) {
            case ArithUnaryType::Minus: return "-" + operand->ToString();
            case ArithUnaryType::Sin:   opName = "sin"; break;
            case ArithUnaryType::Cos:   opName = "cos"; break;
            case ArithUnaryType::Sqrt:  opName = "sqrt"; break;
        }
        return opName + "(" + operand->ToString() + ")";
    }

}