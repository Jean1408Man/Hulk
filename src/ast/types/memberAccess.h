#ifndef MEMBER_ACCESS_H
#define MEMBER_ACCESS_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class MemberAccess : public ASTnode {
    private:
        std::unique_ptr<ASTnode> object;
        std::string memberName;

    public:
        MemberAccess(std::unique_ptr<ASTnode> object,
                     const std::string& memberName);

        ASTnode* GetObject() const;
        const std::string& GetMemberName() const;

        std::string ToString() const override;
    };

}

#endif
