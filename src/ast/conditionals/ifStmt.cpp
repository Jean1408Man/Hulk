#include "ifStmt.h"
#include <memory>

namespace Hulk {

    IfStmt::IfStmt(std::unique_ptr<ASTnode> condition,
                   std::unique_ptr<ASTnode> thenBranch,
                   std::vector<ElifBranch> elifBranches,
                   std::unique_ptr<ASTnode> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elifBranches(std::move(elifBranches)),
          elseBranch(std::move(elseBranch)) {}

    ASTnode* IfStmt::GetCondition() const { return condition.get(); }
    ASTnode* IfStmt::GetThenBranch() const { return thenBranch.get(); }
    const std::vector<ElifBranch>& IfStmt::GetElifBranches() const { return elifBranches; }
    ASTnode* IfStmt::GetElseBranch() const { return elseBranch.get(); }

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
