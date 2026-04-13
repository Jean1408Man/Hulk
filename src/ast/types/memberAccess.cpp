#include "memberAccess.h"

namespace Hulk {

    MemberAccess::MemberAccess(std::unique_ptr<Expr> object,
                               const std::string& memberName)
        : object(std::move(object)), memberName(memberName) {}

    Expr* MemberAccess::GetObject() const { return object.get(); }
    const std::string& MemberAccess::GetMemberName() const { return memberName; }

    std::string MemberAccess::ToString() const {
        return object->ToString() + "." + memberName;
    }

}
