#include "logicBinOp.h"

namespace Hulk {

    LogicBinOp::LogicBinOp(std::unique_ptr<Expr> left, LogicOp operation, std::unique_ptr<Expr> right)
        : BinOp(std::move(left), std::move(right)), op(operation) {}

    std::string LogicBinOp::ToString() const {
        std::string s;
        switch(op) {
            case LogicOp::And:    s = "&";  break;
            case LogicOp::Or:     s = "|";  break;
            case LogicOp::Equal:  s = "=="; break;
            case LogicOp::NotEqual: s = "!="; break;
            case LogicOp::Greater: s = ">"; break;
            case LogicOp::Less: s = "<"; break;
            case LogicOp::GreaterEqual: s = ">="; break;
            case LogicOp::LessEqual: s = "<="; break;
            default:              s = "op"; break;
        }
        return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
    }
}
