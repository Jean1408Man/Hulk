#include "vectorLiteral.h"

namespace Hulk {

    VectorLiteral::VectorLiteral(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements)) {}

    const std::vector<std::unique_ptr<Expr>>& VectorLiteral::GetElements() const {
        return elements;
    }

    std::string VectorLiteral::ToString() const {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->ToString();
        }
        result += "]";
        return result;
    }

}
