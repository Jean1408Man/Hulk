#ifndef EXPR_H
#define EXPR_H

#include "ast.h"
#include "../../common/span.hpp"

namespace Hulk {

    class ExprVisitor;

    class Expr : public ASTnode {
    public:
        hulk::common::Span span {};
        virtual ~Expr() = default;
        virtual void accept(ExprVisitor& v) = 0;
    };

}

#endif
