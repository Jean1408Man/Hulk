#ifndef EXPR_BLOCK_H
#define EXPR_BLOCK_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <vector>

namespace Hulk {

    class ExprBlock : public ASTnode {
    private:
        std::vector<std::unique_ptr<ASTnode>> exprs;

    public:
        explicit ExprBlock(std::vector<std::unique_ptr<ASTnode>> exprs);

        const std::vector<std::unique_ptr<ASTnode>>& GetExprs() const;
        ASTnode* GetLast() const;

        std::string ToString() const override;
    };

}

#endif
