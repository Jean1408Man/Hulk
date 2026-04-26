#ifndef VECTOR_INDEX_H
#define VECTOR_INDEX_H

#include "../abs_nodes/expr.h"
#include <memory>

namespace Hulk {

    class VectorIndex : public Expr {
    private:
        std::unique_ptr<Expr> vector;
        std::unique_ptr<Expr> index;

    public:
        VectorIndex(std::unique_ptr<Expr> vector,
                    std::unique_ptr<Expr> index);

        Expr* GetVector() const;
        Expr* GetIndex() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
