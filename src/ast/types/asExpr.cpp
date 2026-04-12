#include "asExpr.h"

namespace Hulk {

    AsExpr::AsExpr(std::unique_ptr<ASTnode> expr, const std::string& typeName)
        : expr(std::move(expr)), typeName(typeName) {}

    ASTnode* AsExpr::GetExpr() const { return expr.get(); }
    const std::string& AsExpr::GetTypeName() const { return typeName; }

    std::string AsExpr::ToString() const {
        return expr->ToString() + " as " + typeName;
    }

}
