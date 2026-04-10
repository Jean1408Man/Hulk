#include "variableReference.h"

namespace Hulk {

    VariableReference::VariableReference(const std::string& varName) 
        : name(varName) {}

    std::string VariableReference::GetName() const {
        return name;
    }

    std::string VariableReference::ToString() const {
        return "Variable(" + name + ")";
    }

}