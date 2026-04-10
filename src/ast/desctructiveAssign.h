// src/ast/destructiveAssign.h
#ifndef DESTRUCTIVE_ASSIGN_H
#define DESTRUCTIVE_ASSIGN_H

#include "./abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class DestructiveAssign : public ASTnode {
    private:
        std::string name;
        std::unique_ptr<ASTnode> value;

    public:
        DestructiveAssign(const std::string& varName, std::unique_ptr<ASTnode> val);

        const std::string& GetName() const;
        ASTnode* GetValue() const;

        std::string ToString() const override;
    };

}
#endif