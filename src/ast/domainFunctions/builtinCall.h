#ifndef BUILTIN_CALL_H
#define BUILTIN_CALL_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    enum class BuiltinFunc {
        Sqrt,
        Sin,
        Cos,
        Exp,
        Log,
        Rand,
        Range
    };

    class BuiltinCall : public Expr {
    private:
        BuiltinFunc func;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        BuiltinCall(BuiltinFunc func,
                    std::vector<std::unique_ptr<Expr>> args);

        BuiltinFunc GetFunc() const;
        const std::vector<std::unique_ptr<Expr>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
