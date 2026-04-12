#ifndef BUILTIN_CALL_H
#define BUILTIN_CALL_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    enum class BuiltinFunc {
        Exp,
        Log,
        Rand,
        Range
    };

    class BuiltinCall : public ASTnode {
    private:
        BuiltinFunc func;
        std::vector<std::unique_ptr<ASTnode>> args;

    public:
        BuiltinCall(BuiltinFunc func,
                    std::vector<std::unique_ptr<ASTnode>> args);

        BuiltinFunc GetFunc() const;
        const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
