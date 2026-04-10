#ifndef BINOP_H
#define BINOP_H

#include "ast.h"
#include <memory>
#include <string>

namespace Hulk {

    // Clase base para cualquier operación binaria
    class BinOp : public ASTnode {
    protected:
        std::unique_ptr<ASTnode> left;
        std::unique_ptr<ASTnode> right;

    public:
        BinOp(std::unique_ptr<ASTnode> leftNode, std::unique_ptr<ASTnode> rightNode)
            : left(std::move(leftNode)), right(std::move(rightNode)) {}

        virtual ~BinOp() = default;

        // Getters para las subclases o el evaluador
        ASTnode* GetLeft() const { return left.get(); }
        ASTnode* GetRight() const { return right.get(); }
    };

}

#endif