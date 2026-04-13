#include "vectorIndex.h"

namespace Hulk {

    VectorIndex::VectorIndex(std::unique_ptr<Expr> vector,
                             std::unique_ptr<Expr> index)
        : vector(std::move(vector)), index(std::move(index)) {}

    Expr* VectorIndex::GetVector() const { return vector.get(); }
    Expr* VectorIndex::GetIndex() const { return index.get(); }

    std::string VectorIndex::ToString() const {
        return vector->ToString() + "[" + index->ToString() + "]";
    }

}
