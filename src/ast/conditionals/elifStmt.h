#ifndef ELIFSTMT_H
#define ELIFSTMT_H

#include "../abs_nodes/expr.h"
#include <memory>

namespace Hulk {

    // Par auxiliar (condicion, cuerpo) para cada rama elif.
    // No es un nodo del AST independiente: vive dentro de IfStmt.
    struct ElifBranch {
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Expr> body;

        ElifBranch(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> b)
            : condition(std::move(cond)), body(std::move(b)) {}
    };

}

#endif