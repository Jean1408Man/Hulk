#ifndef BASE_CALL_H
#define BASE_CALL_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <vector>

namespace Hulk {

    class BaseCall : public Expr {
    private:
        std::vector<std::unique_ptr<Expr>> args;

    public:
        explicit BaseCall(std::vector<std::unique_ptr<Expr>> args);

        const std::vector<std::unique_ptr<Expr>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
