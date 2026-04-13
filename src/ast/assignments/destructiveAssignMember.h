#ifndef DESTRUCTIVE_ASSIGN_MEMBER_H
#define DESTRUCTIVE_ASSIGN_MEMBER_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class DestructiveAssignMember : public Expr {
    private:
        std::unique_ptr<Expr> object;
        std::string memberName;
        std::unique_ptr<Expr> value;

    public:
        DestructiveAssignMember(std::unique_ptr<Expr> object,
                                const std::string& memberName,
                                std::unique_ptr<Expr> value);

        Expr* GetObject() const;
        const std::string& GetMemberName() const;
        Expr* GetValue() const;

        std::string ToString() const override;
    };

}

#endif
