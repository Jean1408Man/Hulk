#ifndef NEW_EXPR_H
#define NEW_EXPR_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class NewExpr : public Expr {
    private:
        std::string typeName;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        NewExpr(const std::string& typeName,
                std::vector<std::unique_ptr<Expr>> args);

        const std::string& GetTypeName() const;
        const std::vector<std::unique_ptr<Expr>>& GetArgs() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
