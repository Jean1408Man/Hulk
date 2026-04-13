#include "destructiveAssignMember.h"

namespace Hulk {

    DestructiveAssignMember::DestructiveAssignMember(std::unique_ptr<Expr> object,
                                                     const std::string& memberName,
                                                     std::unique_ptr<Expr> value)
        : object(std::move(object)), memberName(memberName), value(std::move(value)) {}

    Expr* DestructiveAssignMember::GetObject() const { return object.get(); }
    const std::string& DestructiveAssignMember::GetMemberName() const { return memberName; }
    Expr* DestructiveAssignMember::GetValue() const { return value.get(); }

    std::string DestructiveAssignMember::ToString() const {
        return object->ToString() + "." + memberName + " := " + value->ToString();
    }

}
