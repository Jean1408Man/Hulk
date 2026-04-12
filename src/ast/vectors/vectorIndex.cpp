#include "vectorIndex.h"

namespace Hulk {

    VectorIndex::VectorIndex(std::unique_ptr<ASTnode> vector,
                             std::unique_ptr<ASTnode> index)
        : vector(std::move(vector)), index(std::move(index)) {}

    ASTnode* VectorIndex::GetVector() const { return vector.get(); }
    ASTnode* VectorIndex::GetIndex() const { return index.get(); }

    std::string VectorIndex::ToString() const {
        return vector->ToString() + "[" + index->ToString() + "]";
    }

}
