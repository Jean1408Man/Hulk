#ifndef TYPE_MEMBER_METHOD_H
#define TYPE_MEMBER_METHOD_H

#include "../abs_nodes/ast.h"
#include "../functions/param.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class TypeMemberMethod : public ASTnode {
    private:
        std::string name;
        std::vector<Param> params;
        std::string returnTypeAnnotation;
        std::unique_ptr<ASTnode> body;

    public:
        TypeMemberMethod(const std::string& name,
                         std::vector<Param> params,
                         std::unique_ptr<ASTnode> body);

        TypeMemberMethod(const std::string& name,
                         std::vector<Param> params,
                         const std::string& returnTypeAnnotation,
                         std::unique_ptr<ASTnode> body);

        const std::string& GetName() const;
        const std::vector<Param>& GetParams() const;
        const std::string& GetReturnTypeAnnotation() const;
        bool HasReturnTypeAnnotation() const;
        ASTnode* GetBody() const;

        std::string ToString() const override;
    };

}

#endif
