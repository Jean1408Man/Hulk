#ifndef PROGRAM_H
#define PROGRAM_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <vector>

namespace Hulk {

    class Program : public ASTnode {
    private:
        std::vector<std::unique_ptr<ASTnode>> declarations;
        std::unique_ptr<ASTnode> globalExpr;

    public:
        Program(std::vector<std::unique_ptr<ASTnode>> declarations,
                std::unique_ptr<ASTnode> globalExpr);

        const std::vector<std::unique_ptr<ASTnode>>& GetDeclarations() const;
        ASTnode* GetGlobalExpr() const;

        std::string ToString() const override;
    };

}

#endif
