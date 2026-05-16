#ifndef HULK_BACKEND_RUNTIME_HPP
#define HULK_BACKEND_RUNTIME_HPP

#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace hulk_rt {

struct Nil {};
struct Object;
using ObjectPtr = std::shared_ptr<Object>;

class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

[[noreturn]] inline void fail(const std::string& msg) {
    throw RuntimeError(msg);
}

struct Value {
    using Inner = std::variant<Nil, double, std::string, bool, ObjectPtr>;
    Inner inner;

    Value() : inner(Nil{}) {}
    explicit Value(double v) : inner(v) {}
    explicit Value(const std::string& v) : inner(v) {}
    explicit Value(std::string&& v) : inner(std::move(v)) {}
    explicit Value(const char* v) : inner(std::string(v)) {}
    explicit Value(bool v) : inner(v) {}
    explicit Value(ObjectPtr v) : inner(std::move(v)) {}
};

struct Object {
    std::string type_name;
    std::unordered_map<std::string, Value> fields;

    explicit Object(std::string type) : type_name(std::move(type)) {}
};

using MethodFn = std::function<Value(ObjectPtr, const std::vector<Value>&)>;

struct Method {
    MethodFn fn;
    std::size_t arity = 0;
};

struct TypeInfo {
    std::string parent;
    std::unordered_map<std::string, Method> methods;
};

inline std::unordered_map<std::string, TypeInfo>& type_registry() {
    static std::unordered_map<std::string, TypeInfo> registry;
    return registry;
}

inline void register_type(const std::string& name, const std::string& parent) {
    type_registry()[name].parent = parent;
}

inline void register_method(const std::string& type_name,
                            const std::string& method_name,
                            MethodFn fn,
                            std::size_t arity) {
    type_registry()[type_name].methods[method_name] = Method{std::move(fn), arity};
}

inline bool is_nil(const Value& v) { return std::holds_alternative<Nil>(v.inner); }
inline bool is_number(const Value& v) { return std::holds_alternative<double>(v.inner); }
inline bool is_string(const Value& v) { return std::holds_alternative<std::string>(v.inner); }
inline bool is_bool(const Value& v) { return std::holds_alternative<bool>(v.inner); }
inline bool is_object(const Value& v) { return std::holds_alternative<ObjectPtr>(v.inner); }

inline double as_number(const Value& v) {
    if (!is_number(v)) fail("Runtime error: se esperaba Number.");
    return std::get<double>(v.inner);
}

inline const std::string& as_string_ref(const Value& v) {
    if (!is_string(v)) fail("Runtime error: se esperaba String.");
    return std::get<std::string>(v.inner);
}

inline bool as_bool(const Value& v) {
    if (!is_bool(v)) fail("Runtime error: se esperaba Boolean.");
    return std::get<bool>(v.inner);
}

inline ObjectPtr as_object(const Value& v) {
    if (!is_object(v)) fail("Runtime error: se esperaba Object.");
    return std::get<ObjectPtr>(v.inner);
}

inline std::string to_string(const Value& v) {
    struct Visitor {
        std::string operator()(Nil) const { return "nil"; }
        std::string operator()(double n) const {
            if (n == static_cast<long long>(n)) {
                return std::to_string(static_cast<long long>(n));
            }
            return std::to_string(n);
        }
        std::string operator()(const std::string& s) const { return s; }
        std::string operator()(bool b) const { return b ? "true" : "false"; }
        std::string operator()(const ObjectPtr& obj) const {
            return "<" + obj->type_name + " object>";
        }
    };
    return std::visit(Visitor{}, v.inner);
}

inline bool truthy(const Value& v) {
    if (is_nil(v)) return false;
    if (is_bool(v)) return std::get<bool>(v.inner);
    return true;
}

inline Value hulk_print(const Value& v) {
    std::cout << to_string(v) << "\n";
    return v;
}

inline Value add(const Value& a, const Value& b) { return Value(as_number(a) + as_number(b)); }
inline Value sub(const Value& a, const Value& b) { return Value(as_number(a) - as_number(b)); }
inline Value mul(const Value& a, const Value& b) { return Value(as_number(a) * as_number(b)); }

inline Value div(const Value& a, const Value& b) {
    const double rhs = as_number(b);
    if (rhs == 0) fail("Runtime error: division por cero.");
    return Value(as_number(a) / rhs);
}

inline Value mod(const Value& a, const Value& b) {
    const double rhs = as_number(b);
    if (rhs == 0) fail("Runtime error: modulo por cero.");
    return Value(std::fmod(as_number(a), rhs));
}

inline Value pow(const Value& a, const Value& b) {
    return Value(std::pow(as_number(a), as_number(b)));
}

inline Value neg(const Value& v) { return Value(-as_number(v)); }
inline Value logical_not(const Value& v) { return Value(!truthy(v)); }

