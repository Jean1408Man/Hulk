#include "while.h"

namespace Hulk {

    WhileStmt::WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    Expr* WhileStmt::GetCondition() const { return condition.get(); }
    Expr* WhileStmt::GetBody() const { return body.get(); }

    std::string WhileStmt::ToString() const {
        return "while (" + condition->ToString() + ") " + body->ToString();
    }

}
