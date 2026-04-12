#ifndef VECTOR_INDEX_H
#define VECTOR_INDEX_H

#include "../abs_nodes/ast.h"
#include <memory>

namespace Hulk {

    class VectorIndex : public ASTnode {
    private:
        std::unique_ptr<ASTnode> vector;
        std::unique_ptr<ASTnode> index;

    public:
        VectorIndex(std::unique_ptr<ASTnode> vector,
                    std::unique_ptr<ASTnode> index);

        ASTnode* GetVector() const;
        ASTnode* GetIndex() const;

        std::string ToString() const override;
    };

}

#endif
