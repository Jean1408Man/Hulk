#include "boolean.h"

namespace Hulk {

    // Constructor: inicializa el miembro 'value'
    Boolean::Boolean(bool val) : value(val) {}

    // Retorna el valor contenido
    bool Boolean::GetValue() const {
        return value;
    }

    // Devuelve una representación legible del nodo
    std::string Boolean::ToString() const {
        return "BooleanLiteral: " + std::string(value ? "true" : "false");
    }

}