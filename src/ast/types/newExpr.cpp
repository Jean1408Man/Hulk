#include "newExpr.h"

namespace Hulk {

    NewExpr::NewExpr(const std::string& typeName,
                     std::vector<std::unique_ptr<ASTnode>> args)
        : typeName(typeName), args(std::move(args)) {}

    const std::string& NewExpr::GetTypeName() const { return typeName; }
    const std::vector<std::unique_ptr<ASTnode>>& NewExpr::GetArgs() const { return args; }

    std::string NewExpr::ToString() const {
        std::string result = "new " + typeName + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->ToString();
        }
        result += ")";
        return result;
    }

}
