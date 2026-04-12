#include "builtinCall.h"

namespace Hulk {

    BuiltinCall::BuiltinCall(BuiltinFunc func,
                             std::vector<std::unique_ptr<ASTnode>> args)
        : func(func), args(std::move(args)) {}

    BuiltinFunc BuiltinCall::GetFunc() const { return func; }
    const std::vector<std::unique_ptr<ASTnode>>& BuiltinCall::GetArgs() const { return args; }

    std::string BuiltinCall::ToString() const {
        std::string name;
        switch (func) {
            case BuiltinFunc::Exp:   name = "exp";   break;
            case BuiltinFunc::Log:   name = "log";   break;
            case BuiltinFunc::Rand:  name = "rand";  break;
            case BuiltinFunc::Range: name = "range"; break;
        }
        std::string result = name + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->ToString();
        }
        result += ")";
        return result;
    }

}
