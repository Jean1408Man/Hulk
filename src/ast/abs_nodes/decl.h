#ifndef DECL_H
#define DECL_H

#include "ast.h"

namespace Hulk {

    class Decl : public ASTnode {
    public:
        virtual ~Decl() = default;
    };

}

#endif
