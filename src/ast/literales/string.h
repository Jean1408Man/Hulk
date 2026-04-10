#ifndef STRING_NODE_H
#define STRING_NODE_H

#include "../abs_nodes/ast.h"
#include <string>

namespace Hulk {

    class String : public ASTnode {
    private:
        std::string value;

    public:
        explicit String(const std::string& val);
        std::string GetValue() const;
        std::string ToString() const override;
    };

}

#endif
