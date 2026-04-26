#ifndef PRINT_H
#define PRINT_H

#include "../abs_nodes/expr.h"
#include <memory>

namespace Hulk {

    class Print : public Expr {
    private:
        std::unique_ptr<Expr> expr;

    public:
        explicit Print(std::unique_ptr<Expr> expression);

        virtual ~Print() = default;

        Expr* GetExpr() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif