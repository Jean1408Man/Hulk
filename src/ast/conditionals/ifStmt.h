#ifndef IFSTMT_H
#define IFSTMT_H

#include "../abs_nodes/ast.h"
#include "elifStmt.h"
#include <memory>
#include <vector>

namespace Hulk {

    class IfStmt : public ASTnode {
    private:
        std::unique_ptr<ASTnode> condition;
        std::unique_ptr<ASTnode> thenBranch;
        std::vector<ElifBranch> elifBranches;
        std::unique_ptr<ASTnode> elseBranch;

    public:
        IfStmt(std::unique_ptr<ASTnode> condition,
               std::unique_ptr<ASTnode> thenBranch,
               std::vector<ElifBranch> elifBranches,
               std::unique_ptr<ASTnode> elseBranch);

        ASTnode* GetCondition() const;
        ASTnode* GetThenBranch() const;
        const std::vector<ElifBranch>& GetElifBranches() const;
        ASTnode* GetElseBranch() const;

        std::string ToString() const override;
    };

}

#endif