#ifndef IS_EXPR_H
#define IS_EXPR_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class IsExpr : public Expr {
    private:
        std::unique_ptr<Expr> expr;
        std::string typeName;

    public:
        IsExpr(std::unique_ptr<Expr> expr, const std::string& typeName);

        Expr* GetExpr() const;
        const std::string& GetTypeName() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
