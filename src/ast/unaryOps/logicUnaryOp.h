#ifndef LOGIC_UNARY_OP_H
#define LOGIC_UNARY_OP_H

#include "../abs_nodes/unaryOp.h"
#include <string>

namespace Hulk {

    class LogicUnaryOp : public UnaryOp {
    public:
        // Constructor: usamos 'explicit' por buena práctica
        explicit LogicUnaryOp(std::unique_ptr<Expr> arg);

        virtual ~LogicUnaryOp() = default;

        // Solo declaramos el método, la lógica va al .cpp
        std::string ToString() const override;
    };

}

#endif
