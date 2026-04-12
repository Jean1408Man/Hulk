#include "destructiveAssignMember.h"

namespace Hulk {

    DestructiveAssignMember::DestructiveAssignMember(std::unique_ptr<ASTnode> object,
                                                     const std::string& memberName,
                                                     std::unique_ptr<ASTnode> value)
        : object(std::move(object)), memberName(memberName), value(std::move(value)) {}

    ASTnode* DestructiveAssignMember::GetObject() const { return object.get(); }
    const std::string& DestructiveAssignMember::GetMemberName() const { return memberName; }
    ASTnode* DestructiveAssignMember::GetValue() const { return value.get(); }

    std::string DestructiveAssignMember::ToString() const {
        return object->ToString() + "." + memberName + " := " + value->ToString();
    }

}
