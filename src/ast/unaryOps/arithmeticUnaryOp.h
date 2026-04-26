#ifndef ARITHMETIC_UNARY_OP_H
#define ARITHMETIC_UNARY_OP_H

#include "../abs_nodes/unaryOp.h"
#include <string>

namespace Hulk {

    enum class ArithUnaryType { Minus };

    class ArithmeticUnaryOp : public UnaryOp {
    private:
        ArithUnaryType type;

    public:
        ArithmeticUnaryOp(ArithUnaryType opType, std::unique_ptr<Expr> arg);

        ArithUnaryType GetType() const { return type; }

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif