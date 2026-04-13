#include "ifStmt.h"
#include <memory>

namespace Hulk {

    IfStmt::IfStmt(std::unique_ptr<Expr> condition,
                   std::unique_ptr<Expr> thenBranch,
                   std::vector<ElifBranch> elifBranches,
                   std::unique_ptr<Expr> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elifBranches(std::move(elifBranches)),
          elseBranch(std::move(elseBranch)) {}

    Expr* IfStmt::GetCondition() const { return condition.get(); }
    Expr* IfStmt::GetThenBranch() const { return thenBranch.get(); }
    const std::vector<ElifBranch>& IfStmt::GetElifBranches() const { return elifBranches; }
    Expr* IfStmt::GetElseBranch() const { return elseBranch.get(); }

    std::string IfStmt::ToString() const {
        std::string result = "if (" + condition->ToString() + ") "
                           + thenBranch->ToString();
        for (const auto& elif : elifBranches)
            result += " elif (" + elif.condition->ToString() + ") "
                    + elif.body->ToString();
        result += " else " + elseBranch->ToString();
        return result;
    }

}
