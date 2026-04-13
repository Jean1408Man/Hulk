#include "arithmeticUnaryOp.h"

namespace Hulk {

    ArithmeticUnaryOp::ArithmeticUnaryOp(ArithUnaryType opType, std::unique_ptr<Expr> arg)
        : UnaryOp(std::move(arg)), type(opType) {}

    std::string ArithmeticUnaryOp::ToString() const {
        return "-" + operand->ToString();
    }

}