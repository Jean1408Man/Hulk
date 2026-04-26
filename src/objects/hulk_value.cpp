#include "hulk_value.h"
#include "hulk_object.h"
#include <string>

namespace Hulk {

std::string HulkValue::to_string() const {
    struct Visitor {
        std::string operator()(Nil) const { return "nil"; }
        std::string operator()(double v) const {
            if (v == (long long)v) return std::to_string((long long)v);
            return std::to_string(v);
        }
        std::string operator()(const std::string& v) const { return v; }
        std::string operator()(bool v) const { return v ? "true" : "false"; }
        std::string operator()(const std::shared_ptr<HulkObject>& o) const {
            return "<" + o->type_name + " object>";
        }
        std::string operator()(const std::shared_ptr<HulkVector>& vec) const {
            std::string s = "[";
            for (size_t i = 0; i < vec->size(); ++i) {
                if (i) s += ", ";
                s += (*vec)[i].to_string();
            }
            return s + "]";
        }
    };
    return std::visit(Visitor{}, inner);
}

} // namespace Hulk
