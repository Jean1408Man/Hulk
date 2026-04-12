#ifndef SELF_REF_H
#define SELF_REF_H

#include "../abs_nodes/ast.h"

namespace Hulk {

    class SelfRef : public ASTnode {
    public:
        SelfRef() = default;

        std::string ToString() const override;
    };

}

#endif
