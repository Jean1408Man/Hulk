#ifndef EXPR_BLOCK_H
#define EXPR_BLOCK_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <vector>

namespace Hulk {

    class ExprBlock : public Expr {
    private:
        std::vector<std::unique_ptr<Expr>> exprs;

    public:
        explicit ExprBlock(std::vector<std::unique_ptr<Expr>> exprs);

        const std::vector<std::unique_ptr<Expr>>& GetExprs() const;
        Expr* GetLast() const;

        std::string ToString() const override;
    };

}

#endif
