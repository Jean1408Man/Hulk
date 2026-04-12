#include "vectorGenerator.h"

namespace Hulk {

    VectorGenerator::VectorGenerator(std::unique_ptr<ASTnode> body,
                                     const std::string& varName,
                                     std::unique_ptr<ASTnode> iterable)
        : body(std::move(body)), varName(varName), iterable(std::move(iterable)) {}

    ASTnode* VectorGenerator::GetBody() const { return body.get(); }
    const std::string& VectorGenerator::GetVarName() const { return varName; }
    ASTnode* VectorGenerator::GetIterable() const { return iterable.get(); }

    std::string VectorGenerator::ToString() const {
        return "[" + body->ToString() + " | " + varName + " in " + iterable->ToString() + "]";
    }

}
