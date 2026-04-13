#ifndef GROUP_H
#define GROUP_H

#include "../abs_nodes/expr.h"
#include <memory>

namespace Hulk {

    class Group : public Expr {
    private:
        std::unique_ptr<Expr> expr;

    public:
        explicit Group(std::unique_ptr<Expr> expression);

        virtual ~Group() = default;

        Expr* GetExpr() const;

        std::string ToString() const override;
    };

}

#endif