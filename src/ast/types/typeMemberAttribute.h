#ifndef TYPE_MEMBER_ATTRIBUTE_H
#define TYPE_MEMBER_ATTRIBUTE_H

#include "../abs_nodes/ast.h"
#include <memory>
#include <string>

namespace Hulk {

    class TypeMemberAttribute : public ASTnode {
    private:
        std::string name;
        std::string typeAnnotation;
        std::unique_ptr<ASTnode> initializer;

    public:
        TypeMemberAttribute(const std::string& name,
                            std::unique_ptr<ASTnode> initializer);

        TypeMemberAttribute(const std::string& name,
                            const std::string& typeAnnotation,
                            std::unique_ptr<ASTnode> initializer);

        const std::string& GetName() const;
        const std::string& GetTypeAnnotation() const;
        bool HasTypeAnnotation() const;
        ASTnode* GetInitializer() const;

        std::string ToString() const override;
    };

}

#endif
