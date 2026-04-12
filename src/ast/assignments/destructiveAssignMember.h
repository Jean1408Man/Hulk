#ifndef DESTRUCTIVE_ASSIGN_MEMBER_H
#define DESTRUCTIVE_ASSIGN_MEMBER_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class DestructiveAssignMember : public ASTnode {
    private:
        std::unique_ptr<ASTnode> object;
        std::string memberName;
        std::unique_ptr<ASTnode> value;

    public:
        DestructiveAssignMember(std::unique_ptr<ASTnode> object,
                                const std::string& memberName,
                                std::unique_ptr<ASTnode> value);

        ASTnode* GetObject() const;
        const std::string& GetMemberName() const;
        ASTnode* GetValue() const;

        std::string ToString() const override;
    };

}

#endif
