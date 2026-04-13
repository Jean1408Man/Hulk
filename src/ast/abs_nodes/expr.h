#ifndef EXPR_H
#define EXPR_H

#include "ast.h"

namespace Hulk {

    class Expr : public ASTnode {
    public:
        virtual ~Expr() = default;
    };

}

#endif
