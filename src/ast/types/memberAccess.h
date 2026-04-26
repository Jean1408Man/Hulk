#ifndef MEMBER_ACCESS_H
#define MEMBER_ACCESS_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class MemberAccess : public Expr {
    private:
        std::unique_ptr<Expr> object;
        std::string memberName;

    public:
        MemberAccess(std::unique_ptr<Expr> object,
                     const std::string& memberName);

        Expr* GetObject() const;
        const std::string& GetMemberName() const;
        std::unique_ptr<Expr> TakeObject();
        std::string TakeMemberName();

        std::string ToString() const override;
        void accept(ExprVisitor& v) override;
    };

}

#endif
