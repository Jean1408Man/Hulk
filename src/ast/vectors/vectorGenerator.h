#ifndef VECTOR_GENERATOR_H
#define VECTOR_GENERATOR_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class VectorGenerator : public Expr {
    private:
        std::unique_ptr<Expr> body;
        std::string varName;
        std::unique_ptr<Expr> iterable;

    public:
        VectorGenerator(std::unique_ptr<Expr> body,
                        const std::string& varName,
                        std::unique_ptr<Expr> iterable);

        Expr* GetBody() const;
        const std::string& GetVarName() const;
        Expr* GetIterable() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
