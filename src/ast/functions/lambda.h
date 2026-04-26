#ifndef LAMBDA_H
#define LAMBDA_H

#include "../abs_nodes/expr.h"
#include "param.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class Lambda : public Expr {
    private:
        std::vector<Param> params;
        std::string returnTypeAnnotation;
        std::unique_ptr<Expr> body;

    public:
        Lambda(std::vector<Param> params,
               std::unique_ptr<Expr> body);

        Lambda(std::vector<Param> params,
               const std::string& returnTypeAnnotation,
               std::unique_ptr<Expr> body);

        const std::vector<Param>& GetParams() const;
        const std::string& GetReturnTypeAnnotation() const;
        bool HasReturnTypeAnnotation() const;
        Expr* GetBody() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
