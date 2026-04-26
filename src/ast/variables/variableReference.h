#ifndef VARIABLE_REFERENCE_H
#define VARIABLE_REFERENCE_H

#include "../abs_nodes/expr.h"
#include <string>

namespace Hulk {

    class VariableReference : public Expr {
    private:
        std::string name;

    public:
        explicit VariableReference(const std::string& varName);

        virtual ~VariableReference() = default;

        std::string GetName() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif