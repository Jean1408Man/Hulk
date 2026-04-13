#include "isExpr.h"

namespace Hulk {

    IsExpr::IsExpr(std::unique_ptr<Expr> expr, const std::string& typeName)
        : expr(std::move(expr)), typeName(typeName) {}

    Expr* IsExpr::GetExpr() const { return expr.get(); }
    const std::string& IsExpr::GetTypeName() const { return typeName; }

    std::string IsExpr::ToString() const {
        return expr->ToString() + " is " + typeName;
    }

}
