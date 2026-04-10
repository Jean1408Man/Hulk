#ifndef FOR_H
#define FOR_H
#include "../abs_nodes/ast.h"
#include <memory>

namespace Hulk {

    class For : public ASTnode {
    private:
        std::string varName;
        std::unique_ptr<ASTnode> iterable;
        std::unique_ptr<ASTnode> body;

    public:
        For(const std::string& varName, std::unique_ptr<ASTnode> iterable, std::unique_ptr<ASTnode> body);

        const std::string& GetVarName() const;
        ASTnode* GetIterable() const;
        ASTnode* GetBody() const;

        std::string ToString() const override;
    };

}

#endif