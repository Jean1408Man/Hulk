#ifndef IFSTMT_H
#define IFSTMT_H

#include "../abs_nodes/expr.h"
#include "elifStmt.h"
#include <memory>
#include <vector>

namespace Hulk {

    class IfStmt : public Expr {
    private:
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Expr> thenBranch;
        std::vector<ElifBranch> elifBranches;
        std::unique_ptr<Expr> elseBranch;

    public:
        IfStmt(std::unique_ptr<Expr> condition,
               std::unique_ptr<Expr> thenBranch,
               std::vector<ElifBranch> elifBranches,
               std::unique_ptr<Expr> elseBranch);

        Expr* GetCondition() const;
        Expr* GetThenBranch() const;
        const std::vector<ElifBranch>& GetElifBranches() const;
        Expr* GetElseBranch() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif