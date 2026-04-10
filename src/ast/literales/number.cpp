#include "number.h"

namespace Hulk {

    // Constructor: inicializa el miembro 'value'
    Number::Number(double val) : value(val) {}

    // Retorna el valor contenido
    double Number::GetValue() const {
        return value;
    }

    // Devuelve una representación legible del nodo
    std::string Number::ToString() const {
        return "NumberLiteral: " + std::to_string(value);
    }

}