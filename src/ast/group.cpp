#include "group.h"
#include <utility> // Para std::move

namespace Hulk {

    Group::Group(std::unique_ptr<ASTnode> expression) 
        : expr(std::move(expression)) {}

    ASTnode* Group::GetExpr() const {
        return expr.get();
    }

    std::string Group::ToString() const {
        // Representación visual: (<expr>)
        return "(" + (expr ? expr->ToString() : "null") + ")";
    }

}