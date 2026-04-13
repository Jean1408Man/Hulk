#include "letIn.h"

namespace Hulk {

    LetIn::LetIn(std::vector<std::unique_ptr<VariableBinding>> bindings,
                 std::unique_ptr<Expr> body)
        : bindings(std::move(bindings)), body(std::move(body)) {}

    const std::vector<std::unique_ptr<VariableBinding>>& LetIn::GetBindings() const {
        return bindings;
    }

    Expr* LetIn::GetBody() const { return body.get(); }

    std::string LetIn::ToString() const {
        std::string result = "let ";
        for (size_t i = 0; i < bindings.size(); ++i) {
            if (i > 0) result += ", ";
            result += bindings[i]->ToString();
        }
        result += " in " + body->ToString();
        return result;
    }

}
