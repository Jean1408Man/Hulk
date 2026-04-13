#include "for.h"

namespace Hulk {

    For::For(const std::string& varName, std::unique_ptr<Expr> iterable, std::unique_ptr<Expr> body)
        : varName(varName), iterable(std::move(iterable)), body(std::move(body)) {}

    const std::string& For::GetVarName() const {
        return varName;
    }

    Expr* For::GetIterable() const {
        return iterable.get();
    }

    Expr* For::GetBody() const {
        return body.get();
    }

    std::string For::ToString() const {
        return "for " + varName + " in " + iterable->ToString() + ":\n" + body->ToString();
    }
}