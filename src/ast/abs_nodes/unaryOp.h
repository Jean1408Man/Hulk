#ifndef UNARY_OP_H
#define UNARY_OP_H

#include "expr.h"
#include <memory>

namespace Hulk {

    class UnaryOp : public Expr {
    protected:
        std::unique_ptr<Expr> operand; // El único hijo del nodo

    public:
        // El constructor recibe la propiedad del nodo operando
        explicit UnaryOp(std::unique_ptr<Expr> arg) 
            : operand(std::move(arg)) {}

        virtual ~UnaryOp() = default;

        // Getter para inspeccionar el operando
        Expr* GetOperand() const {
            return operand.get();
        }
    };

}

#endif