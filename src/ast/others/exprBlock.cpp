#include "exprBlock.h"

namespace Hulk {

    ExprBlock::ExprBlock(std::vector<std::unique_ptr<ASTnode>> exprs)
        : exprs(std::move(exprs)) {}

    const std::vector<std::unique_ptr<ASTnode>>& ExprBlock::GetExprs() const { return exprs; }

    ASTnode* ExprBlock::GetLast() const {
        if (exprs.empty()) return nullptr;
        return exprs.back().get();
    }

    std::string ExprBlock::ToString() const {
        std::string result = "{ ";
        for (const auto& e : exprs)
            result += e->ToString() + "; ";
        result += "}";
        return result;
    }

}
