#ifndef PROTOCOL_DECL_H
#define PROTOCOL_DECL_H

#include "../abs_nodes/decl.h"
#include "protocolMethodSig.h"
#include <string>
#include <vector>

namespace Hulk {

    class ProtocolDecl : public Decl {
    private:
        std::string name;
        std::string parentName;
        std::vector<ProtocolMethodSig> methodSigs;

    public:
        ProtocolDecl(const std::string& name,
                     std::vector<ProtocolMethodSig> methodSigs);

        ProtocolDecl(const std::string& name,
                     const std::string& parentName,
                     std::vector<ProtocolMethodSig> methodSigs);

        const std::string& GetName() const;
        const std::string& GetParentName() const;
        bool HasParent() const;
        const std::vector<ProtocolMethodSig>& GetMethodSigs() const;

        std::string ToString() const override;
    };

}

#endif
