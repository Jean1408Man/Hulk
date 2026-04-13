#include "desctructiveAssign.h"

namespace Hulk {

    DestructiveAssign::DestructiveAssign(const std::string& varName, std::unique_ptr<Expr> val)
        : name(varName), value(std::move(val)) {}

    const std::string& DestructiveAssign::GetName() const {
        return name;
    }

    Expr* DestructiveAssign::GetValue() const {
        return value.get();
    }

    std::string DestructiveAssign::ToString() const {
        return name + " := " + value->ToString();
    }

}
