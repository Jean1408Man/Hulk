#include "methodCall.h"

namespace Hulk {

    MethodCall::MethodCall(std::unique_ptr<Expr> object,
                           const std::string& methodName,
                           std::vector<std::unique_ptr<Expr>> args)
        : object(std::move(object)), methodName(methodName), args(std::move(args)) {}

    Expr* MethodCall::GetObject() const { return object.get(); }
    const std::string& MethodCall::GetMethodName() const { return methodName; }
    const std::vector<std::unique_ptr<Expr>>& MethodCall::GetArgs() const { return args; }

    std::string MethodCall::ToString() const {
        std::string result = object->ToString() + "." + methodName + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->ToString();
        }
        result += ")";
        return result;
    }

}
