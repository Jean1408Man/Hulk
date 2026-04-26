#ifndef LOGIC_BINOP_H
#define LOGIC_BINOP_H

#include "../abs_nodes/binOp.h"

namespace Hulk {

    enum class LogicOp { And, Or, Greater, Less, Equal, NotEqual, GreaterEqual, LessEqual };

    class LogicBinOp : public BinOp {
    private:
        LogicOp op;

    public:
        LogicBinOp(std::unique_ptr<Expr> left, LogicOp operation, std::unique_ptr<Expr> right);

        LogicOp GetOperator() const { return op; }

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
