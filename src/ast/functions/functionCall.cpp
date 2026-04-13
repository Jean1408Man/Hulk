#include "functionCall.h"

namespace Hulk {

    FunctionCall::FunctionCall(const std::string& name,
                               std::vector<std::unique_ptr<Expr>> args)
        : name(name), args(std::move(args)) {}

    const std::string& FunctionCall::GetName() const { return name; }
    const std::vector<std::unique_ptr<Expr>>& FunctionCall::GetArgs() const { return args; }

    std::string FunctionCall::ToString() const {
        std::string result = name + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->ToString();
        }
        result += ")";
        return result;
    }

}
