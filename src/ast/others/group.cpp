#include "group.h"
#include <utility> // Para std::move

namespace Hulk {

    Group::Group(std::unique_ptr<Expr> expression) 
        : expr(std::move(expression)) {}

    Expr* Group::GetExpr() const {
        return expr.get();
    }

    std::string Group::ToString() const {
        // Representación visual: (<expr>)
        return "(" + (expr ? expr->ToString() : "null") + ")";
    }

}