#ifndef LET_IN_H
#define LET_IN_H

#include "../abs_nodes/expr.h"
#include "variableBinding.h"
#include <memory>
#include <vector>

namespace Hulk {

    class LetIn : public Expr {
    private:
        std::vector<std::unique_ptr<VariableBinding>> bindings;
        std::unique_ptr<Expr> body;

    public:
        LetIn(std::vector<std::unique_ptr<VariableBinding>> bindings,
              std::unique_ptr<Expr> body);

        const std::vector<std::unique_ptr<VariableBinding>>& GetBindings() const;
        Expr* GetBody() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
