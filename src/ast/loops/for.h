#ifndef FOR_H
#define FOR_H
#include "../abs_nodes/expr.h"
#include <memory>

namespace Hulk {

    class For : public Expr {
    private:
        std::string varName;
        std::unique_ptr<Expr> iterable;
        std::unique_ptr<Expr> body;

    public:
        For(const std::string& varName, std::unique_ptr<Expr> iterable, std::unique_ptr<Expr> body);

        const std::string& GetVarName() const;
        Expr* GetIterable() const;
        Expr* GetBody() const;

        std::string ToString() const override;
    };

}

#endif