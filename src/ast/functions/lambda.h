#ifndef LAMBDA_H
#define LAMBDA_H

#include "../abs_nodes/ast.h"
#include "param.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class Lambda : public ASTnode {
    private:
        std::vector<Param> params;
        std::string returnTypeAnnotation;
        std::unique_ptr<ASTnode> body;

    public:
        Lambda(std::vector<Param> params,
               std::unique_ptr<ASTnode> body);

        Lambda(std::vector<Param> params,
               const std::string& returnTypeAnnotation,
               std::unique_ptr<ASTnode> body);

        const std::vector<Param>& GetParams() const;
        const std::string& GetReturnTypeAnnotation() const;
        bool HasReturnTypeAnnotation() const;
        ASTnode* GetBody() const;

        std::string ToString() const override;
    };

}

#endif
