#ifndef LET_IN_H
#define LET_IN_H

#include "../abs_nodes/ast.h"
#include "variableBinding.h"
#include <memory>
#include <vector>

namespace Hulk {

    class LetIn : public ASTnode {
    private:
        std::vector<std::unique_ptr<VariableBinding>> bindings;
        std::unique_ptr<ASTnode> body;

    public:
        LetIn(std::vector<std::unique_ptr<VariableBinding>> bindings,
              std::unique_ptr<ASTnode> body);

        const std::vector<std::unique_ptr<VariableBinding>>& GetBindings() const;
        ASTnode* GetBody() const;

        std::string ToString() const override;
    };

}

#endif
