#ifndef BASE_CALL_H
#define BASE_CALL_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <vector>

namespace Hulk {

    class BaseCall : public ASTnode {
    private:
        std::vector<std::unique_ptr<ASTnode>> args;

    public:
        explicit BaseCall(std::vector<std::unique_ptr<ASTnode>> args);

        const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
