#ifndef PRINT_H
#define PRINT_H

#include "../abs_nodes/ast.h"
#include <memory>

namespace Hulk {

    class Print : public ASTnode {
    private:
        std::unique_ptr<ASTnode> expr;

    public:
        explicit Print(std::unique_ptr<ASTnode> expression);

        virtual ~Print() = default;

        ASTnode* GetExpr() const;

        std::string ToString() const override;
    };

}

#endif