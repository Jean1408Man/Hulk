#ifndef VARIABLE_BINDING_H
#define VARIABLE_BINDING_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class VariableBinding : public ASTnode {
    private:
        std::string name;
        std::string typeAnnotation;
        std::unique_ptr<ASTnode> initializer;

    public:
        VariableBinding(const std::string& name,
                        std::unique_ptr<ASTnode> initializer);

        VariableBinding(const std::string& name,
                        const std::string& typeAnnotation,
                        std::unique_ptr<ASTnode> initializer);

        const std::string& GetName() const;
        const std::string& GetTypeAnnotation() const;
        bool HasTypeAnnotation() const;
        ASTnode* GetInitializer() const;

        std::string ToString() const override;
    };

}

#endif
