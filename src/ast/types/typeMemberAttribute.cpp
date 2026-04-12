#include "typeMemberAttribute.h"

namespace Hulk {

    TypeMemberAttribute::TypeMemberAttribute(const std::string& name,
                                             std::unique_ptr<ASTnode> initializer)
        : name(name), typeAnnotation(""), initializer(std::move(initializer)) {}

    TypeMemberAttribute::TypeMemberAttribute(const std::string& name,
                                             const std::string& typeAnnotation,
                                             std::unique_ptr<ASTnode> initializer)
        : name(name), typeAnnotation(typeAnnotation), initializer(std::move(initializer)) {}

    const std::string& TypeMemberAttribute::GetName() const { return name; }
    const std::string& TypeMemberAttribute::GetTypeAnnotation() const { return typeAnnotation; }
    bool TypeMemberAttribute::HasTypeAnnotation() const { return !typeAnnotation.empty(); }
    ASTnode* TypeMemberAttribute::GetInitializer() const { return initializer.get(); }

    std::string TypeMemberAttribute::ToString() const {
        std::string result = name;
        if (!typeAnnotation.empty())
            result += " : " + typeAnnotation;
        result += " = " + initializer->ToString();
        return result;
    }

}
