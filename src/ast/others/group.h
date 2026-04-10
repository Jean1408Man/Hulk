#ifndef GROUP_H
#define GROUP_H

#include "../abs_nodes/ast.h"
#include <memory>

namespace Hulk {

    class Group : public ASTnode {
    private:
        std::unique_ptr<ASTnode> expr;

    public:
        explicit Group(std::unique_ptr<ASTnode> expression);

        virtual ~Group() = default;

        ASTnode* GetExpr() const;

        std::string ToString() const override;
    };

}

#endif