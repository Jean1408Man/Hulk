#include "variableBinding.h"

namespace Hulk {

    VariableBinding::VariableBinding(const std::string& name,
                                     std::unique_ptr<Expr> initializer)
        : name(name), typeAnnotation(""), initializer(std::move(initializer)) {}

    VariableBinding::VariableBinding(const std::string& name,
                                     const std::string& typeAnnotation,
                                     std::unique_ptr<Expr> initializer)
        : name(name), typeAnnotation(typeAnnotation), initializer(std::move(initializer)) {}

    const std::string& VariableBinding::GetName() const { return name; }
    const std::string& VariableBinding::GetTypeAnnotation() const { return typeAnnotation; }
    bool VariableBinding::HasTypeAnnotation() const { return !typeAnnotation.empty(); }
    Expr* VariableBinding::GetInitializer() const { return initializer.get(); }

    std::string VariableBinding::ToString() const {
        std::string result = name;
        if (!typeAnnotation.empty())
            result += " : " + typeAnnotation;
        result += " = " + initializer->ToString();
        return result;
    }

}
