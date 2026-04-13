#ifndef PROGRAM_H
#define PROGRAM_H

#include "../abs_nodes/ast.h"
#include "../abs_nodes/decl.h"
#include "../abs_nodes/expr.h"
#include <memory>
#include <vector>

namespace Hulk {

    class Program : public ASTnode {
    private:
        std::vector<std::unique_ptr<Decl>> declarations;
        std::unique_ptr<Expr> globalExpr;

    public:
        Program(std::vector<std::unique_ptr<Decl>> declarations,
                std::unique_ptr<Expr> globalExpr);

        const std::vector<std::unique_ptr<Decl>>& GetDeclarations() const;
        Expr* GetGlobalExpr() const;

        std::string ToString() const override;
    };

}

#endif
