#include "protocolDecl.h"

namespace Hulk {

    ProtocolDecl::ProtocolDecl(const std::string& name,
                               std::vector<ProtocolMethodSig> methodSigs)
        : name(name), parentName(""), methodSigs(std::move(methodSigs)) {}

    ProtocolDecl::ProtocolDecl(const std::string& name,
                               const std::string& parentName,
                               std::vector<ProtocolMethodSig> methodSigs)
        : name(name), parentName(parentName), methodSigs(std::move(methodSigs)) {}

    const std::string& ProtocolDecl::GetName() const { return name; }
    const std::string& ProtocolDecl::GetParentName() const { return parentName; }
    bool ProtocolDecl::HasParent() const { return !parentName.empty(); }
    const std::vector<ProtocolMethodSig>& ProtocolDecl::GetMethodSigs() const { return methodSigs; }

    std::string ProtocolDecl::ToString() const {
        std::string result = "protocol " + name;
        if (!parentName.empty())
            result += " extends " + parentName;
        result += " { ";
        for (const auto& sig : methodSigs) {
            result += sig.name + "(";
            for (size_t i = 0; i < sig.params.size(); ++i) {
                if (i > 0) result += ", ";
                result += sig.params[i].name;
                if (sig.params[i].HasTypeAnnotation())
                    result += " : " + sig.params[i].typeAnnotation;
            }
            result += ") : " + sig.returnType + "; ";
        }
        result += "}";
        return result;
    }

}
