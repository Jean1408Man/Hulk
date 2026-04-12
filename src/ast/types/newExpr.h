#ifndef NEW_EXPR_H
#define NEW_EXPR_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class NewExpr : public ASTnode {
    private:
        std::string typeName;
        std::vector<std::unique_ptr<ASTnode>> args;

    public:
        NewExpr(const std::string& typeName,
                std::vector<std::unique_ptr<ASTnode>> args);

        const std::string& GetTypeName() const;
        const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
