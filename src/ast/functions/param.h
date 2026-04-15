#ifndef PARAM_H
#define PARAM_H

#include <string>

namespace Hulk {

    struct Param {
        std::string name;
        std::string typeAnnotation;

        Param()
            : name(""), typeAnnotation("") {}

        explicit Param(const std::string& name)
            : name(name), typeAnnotation("") {}

        Param(const std::string& name, const std::string& typeAnnotation)
            : name(name), typeAnnotation(typeAnnotation) {}

        bool HasTypeAnnotation() const { return !typeAnnotation.empty(); }
    };

}

#endif
