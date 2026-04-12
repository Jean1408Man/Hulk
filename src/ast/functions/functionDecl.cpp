#include "functionDecl.h"

namespace Hulk {

    FunctionDecl::FunctionDecl(const std::string& name,
                               std::vector<Param> params,
                               std::unique_ptr<ASTnode> body)
        : name(name), params(std::move(params)), returnTypeAnnotation(""), body(std::move(body)) {}

    FunctionDecl::FunctionDecl(const std::string& name,
                               std::vector<Param> params,
                               const std::string& returnTypeAnnotation,
                               std::unique_ptr<ASTnode> body)
        : name(name), params(std::move(params)), returnTypeAnnotation(returnTypeAnnotation), body(std::move(body)) {}

    const std::string& FunctionDecl::GetName() const { return name; }
    const std::vector<Param>& FunctionDecl::GetParams() const { return params; }
    const std::string& FunctionDecl::GetReturnTypeAnnotation() const { return returnTypeAnnotation; }
    bool FunctionDecl::HasReturnTypeAnnotation() const { return !returnTypeAnnotation.empty(); }
    ASTnode* FunctionDecl::GetBody() const { return body.get(); }

    std::string FunctionDecl::ToString() const {
        std::string result = "function " + name + "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) result += ", ";
            result += params[i].name;
            if (params[i].HasTypeAnnotation())
                result += " : " + params[i].typeAnnotation;
        }
        result += ")";
        if (!returnTypeAnnotation.empty())
            result += " : " + returnTypeAnnotation;
        result += " => " + body->ToString();
        return result;
    }

}
