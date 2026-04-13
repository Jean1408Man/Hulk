#ifndef BINOP_H
#define BINOP_H

#include "expr.h"
#include <memory>
#include <string>

namespace Hulk {

    // Clase base para cualquier operación binaria
    class BinOp : public Expr {
    protected:
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;

    public:
        BinOp(std::unique_ptr<Expr> leftNode, std::unique_ptr<Expr> rightNode)
            : left(std::move(leftNode)), right(std::move(rightNode)) {}

        virtual ~BinOp() = default;

        // Getters para las subclases o el evaluador
        Expr* GetLeft() const { return left.get(); }
        Expr* GetRight() const { return right.get(); }
    };

}

#endif