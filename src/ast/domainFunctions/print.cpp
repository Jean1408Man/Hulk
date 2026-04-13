#include "print.h"
#include <utility> // Para std::move

namespace Hulk {

    Print::Print(std::unique_ptr<Expr> expression) 
        : expr(std::move(expression)) {}

    Expr* Print::GetExpr() const {
        return expr.get();
    }

    std::string Print::ToString() const {
        // Representación visual: print(<expr>)
        return "print(" + (expr ? expr->ToString() : "null") + ")";
    }

}