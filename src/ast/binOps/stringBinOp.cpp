#include "stringBinOp.h"

namespace Hulk {

    StringBinOp::StringBinOp(std::unique_ptr<ASTnode> left, StringOp operation, std::unique_ptr<ASTnode> right)
        : BinOp(std::move(left), std::move(right)), op(operation) {}

    StringOp StringBinOp::GetOperator() const {
        return op;
    }

    std::string StringBinOp::ToString() const {
        std::string s;
        switch(op) {
            case StringOp::Concat: s = "@"; 
            return "(" + left->ToString() + s + right->ToString() + ")";
            case StringOp::SpaceConcat: s = "@@";
            return "(" + left->ToString() + " " + s + " " + right->ToString() + ")"; 
            default:              s = "op"; break;
        }
        return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
    }
}