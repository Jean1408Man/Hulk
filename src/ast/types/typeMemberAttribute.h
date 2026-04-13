#ifndef TYPE_MEMBER_ATTRIBUTE_H
#define TYPE_MEMBER_ATTRIBUTE_H

#include "../abs_nodes/decl.h"
#include "../abs_nodes/expr.h"
#include <memory>
#include <string>

namespace Hulk {

    class TypeMemberAttribute : public Decl {
    private:
        std::string name;
        std::string typeAnnotation;
        std::unique_ptr<Expr> initializer;

    public:
        TypeMemberAttribute(const std::string& name,
                            std::unique_ptr<Expr> initializer);

        TypeMemberAttribute(const std::string& name,
                            const std::string& typeAnnotation,
                            std::unique_ptr<Expr> initializer);

        const std::string& GetName() const;
        const std::string& GetTypeAnnotation() const;
        bool HasTypeAnnotation() const;
        Expr* GetInitializer() const;

        std::string ToString() const override;
    };

}

#endif
