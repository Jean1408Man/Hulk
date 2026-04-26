// src/ast/assignments/destructiveAssign.h
#ifndef DESTRUCTIVE_ASSIGN_H
#define DESTRUCTIVE_ASSIGN_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class DestructiveAssign : public Expr {
    private:
        std::string name;
        std::unique_ptr<Expr> value;

    public:
        DestructiveAssign(const std::string& varName, std::unique_ptr<Expr> val);

        const std::string& GetName() const;
        Expr* GetValue() const;

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}
#endif