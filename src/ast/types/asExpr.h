#ifndef AS_EXPR_H
#define AS_EXPR_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class AsExpr : public ASTnode {
    private:
        std::unique_ptr<ASTnode> expr;
        std::string typeName;

    public:
        AsExpr(std::unique_ptr<ASTnode> expr, const std::string& typeName);

        ASTnode* GetExpr() const;
        const std::string& GetTypeName() const;

        std::string ToString() const override;
    };

}

#endif
