#ifndef VARIABLE_REFERENCE_H
#define VARIABLE_REFERENCE_H

#include "./abs_nodes/ast.h"
#include <string>

namespace Hulk {

    class VariableReference : public ASTnode {
    private:
        std::string name;

    public:
        explicit VariableReference(const std::string& varName);

        virtual ~VariableReference() = default;

        std::string GetName() const;

        std::string ToString() const override;
    };

}

#endif