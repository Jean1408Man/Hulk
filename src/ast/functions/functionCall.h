#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class FunctionCall : public ASTnode {
    private:
        std::string name;
        std::vector<std::unique_ptr<ASTnode>> args;

    public:
        FunctionCall(const std::string& name,
                     std::vector<std::unique_ptr<ASTnode>> args);

        const std::string& GetName() const;
        const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
