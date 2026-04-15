#ifndef TYPE_DECL_H
#define TYPE_DECL_H

#include "../abs_nodes/decl.h"
#include "../abs_nodes/expr.h"
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
        std::unique_ptr<Decl> node;

        TypeMember()
            : kind(Kind::Attribute), node(nullptr) {}

        TypeMember(Kind kind, std::unique_ptr<Decl> node)
            : kind(kind), node(std::move(node)) {}
    };

    class TypeDecl : public Decl {
    private:
        std::string name;
        std::vector<Param> ctorParams;
        std::string parentName;
        std::vector<std::unique_ptr<Expr>> parentArgs;
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
                 std::vector<std::unique_ptr<Expr>> parentArgs,
                 std::vector<TypeMember> members);

        const std::string& GetName() const;
        const std::vector<Param>& GetCtorParams() const;
        bool HasCtorParams() const;
        const std::string& GetParentName() const;
        bool HasParent() const;
        const std::vector<std::unique_ptr<Expr>>& GetParentArgs() const;
        const std::vector<TypeMember>& GetMembers() const;

        std::string ToString() const override;
    };

}

#endif
