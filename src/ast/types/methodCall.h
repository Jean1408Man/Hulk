#ifndef METHOD_CALL_H
#define METHOD_CALL_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class MethodCall : public ASTnode {
    private:
        std::unique_ptr<ASTnode> object;
        std::string methodName;
        std::vector<std::unique_ptr<ASTnode>> args;

    public:
        MethodCall(std::unique_ptr<ASTnode> object,
                   const std::string& methodName,
                   std::vector<std::unique_ptr<ASTnode>> args);

        ASTnode* GetObject() const;
        const std::string& GetMethodName() const;
        const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
