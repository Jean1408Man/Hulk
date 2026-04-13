#ifndef FUNCTION_DECL_H
#define FUNCTION_DECL_H

#include "../abs_nodes/decl.h"
#include "../abs_nodes/expr.h"
#include "param.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class FunctionDecl : public Decl {
    private:
        std::string name;
        std::vector<Param> params;
        std::string returnTypeAnnotation;
        std::unique_ptr<Expr> body;

    public:
        FunctionDecl(const std::string& name,
                     std::vector<Param> params,
                     std::unique_ptr<Expr> body);

        FunctionDecl(const std::string& name,
                     std::vector<Param> params,
                     const std::string& returnTypeAnnotation,
                     std::unique_ptr<Expr> body);

        const std::string& GetName() const;
        const std::vector<Param>& GetParams() const;
        const std::string& GetReturnTypeAnnotation() const;
        bool HasReturnTypeAnnotation() const;
        Expr* GetBody() const;

        std::string ToString() const override;
    };

}

#endif
