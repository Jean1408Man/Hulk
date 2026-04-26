#ifndef SELF_REF_H
#define SELF_REF_H

#include "../abs_nodes/expr.h"

namespace Hulk {

    class SelfRef : public Expr {
    public:
        SelfRef() = default;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
