#ifndef WHILE_H
#define WHILE_H

#include "../abs_nodes/ast.h"
#include <memory>

namespace Hulk {

    class WhileStmt : public ASTnode {
    private:
        std::unique_ptr<ASTnode> condition;
        std::unique_ptr<ASTnode> body;

    public:
        WhileStmt(std::unique_ptr<ASTnode> condition, std::unique_ptr<ASTnode> body);

        ASTnode* GetCondition() const;
        ASTnode* GetBody() const;

        std::string ToString() const override;
    };

}

#endif