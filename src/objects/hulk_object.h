#ifndef HULK_OBJECT_H
#define HULK_OBJECT_H

#include "hulk_value.h"
#include "../ast/functions/param.h"
#include "../ast/abs_nodes/expr.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Hulk {

    // -----------------------------------------------------------------------
    // Definición de un método tal como se almacena en runtime
    // -----------------------------------------------------------------------
    struct MethodDef {
        std::vector<Param> params;
        Expr* body;             // puntero no-owning al AST (vive en Program)
        bool is_override = false;
    };

    // -----------------------------------------------------------------------
    // Definición de un tipo (clase) tal como se registra en runtime
    // -----------------------------------------------------------------------
    struct TypeDef {
        std::string name;
        std::string parent_name;                          // "" si no tiene padre
        std::vector<Param> ctor_params;
        std::vector<Expr*> parent_args;                   // expresiones pasadas al ctor del padre
        std::vector<std::pair<std::string, Expr*>> attributes; // (nombre, inicializador)
        std::unordered_map<std::string, MethodDef> methods;
    };

    // -----------------------------------------------------------------------
    // Instancia en runtime de un tipo HULK
    // -----------------------------------------------------------------------
    struct HulkObject {
        std::string type_name;
        std::unordered_map<std::string, HulkValue> fields;

        explicit HulkObject(const std::string& type) : type_name(type) {}
    };

} // namespace Hulk

#endif // HULK_OBJECT_H
