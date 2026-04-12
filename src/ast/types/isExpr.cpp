#include "isExpr.h"

namespace Hulk {

    IsExpr::IsExpr(std::unique_ptr<ASTnode> expr, const std::string& typeName)
        : expr(std::move(expr)), typeName(typeName) {}

    ASTnode* IsExpr::GetExpr() const { return expr.get(); }
    const std::string& IsExpr::GetTypeName() const { return typeName; }

    std::string IsExpr::ToString() const {
        return expr->ToString() + " is " + typeName;
    }

}
