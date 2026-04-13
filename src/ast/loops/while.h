#ifndef WHILE_H
#define WHILE_H

#include "../abs_nodes/expr.h"
#include <memory>

namespace Hulk {

    class WhileStmt : public Expr {
    private:
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Expr> body;

    public:
        WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> body);

        Expr* GetCondition() const;
        Expr* GetBody() const;

        std::string ToString() const override;
    };

}

#endif