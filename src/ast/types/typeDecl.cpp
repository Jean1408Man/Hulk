#include "typeDecl.h"

namespace Hulk {

    TypeDecl::TypeDecl(const std::string& name,
                       std::vector<TypeMember> members)
        : name(name), ctorParams({}), parentName(""), parentArgs({}), members(std::move(members)) {}

    TypeDecl::TypeDecl(const std::string& name,
                       std::vector<Param> ctorParams,
                       std::vector<TypeMember> members)
        : name(name), ctorParams(std::move(ctorParams)), parentName(""), parentArgs({}), members(std::move(members)) {}

    TypeDecl::TypeDecl(const std::string& name,
                       const std::string& parentName,
                       std::vector<TypeMember> members)
        : name(name), ctorParams({}), parentName(parentName), parentArgs({}), members(std::move(members)) {}

    TypeDecl::TypeDecl(const std::string& name,
                       std::vector<Param> ctorParams,
                       const std::string& parentName,
                       std::vector<std::unique_ptr<ASTnode>> parentArgs,
                       std::vector<TypeMember> members)
        : name(name), ctorParams(std::move(ctorParams)), parentName(parentName),
          parentArgs(std::move(parentArgs)), members(std::move(members)) {}

    const std::string& TypeDecl::GetName() const { return name; }
    const std::vector<Param>& TypeDecl::GetCtorParams() const { return ctorParams; }
    bool TypeDecl::HasCtorParams() const { return !ctorParams.empty(); }
    const std::string& TypeDecl::GetParentName() const { return parentName; }
    bool TypeDecl::HasParent() const { return !parentName.empty(); }
    const std::vector<std::unique_ptr<ASTnode>>& TypeDecl::GetParentArgs() const { return parentArgs; }
    const std::vector<TypeMember>& TypeDecl::GetMembers() const { return members; }

    std::string TypeDecl::ToString() const {
        std::string result = "type " + name;
        if (!ctorParams.empty()) {
            result += "(";
            for (size_t i = 0; i < ctorParams.size(); ++i) {
                if (i > 0) result += ", ";
                result += ctorParams[i].name;
                if (ctorParams[i].HasTypeAnnotation())
                    result += " : " + ctorParams[i].typeAnnotation;
            }
            result += ")";
        }
        if (!parentName.empty()) {
            result += " inherits " + parentName;
            if (!parentArgs.empty()) {
                result += "(";
                for (size_t i = 0; i < parentArgs.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += parentArgs[i]->ToString();
                }
                result += ")";
            }
        }
        result += " { ... }";
        return result;
    }

}
