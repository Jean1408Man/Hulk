#include "arithmeticBinOp.h"

namespace Hulk {

    ArithmeticBinOp::ArithmeticBinOp(std::unique_ptr<ASTnode> left, ArithmeticOp operation, std::unique_ptr<ASTnode> right)
        : BinOp(std::move(left), std::move(right)), op(operation) {}

    std::string ArithmeticBinOp::ToString() const {
        std::string s;
        switch(op) {
            case ArithmeticOp::Plus: s = "+"; break;
            case ArithmeticOp::Minus: s = "-"; break;
            case ArithmeticOp::Mult: s = "*"; break;
            case ArithmeticOp::Div: s = "/"; break;
            case ArithmeticOp::Pow: s = "^"; break;
        }
        return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
    }
}