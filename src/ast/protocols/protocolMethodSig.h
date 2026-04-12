#ifndef PROTOCOL_METHOD_SIG_H
#define PROTOCOL_METHOD_SIG_H

#include "../functions/param.h"
#include <string>
#include <vector>

namespace Hulk {

    struct ProtocolMethodSig {
        std::string name;
        std::vector<Param> params;
        std::string returnType;

        ProtocolMethodSig(const std::string& name,
                          std::vector<Param> params,
                          const std::string& returnType)
            : name(name), params(std::move(params)), returnType(returnType) {}
    };

}

#endif
