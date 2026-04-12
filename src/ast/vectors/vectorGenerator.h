#ifndef VECTOR_GENERATOR_H
#define VECTOR_GENERATOR_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class VectorGenerator : public ASTnode {
    private:
        std::unique_ptr<ASTnode> body;
        std::string varName;
        std::unique_ptr<ASTnode> iterable;

    public:
        VectorGenerator(std::unique_ptr<ASTnode> body,
                        const std::string& varName,
                        std::unique_ptr<ASTnode> iterable);

        ASTnode* GetBody() const;
        const std::string& GetVarName() const;
        ASTnode* GetIterable() const;

        std::string ToString() const override;
    };

}

#endif
