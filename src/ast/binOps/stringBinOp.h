#ifndef STRING_BINOP_H
#define STRING_BINOP_H

#include "../abs_nodes/binOp.h"

namespace Hulk {

    enum class StringOp { Concat, SpaceConcat };

    class StringBinOp : public BinOp {
    private:
        StringOp op;

    public:
        StringBinOp(std::unique_ptr<Expr> left, StringOp operation, std::unique_ptr<Expr> right);

        StringOp GetOperator() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
