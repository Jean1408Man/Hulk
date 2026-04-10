#ifndef UNARY_OP_H
#define UNARY_OP_H

#include "ast.h"
#include <memory>

namespace Hulk {

    class UnaryOp : public ASTnode {
    protected:
        std::unique_ptr<ASTnode> operand; // El único hijo del nodo

    public:
        // El constructor recibe la propiedad del nodo operando
        explicit UnaryOp(std::unique_ptr<ASTnode> arg) 
            : operand(std::move(arg)) {}

        virtual ~UnaryOp() = default;

        // Getter para inspeccionar el operando
        ASTnode* GetOperand() const {
            return operand.get();
        }
    };

}

#endif