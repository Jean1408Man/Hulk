#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class FunctionCall : public Expr {
    private:
        std::string name;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        FunctionCall(const std::string& name,
                     std::vector<std::unique_ptr<Expr>> args);

        const std::string& GetName() const;
        const std::vector<std::unique_ptr<Expr>>& GetArgs() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
