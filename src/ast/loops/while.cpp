#include "while.h"

namespace Hulk {

    WhileStmt::WhileStmt(std::unique_ptr<ASTnode> condition, std::unique_ptr<ASTnode> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    ASTnode* WhileStmt::GetCondition() const { return condition.get(); }
    ASTnode* WhileStmt::GetBody() const { return body.get(); }

    std::string WhileStmt::ToString() const {
        return "while (" + condition->ToString() + ") " + body->ToString();
    }

}
