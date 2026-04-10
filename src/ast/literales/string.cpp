#include "string.h"

namespace Hulk {

    // Constructor: inicializa el miembro 'value'
    String::String(const std::string& val) : value(val) {}

    // Retorna el valor contenido
    std::string String::GetValue() const {
        return value;
    }

    // Devuelve una representación legible del nodo
    std::string String::ToString() const {
        return "StringLiteral: " + value;
    }

}