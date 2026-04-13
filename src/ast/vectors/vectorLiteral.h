#ifndef VECTOR_LITERAL_H
#define VECTOR_LITERAL_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <vector>

namespace Hulk {

    class VectorLiteral : public Expr {
    private:
        std::vector<std::unique_ptr<Expr>> elements;

    public:
        explicit VectorLiteral(std::vector<std::unique_ptr<Expr>> elements);

        const std::vector<std::unique_ptr<Expr>>& GetElements() const;

        std::string ToString() const override;
    };

}

#endif
