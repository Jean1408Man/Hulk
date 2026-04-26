#ifndef TYPE_MEMBER_METHOD_H
#define TYPE_MEMBER_METHOD_H

#include "../abs_nodes/decl.h"
#include "../abs_nodes/expr.h"
#include "../functions/param.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class TypeMemberMethod : public Decl {
    private:
        std::string name;
        std::vector<Param> params;
        std::string returnTypeAnnotation;
        std::unique_ptr<Expr> body;

    public:
        TypeMemberMethod(const std::string& name,
                         std::vector<Param> params,
                         std::unique_ptr<Expr> body);

        TypeMemberMethod(const std::string& name,
                         std::vector<Param> params,
                         const std::string& returnTypeAnnotation,
                         std::unique_ptr<Expr> body);

        const std::string& GetName() const;
        const std::vector<Param>& GetParams() const;
        const std::string& GetReturnTypeAnnotation() const;
        bool HasReturnTypeAnnotation() const;
        Expr* GetBody() const;

        std::string ToString() const override;
        void accept(DeclVisitor& v) override;
    };

}

#endif
