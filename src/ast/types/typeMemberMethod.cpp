#include "typeMemberMethod.h"

namespace Hulk {

    TypeMemberMethod::TypeMemberMethod(const std::string& name,
                                       std::vector<Param> params,
                                       std::unique_ptr<ASTnode> body)
        : name(name), params(std::move(params)), returnTypeAnnotation(""), body(std::move(body)) {}

    TypeMemberMethod::TypeMemberMethod(const std::string& name,
                                       std::vector<Param> params,
                                       const std::string& returnTypeAnnotation,
                                       std::unique_ptr<ASTnode> body)
        : name(name), params(std::move(params)), returnTypeAnnotation(returnTypeAnnotation), body(std::move(body)) {}

    const std::string& TypeMemberMethod::GetName() const { return name; }
    const std::vector<Param>& TypeMemberMethod::GetParams() const { return params; }
    const std::string& TypeMemberMethod::GetReturnTypeAnnotation() const { return returnTypeAnnotation; }
    bool TypeMemberMethod::HasReturnTypeAnnotation() const { return !returnTypeAnnotation.empty(); }
    ASTnode* TypeMemberMethod::GetBody() const { return body.get(); }

    std::string TypeMemberMethod::ToString() const {
        std::string result = name + "(";
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
