#ifndef AS_EXPR_H
#define AS_EXPR_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class AsExpr : public Expr {
    private:
        std::unique_ptr<Expr> expr;
        std::string typeName;

    public:
        AsExpr(std::unique_ptr<Expr> expr, const std::string& typeName);

        Expr* GetExpr() const;
        const std::string& GetTypeName() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
