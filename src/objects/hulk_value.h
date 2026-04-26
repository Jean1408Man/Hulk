#ifndef HULK_VALUE_H
#define HULK_VALUE_H

#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <stdexcept>

namespace Hulk {

    struct HulkObject;

    // Nil representa la ausencia de valor (while sin iteraciones, void, etc.)
    struct Nil {};

    // Vector de valores HULK
    using HulkVector = std::vector<class HulkValue>;

    // -----------------------------------------------------------------------
    // HulkValue — unión tipada de todos los valores posibles en runtime
    // -----------------------------------------------------------------------
    class HulkValue {
    public:
        using Inner = std::variant<
            Nil,
            double,
            std::string,
            bool,
            std::shared_ptr<HulkObject>,
            std::shared_ptr<HulkVector>
        >;

        Inner inner;

        HulkValue()                              : inner(Nil{}) {}
        explicit HulkValue(double v)             : inner(v) {}
        explicit HulkValue(const std::string& v) : inner(v) {}
        explicit HulkValue(std::string&& v)      : inner(std::move(v)) {}
        explicit HulkValue(bool v)               : inner(v) {}
        explicit HulkValue(std::shared_ptr<HulkObject> v) : inner(std::move(v)) {}
        explicit HulkValue(std::shared_ptr<HulkVector> v) : inner(std::move(v)) {}

        bool is_nil()    const { return std::holds_alternative<Nil>(inner); }
        bool is_number() const { return std::holds_alternative<double>(inner); }
        bool is_string() const { return std::holds_alternative<std::string>(inner); }
        bool is_bool()   const { return std::holds_alternative<bool>(inner); }
        bool is_object() const { return std::holds_alternative<std::shared_ptr<HulkObject>>(inner); }
        bool is_vector() const { return std::holds_alternative<std::shared_ptr<HulkVector>>(inner); }

        double      as_number() const { return std::get<double>(inner); }
        const std::string& as_string() const { return std::get<std::string>(inner); }
        bool        as_bool()   const { return std::get<bool>(inner); }
        std::shared_ptr<HulkObject> as_object() const { return std::get<std::shared_ptr<HulkObject>>(inner); }
        std::shared_ptr<HulkVector> as_vector() const { return std::get<std::shared_ptr<HulkVector>>(inner); }

        // Representación textual para print() — implementado en hulk_value.cpp
        std::string to_string() const;

        // Truthiness: false y nil son falsy, todo lo demás es truthy
        bool is_truthy() const {
            if (is_nil())  return false;
            if (is_bool()) return as_bool();
            return true;
        }
    };

} // namespace Hulk

#endif // HULK_VALUE_H
