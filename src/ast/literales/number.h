#ifndef NUMBER_NODE_H
#define NUMBER_NODE_H

#include "../abs_nodes/expr.h"

namespace Hulk {

    class Number : public Expr {
    private:
        double value;

    public:
        explicit Number(double val);
        double GetValue() const;
        std::string ToString() const override;
    };

}

#endif