inline Value greater(const Value& a, const Value& b) { return Value(as_number(a) > as_number(b)); }
inline Value less(const Value& a, const Value& b) { return Value(as_number(a) < as_number(b)); }
inline Value greater_equal(const Value& a, const Value& b) { return Value(as_number(a) >= as_number(b)); }
inline Value less_equal(const Value& a, const Value& b) { return Value(as_number(a) <= as_number(b)); }

inline Value equal(const Value& a, const Value& b) {
    if (is_number(a) && is_number(b)) return Value(std::get<double>(a.inner) == std::get<double>(b.inner));
    if (is_string(a) && is_string(b)) return Value(std::get<std::string>(a.inner) == std::get<std::string>(b.inner));
    if (is_bool(a) && is_bool(b)) return Value(std::get<bool>(a.inner) == std::get<bool>(b.inner));
    if (is_nil(a) && is_nil(b)) return Value(true);
    if (is_object(a) && is_object(b)) return Value(std::get<ObjectPtr>(a.inner) == std::get<ObjectPtr>(b.inner));
    return Value(false);
}

inline Value not_equal(const Value& a, const Value& b) {
    return Value(!std::get<bool>(equal(a, b).inner));
}

inline Value concat(const Value& a, const Value& b, bool with_space) {
    return Value(to_string(a) + (with_space ? " " : "") + to_string(b));
}

inline Value hulk_sqrt(const Value& v) { return Value(std::sqrt(as_number(v))); }
inline Value hulk_sin(const Value& v) { return Value(std::sin(as_number(v))); }
inline Value hulk_cos(const Value& v) { return Value(std::cos(as_number(v))); }
inline Value hulk_exp(const Value& v) { return Value(std::exp(as_number(v))); }
inline Value hulk_log(const Value& base, const Value& value) {
    return Value(std::log(as_number(value)) / std::log(as_number(base)));
}
inline Value hulk_rand() { return Value(static_cast<double>(std::rand()) / RAND_MAX); }

inline Value get_field(const Value& receiver, const std::string& field) {
    auto obj = as_object(receiver);
    auto it = obj->fields.find(field);
    if (it == obj->fields.end()) {
        fail("Runtime error: atributo '" + field + "' no existe en tipo '" + obj->type_name + "'.");
    }
    return it->second;
}

inline Value set_field(const Value& receiver, const std::string& field, const Value& value) {
    auto obj = as_object(receiver);
    auto it = obj->fields.find(field);
    if (it == obj->fields.end()) {
        fail("Runtime error: atributo '" + field + "' no existe en tipo '" + obj->type_name + "'.");
    }
    it->second = value;
    return value;
}

inline bool is_instance(const Value& value, const std::string& type_name) {
    if (!is_object(value)) {
        if (type_name == "Number") return is_number(value);
        if (type_name == "String") return is_string(value);
        if (type_name == "Boolean") return is_bool(value);
        return false;
    }

    std::string current = as_object(value)->type_name;
    int depth = 0;
    while (!current.empty() && depth++ < 256) {
        if (current == type_name) return true;
        auto it = type_registry().find(current);
        if (it == type_registry().end()) break;
        current = it->second.parent;
    }
    return type_name == "Object";
}

inline Value checked_cast(const Value& value, const std::string& type_name) {
    if (!is_instance(value, type_name)) {
        fail("Runtime error: no se puede castear '" + to_string(value) + "' a '" + type_name + "'.");
    }
    return value;
}

inline const Method* find_method(const std::string& start_type, const std::string& method_name) {
    std::string current = start_type;
    int depth = 0;
    while (!current.empty() && depth++ < 256) {
        auto type_it = type_registry().find(current);
        if (type_it == type_registry().end()) return nullptr;
        auto method_it = type_it->second.methods.find(method_name);
        if (method_it != type_it->second.methods.end()) return &method_it->second;
        current = type_it->second.parent;
    }
    return nullptr;
}

inline Value call_method_from(const Value& receiver,
                              const std::string& start_type,
                              const std::string& method_name,
                              const std::vector<Value>& args) {
    auto obj = as_object(receiver);
    const Method* method = find_method(start_type, method_name);
    if (!method) {
        fail("Runtime error: metodo '" + method_name + "' no existe en tipo '" + start_type + "'.");
    }
    if (method->arity != args.size()) {
        fail("Runtime error: metodo '" + method_name + "' recibio aridad incorrecta.");
    }
    return method->fn(obj, args);
}

inline Value call_method(const Value& receiver,
                         const std::string& method_name,
                         const std::vector<Value>& args) {
    auto obj = as_object(receiver);
    return call_method_from(receiver, obj->type_name, method_name, args);
}

} // namespace hulk_rt

#endif
