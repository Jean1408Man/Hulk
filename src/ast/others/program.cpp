#include "program.h"

namespace Hulk {

    Program::Program(std::vector<std::unique_ptr<Decl>> declarations,
                     std::unique_ptr<Expr> globalExpr)
        : declarations(std::move(declarations)), globalExpr(std::move(globalExpr)) {}

    const std::vector<std::unique_ptr<Decl>>& Program::GetDeclarations() const {
        return declarations;
    }

    Expr* Program::GetGlobalExpr() const { return globalExpr.get(); }

    std::string Program::ToString() const {
        std::string result;
        for (const auto& decl : declarations)
            result += decl->ToString() + "\n";
        result += globalExpr->ToString();
        return result;
    }

}
