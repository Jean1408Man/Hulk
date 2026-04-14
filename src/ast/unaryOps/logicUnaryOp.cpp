#include "logicUnaryOp.h"

namespace Hulk {

    LogicUnaryOp::LogicUnaryOp(std::unique_ptr<Expr> arg) 
        : UnaryOp(std::move(arg)) {}

    std::string LogicUnaryOp::ToString() const {
        return "!(" + operand->ToString() + ")";
    }

}
