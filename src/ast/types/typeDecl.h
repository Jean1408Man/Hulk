#ifndef TYPE_DECL_H
#define TYPE_DECL_H

#include "../abs_nodes/ast.h"
#include "../functions/param.h"
#include "typeMemberAttribute.h"
#include "typeMemberMethod.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    struct TypeMember {
        enum class Kind { Attribute, Method };
        Kind kind;
        std::unique_ptr<ASTnode> node;

        TypeMember(Kind kind, std::unique_ptr<ASTnode> node)
            : kind(kind), node(std::move(node)) {}
    };

    class TypeDecl : public ASTnode {
    private:
        std::string name;
        std::vector<Param> ctorParams;
        std::string parentName;
        std::vector<std::unique_ptr<ASTnode>> parentArgs;
        std::vector<TypeMember> members;

    public:
        TypeDecl(const std::string& name,
                 std::vector<TypeMember> members);

        TypeDecl(const std::string& name,
                 std::vector<Param> ctorParams,
                 std::vector<TypeMember> members);

        TypeDecl(const std::string& name,
                 const std::string& parentName,
                 std::vector<TypeMember> members);

        TypeDecl(const std::string& name,
                 std::vector<Param> ctorParams,
                 const std::string& parentName,
                 std::vector<std::unique_ptr<ASTnode>> parentArgs,
                 std::vector<TypeMember> members);

        const std::string& GetName() const;
        const std::vector<Param>& GetCtorParams() const;
        bool HasCtorParams() const;
        const std::string& GetParentName() const;
        bool HasParent() const;
        const std::vector<std::unique_ptr<ASTnode>>& GetParentArgs() const;
        const std::vector<TypeMember>& GetMembers() const;

        std::string ToString() const override;
    };

}

#endif
