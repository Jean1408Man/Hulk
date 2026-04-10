#include "for.h"

namespace Hulk {

    For::For(const std::string& varName, std::unique_ptr<ASTnode> iterable, std::unique_ptr<ASTnode> body)
        : varName(varName), iterable(std::move(iterable)), body(std::move(body)) {}

    const std::string& For::GetVarName() const {
        return varName;
    }

    ASTnode* For::GetIterable() const {
        return iterable.get();
    }

    ASTnode* For::GetBody() const {
        return body.get();
    }

    std::string For::ToString() const {
        return "for " + varName + " in " + iterable->ToString() + ":\n" + body->ToString();
    }
}