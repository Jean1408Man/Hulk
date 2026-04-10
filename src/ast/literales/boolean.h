#ifndef BOOLEAN_NODE_H
#define BOOLEAN_NODE_H

#include "../abs_nodes/ast.h"

namespace Hulk {

    class Boolean : public ASTnode {
    private:
        bool value;

    public:
        explicit Boolean(bool val);
        bool GetValue() const;
        std::string ToString() const override;
    };

}

#endif
