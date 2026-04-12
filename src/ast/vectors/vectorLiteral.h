#ifndef VECTOR_LITERAL_H
#define VECTOR_LITERAL_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <vector>

namespace Hulk {

    class VectorLiteral : public ASTnode {
    private:
        std::vector<std::unique_ptr<ASTnode>> elements;

    public:
        explicit VectorLiteral(std::vector<std::unique_ptr<ASTnode>> elements);

        const std::vector<std::unique_ptr<ASTnode>>& GetElements() const;

        std::string ToString() const override;
    };

}

#endif
