#include "asExpr.h"

namespace Hulk {

    AsExpr::AsExpr(std::unique_ptr<Expr> expr, const std::string& typeName)
        : expr(std::move(expr)), typeName(typeName) {}

    Expr* AsExpr::GetExpr() const { return expr.get(); }
    const std::string& AsExpr::GetTypeName() const { return typeName; }

    std::string AsExpr::ToString() const {
        return expr->ToString() + " as " + typeName;
    }

}
