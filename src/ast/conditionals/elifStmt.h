#ifndef ELIFSTMT_H
#define ELIFSTMT_H

#include "../abs_nodes/ast.h"
#include <memory>

namespace Hulk {

    // Par auxiliar (condicion, cuerpo) para cada rama elif.
    // No es un nodo del AST independiente: vive dentro de IfStmt.
    struct ElifBranch {
        std::unique_ptr<ASTnode> condition;
        std::unique_ptr<ASTnode> body;

        ElifBranch(std::unique_ptr<ASTnode> cond, std::unique_ptr<ASTnode> b)
            : condition(std::move(cond)), body(std::move(b)) {}
    };

}

#endif