#ifndef ARITHMETIC_BINOP_H
#define ARITHMETIC_BINOP_H

#include "../abs_nodes/binOp.h"

namespace Hulk {

    enum class ArithmeticOp { Plus, Minus, Mult, Div, Pow };

    class ArithmeticBinOp : public BinOp {
    private:
        ArithmeticOp op;

    public:
        ArithmeticBinOp(std::unique_ptr<ASTnode> left, ArithmeticOp operation, std::unique_ptr<ASTnode> right);

        ArithmeticOp GetOperator() const { return op; }
        
        std::string ToString() const override;
    };

}

#endif