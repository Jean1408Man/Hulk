#include "baseCall.h"

namespace Hulk {

    BaseCall::BaseCall(std::vector<std::unique_ptr<ASTnode>> args)
        : args(std::move(args)) {}

    const std::vector<std::unique_ptr<ASTnode>>& BaseCall::GetArgs() const { return args; }

    std::string BaseCall::ToString() const {
        std::string result = "base(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->ToString();
        }
        result += ")";
        return result;
    }

}
