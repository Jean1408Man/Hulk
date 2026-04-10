#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>

namespace Hulk {

    class ASTnode {
    public:
        // Destructor virtual: Vital para que al borrar un ASTnode* // se limpie correctamente la memoria de la clase derivada.
        virtual ~ASTnode() = default;

        // Método virtual puro: Hace que la clase sea abstracta.
        // Cada nodo decidirá cómo representarse a sí mismo.
        virtual std::string ToString() const = 0;
    };

}

#endif