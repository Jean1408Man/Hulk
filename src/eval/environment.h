#ifndef HULK_ENVIRONMENT_H
#define HULK_ENVIRONMENT_H

#include "../objects/hulk_value.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace Hulk {

    // -----------------------------------------------------------------------
    // Environment — tabla de símbolos con scopes encadenados.
    //
    // Cada scope tiene un puntero a su padre. La búsqueda sube la cadena.
    // La asignación destructiva (:=) también sube la cadena hasta encontrar
    // el scope donde se declaró la variable.
    // -----------------------------------------------------------------------
    class Environment {
    public:
        explicit Environment(std::shared_ptr<Environment> parent = nullptr)
            : parent_(std::move(parent)) {}

        // Declara una variable en el scope ACTUAL (let, param, etc.)
        void define(const std::string& name, HulkValue value) {
            vars_[name] = std::move(value);
        }

        // Lee una variable subiendo por la cadena de scopes
        const HulkValue& get(const std::string& name) const {
            auto it = vars_.find(name);
            if (it != vars_.end()) return it->second;
            if (parent_) return parent_->get(name);
            throw std::runtime_error("Variable no definida: '" + name + "'");
        }

        // Asignación destructiva := — modifica en el scope donde fue declarada
        void assign(const std::string& name, HulkValue value) {
            auto it = vars_.find(name);
            if (it != vars_.end()) {
                it->second = std::move(value);
                return;
            }
            if (parent_) {
                parent_->assign(name, std::move(value));
                return;
            }
            throw std::runtime_error("Variable no definida para asignar: '" + name + "'");
        }

        // Verifica si existe en este scope o en algún padre
        bool contains(const std::string& name) const {
            if (vars_.count(name)) return true;
            if (parent_) return parent_->contains(name);
            return false;
        }

        std::shared_ptr<Environment> parent() const { return parent_; }

    private:
        std::unordered_map<std::string, HulkValue> vars_;
        std::shared_ptr<Environment> parent_;
    };

} // namespace Hulk

#endif // HULK_ENVIRONMENT_H
