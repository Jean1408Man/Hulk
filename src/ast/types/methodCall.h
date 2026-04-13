#ifndef METHOD_CALL_H
#define METHOD_CALL_H

#include "../abs_nodes/expr.h"
#include <memory>
#include <string>
#include <vector>

namespace Hulk {

    class MethodCall : public Expr {
    private:
        std::unique_ptr<Expr> object;
        std::string methodName;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        MethodCall(std::unique_ptr<Expr> object,
                   const std::string& methodName,
                   std::vector<std::unique_ptr<Expr>> args);

        Expr* GetObject() const;
        const std::string& GetMethodName() const;
        const std::vector<std::unique_ptr<Expr>>& GetArgs() const;

        std::string ToString() const override;
    };

}

#endif
