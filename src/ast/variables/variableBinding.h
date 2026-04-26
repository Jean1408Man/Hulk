#ifndef VARIABLE_BINDING_H
#define VARIABLE_BINDING_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class VariableBinding : public Expr {
    private:
        std::string name;
        std::string typeAnnotation;
        std::unique_ptr<Expr> initializer;

    public:
        VariableBinding(const std::string& name,
                        std::unique_ptr<Expr> initializer);

        VariableBinding(const std::string& name,
                        const std::string& typeAnnotation,
                        std::unique_ptr<Expr> initializer);

        const std::string& GetName() const;
        const std::string& GetTypeAnnotation() const;
        bool HasTypeAnnotation() const;
        Expr* GetInitializer() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
