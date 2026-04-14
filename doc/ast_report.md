# Informe total del AST

Este documento reúne el AST actual definido en `src/ast`, incluyendo:

- nodos base;
- nodos concretos;
- estructuras auxiliares que forman parte del modelo del AST;
- código real de cada nodo, tomado de sus `.h` y `.cpp`.

## Mapa general

Directorios del AST:

- `src/ast/abs_nodes`
- `src/ast/assignments`
- `src/ast/binOps`
- `src/ast/conditionals`
- `src/ast/domainFunctions`
- `src/ast/functions`
- `src/ast/literales`
- `src/ast/loops`
- `src/ast/others`
- `src/ast/protocols`
- `src/ast/types`
- `src/ast/unaryOps`
- `src/ast/variables`
- `src/ast/vectors`

## Nodos base

### `ASTnode`

Archivos:

- [src/ast/abs_nodes/ast.h](/home/jean/School/Compilacion/Hulk/src/ast/abs_nodes/ast.h:1)

```cpp
class ASTnode {
public:
    virtual ~ASTnode() = default;
    virtual std::string ToString() const = 0;
};
```

### `BinOp`

Archivo:

- [src/ast/abs_nodes/binOp.h](/home/jean/School/Compilacion/Hulk/src/ast/abs_nodes/binOp.h:1)

```cpp
class BinOp : public ASTnode {
protected:
    std::unique_ptr<ASTnode> left;
    std::unique_ptr<ASTnode> right;

public:
    BinOp(std::unique_ptr<ASTnode> leftNode, std::unique_ptr<ASTnode> rightNode)
        : left(std::move(leftNode)), right(std::move(rightNode)) {}

    virtual ~BinOp() = default;

    ASTnode* GetLeft() const { return left.get(); }
    ASTnode* GetRight() const { return right.get(); }
};
```

### `UnaryOp`

Archivo:

- [src/ast/abs_nodes/unaryOp.h](/home/jean/School/Compilacion/Hulk/src/ast/abs_nodes/unaryOp.h:1)

```cpp
class UnaryOp : public ASTnode {
protected:
    std::unique_ptr<ASTnode> operand;

public:
    explicit UnaryOp(std::unique_ptr<ASTnode> arg)
        : operand(std::move(arg)) {}

    virtual ~UnaryOp() = default;

    ASTnode* GetOperand() const {
        return operand.get();
    }
};
```

## Literales

### `Number`

Archivos:

- [src/ast/literales/number.h](/home/jean/School/Compilacion/Hulk/src/ast/literales/number.h:1)
- [src/ast/literales/number.cpp](/home/jean/School/Compilacion/Hulk/src/ast/literales/number.cpp:1)

```cpp
class Number : public ASTnode {
private:
    double value;

public:
    explicit Number(double val);
    double GetValue() const;
    std::string ToString() const override;
};
```

```cpp
Number::Number(double val) : value(val) {}

double Number::GetValue() const {
    return value;
}

std::string Number::ToString() const {
    return "NumberLiteral: " + std::to_string(value);
}
```

### `Boolean`

Archivos:

- [src/ast/literales/boolean.h](/home/jean/School/Compilacion/Hulk/src/ast/literales/boolean.h:1)
- [src/ast/literales/boolean.cpp](/home/jean/School/Compilacion/Hulk/src/ast/literales/boolean.cpp:1)

```cpp
class Boolean : public ASTnode {
private:
    bool value;

public:
    explicit Boolean(bool val);
    bool GetValue() const;
    std::string ToString() const override;
};
```

```cpp
Boolean::Boolean(bool val) : value(val) {}

bool Boolean::GetValue() const {
    return value;
}

std::string Boolean::ToString() const {
    return "BooleanLiteral: " + std::string(value ? "true" : "false");
}
```

### `String`

Archivos:

- [src/ast/literales/string.h](/home/jean/School/Compilacion/Hulk/src/ast/literales/string.h:1)
- [src/ast/literales/string.cpp](/home/jean/School/Compilacion/Hulk/src/ast/literales/string.cpp:1)

```cpp
class String : public ASTnode {
private:
    std::string value;

public:
    explicit String(const std::string& val);
    std::string GetValue() const;
    std::string ToString() const override;
};
```

```cpp
String::String(const std::string& val) : value(val) {}

std::string String::GetValue() const {
    return value;
}

std::string String::ToString() const {
    return "StringLiteral: " + value;
}
```

## Variables

### `VariableReference`

Archivos:

- [src/ast/variables/variableReference.h](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableReference.h:1)
- [src/ast/variables/variableReference.cpp](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableReference.cpp:1)

```cpp
class VariableReference : public ASTnode {
private:
    std::string name;

public:
    explicit VariableReference(const std::string& varName);
    std::string GetName() const;
    std::string ToString() const override;
};
```

```cpp
VariableReference::VariableReference(const std::string& varName)
    : name(varName) {}

std::string VariableReference::GetName() const {
    return name;
}

std::string VariableReference::ToString() const {
    return "Variable(" + name + ")";
}
```

### `VariableBinding`

Archivos:

- [src/ast/variables/variableBinding.h](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableBinding.h:1)
- [src/ast/variables/variableBinding.cpp](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableBinding.cpp:1)

```cpp
class VariableBinding : public ASTnode {
private:
    std::string name;
    std::string typeAnnotation;
    std::unique_ptr<ASTnode> initializer;

public:
    VariableBinding(const std::string& name, std::unique_ptr<ASTnode> initializer);
    VariableBinding(const std::string& name, const std::string& typeAnnotation,
                    std::unique_ptr<ASTnode> initializer);

    const std::string& GetName() const;
    const std::string& GetTypeAnnotation() const;
    bool HasTypeAnnotation() const;
    ASTnode* GetInitializer() const;
    std::string ToString() const override;
};
```

```cpp
VariableBinding::VariableBinding(const std::string& name,
                                 std::unique_ptr<ASTnode> initializer)
    : name(name), typeAnnotation(""), initializer(std::move(initializer)) {}

VariableBinding::VariableBinding(const std::string& name,
                                 const std::string& typeAnnotation,
                                 std::unique_ptr<ASTnode> initializer)
    : name(name), typeAnnotation(typeAnnotation), initializer(std::move(initializer)) {}

std::string VariableBinding::ToString() const {
    std::string result = name;
    if (!typeAnnotation.empty()) result += " : " + typeAnnotation;
    result += " = " + initializer->ToString();
    return result;
}
```

### `LetIn`

Archivos:

- [src/ast/variables/letIn.h](/home/jean/School/Compilacion/Hulk/src/ast/variables/letIn.h:1)
- [src/ast/variables/letIn.cpp](/home/jean/School/Compilacion/Hulk/src/ast/variables/letIn.cpp:1)

```cpp
class LetIn : public ASTnode {
private:
    std::vector<std::unique_ptr<VariableBinding>> bindings;
    std::unique_ptr<ASTnode> body;

public:
    LetIn(std::vector<std::unique_ptr<VariableBinding>> bindings,
          std::unique_ptr<ASTnode> body);

    const std::vector<std::unique_ptr<VariableBinding>>& GetBindings() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

```cpp
LetIn::LetIn(std::vector<std::unique_ptr<VariableBinding>> bindings,
             std::unique_ptr<ASTnode> body)
    : bindings(std::move(bindings)), body(std::move(body)) {}

std::string LetIn::ToString() const {
    std::string result = "let ";
    for (size_t i = 0; i < bindings.size(); ++i) {
        if (i > 0) result += ", ";
        result += bindings[i]->ToString();
    }
    result += " in " + body->ToString();
    return result;
}
```

## Asignaciones

### `DestructiveAssign`

Archivos:

- [src/ast/assignments/desctructiveAssign.h](/home/jean/School/Compilacion/Hulk/src/ast/assignments/desctructiveAssign.h:1)
- [src/ast/assignments/desctructiveAssign.cpp](/home/jean/School/Compilacion/Hulk/src/ast/assignments/desctructiveAssign.cpp:1)

```cpp
class DestructiveAssign : public ASTnode {
private:
    std::string name;
    std::unique_ptr<ASTnode> value;

public:
    DestructiveAssign(const std::string& varName, std::unique_ptr<ASTnode> val);
    const std::string& GetName() const;
    ASTnode* GetValue() const;
    std::string ToString() const override;
};
```

```cpp
DestructiveAssign::DestructiveAssign(const std::string& varName,
                                     std::unique_ptr<ASTnode> val)
    : name(varName), value(std::move(val)) {}

std::string DestructiveAssign::ToString() const {
    return name + " := " + value->ToString();
}
```

### `DestructiveAssignMember`

Archivos:

- [src/ast/assignments/destructiveAssignMember.h](/home/jean/School/Compilacion/Hulk/src/ast/assignments/destructiveAssignMember.h:1)
- [src/ast/assignments/destructiveAssignMember.cpp](/home/jean/School/Compilacion/Hulk/src/ast/assignments/destructiveAssignMember.cpp:1)

```cpp
class DestructiveAssignMember : public ASTnode {
private:
    std::unique_ptr<ASTnode> object;
    std::string memberName;
    std::unique_ptr<ASTnode> value;

public:
    DestructiveAssignMember(std::unique_ptr<ASTnode> object,
                            const std::string& memberName,
                            std::unique_ptr<ASTnode> value);

    ASTnode* GetObject() const;
    const std::string& GetMemberName() const;
    ASTnode* GetValue() const;
    std::string ToString() const override;
};
```

```cpp
DestructiveAssignMember::DestructiveAssignMember(std::unique_ptr<ASTnode> object,
                                                 const std::string& memberName,
                                                 std::unique_ptr<ASTnode> value)
    : object(std::move(object)), memberName(memberName), value(std::move(value)) {}

std::string DestructiveAssignMember::ToString() const {
    return object->ToString() + "." + memberName + " := " + value->ToString();
}
```

## Operaciones binarias

### `ArithmeticOp` y `ArithmeticBinOp`

Archivos:

- [src/ast/binOps/arithmeticBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/arithmeticBinOp.h:1)
- [src/ast/binOps/arithmeticBinOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/binOps/arithmeticBinOp.cpp:1)

```cpp
enum class ArithmeticOp { Plus, Minus, Mult, Div, Pow };

class ArithmeticBinOp : public BinOp {
private:
    ArithmeticOp op;

public:
    ArithmeticBinOp(std::unique_ptr<ASTnode> left, ArithmeticOp operation,
                    std::unique_ptr<ASTnode> right);

    ArithmeticOp GetOperator() const { return op; }
    std::string ToString() const override;
};
```

```cpp
std::string ArithmeticBinOp::ToString() const {
    std::string s;
    switch(op) {
        case ArithmeticOp::Plus: s = "+"; break;
        case ArithmeticOp::Minus: s = "-"; break;
        case ArithmeticOp::Mult: s = "*"; break;
        case ArithmeticOp::Div: s = "/"; break;
        case ArithmeticOp::Pow: s = "^"; break;
    }
    return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
}
```

### `LogicOp` y `LogicBinOp`

Archivos:

- [src/ast/binOps/logicBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/logicBinOp.h:1)
- [src/ast/binOps/logicBinOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/binOps/logicBinOp.cpp:1)

```cpp
enum class LogicOp {
    And, Or, Greater, Less, Equal, NotEqual, GreaterEqual, LessEqual
};

class LogicBinOp : public BinOp {
private:
    LogicOp op;

public:
    LogicBinOp(std::unique_ptr<ASTnode> left, LogicOp operation,
               std::unique_ptr<ASTnode> right);

    LogicOp GetOperator() const { return op; }
    std::string ToString() const override;
};
```

```cpp
std::string LogicBinOp::ToString() const {
    std::string s;
    switch(op) {
        case LogicOp::And: s = "&"; break;
        case LogicOp::Or: s = "|"; break;
        case LogicOp::Equal: s = "=="; break;
        case LogicOp::NotEqual: s = "!="; break;
        case LogicOp::Greater: s = ">"; break;
        case LogicOp::Less: s = "<"; break;
        case LogicOp::GreaterEqual: s = ">="; break;
        case LogicOp::LessEqual: s = "<="; break;
        default: s = "op"; break;
    }
    return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
}
```

### `StringOp` y `StringBinOp`

Archivos:

- [src/ast/binOps/stringBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/stringBinOp.h:1)
- [src/ast/binOps/stringBinOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/binOps/stringBinOp.cpp:1)

```cpp
enum class StringOp { Concat, SpaceConcat };

class StringBinOp : public BinOp {
private:
    StringOp op;

public:
    StringBinOp(std::unique_ptr<ASTnode> left, StringOp operation,
                std::unique_ptr<ASTnode> right);

    StringOp GetOperator() const;
    std::string ToString() const override;
};
```

```cpp
std::string StringBinOp::ToString() const {
    std::string s;
    switch(op) {
        case StringOp::Concat:
            s = "@";
            return "(" + left->ToString() + s + right->ToString() + ")";
        case StringOp::SpaceConcat:
            s = "@@";
            return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
        default:
            s = "op";
            break;
    }
    return "(" + left->ToString() + " " + s + " " + right->ToString() + ")";
}
```

## Operaciones unarias

### `ArithUnaryType` y `ArithmeticUnaryOp`

Archivos:

- [src/ast/unaryOps/arithmeticUnaryOp.h](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/arithmeticUnaryOp.h:1)
- [src/ast/unaryOps/arithmeticUnaryOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/arithmeticUnaryOp.cpp:1)

```cpp
enum class ArithUnaryType { Minus, Sin, Cos, Sqrt };

class ArithmeticUnaryOp : public UnaryOp {
private:
    ArithUnaryType type;

public:
    ArithmeticUnaryOp(ArithUnaryType opType, std::unique_ptr<ASTnode> arg);
    ArithUnaryType GetType() const { return type; }
    std::string ToString() const override;
};
```

```cpp
std::string ArithmeticUnaryOp::ToString() const {
    std::string opName;
    switch (type) {
        case ArithUnaryType::Minus: return "-" + operand->ToString();
        case ArithUnaryType::Sin:   opName = "sin"; break;
        case ArithUnaryType::Cos:   opName = "cos"; break;
        case ArithUnaryType::Sqrt:  opName = "sqrt"; break;
    }
    return opName + "(" + operand->ToString() + ")";
}
```

### `LogicUnaryOp`

Archivos:

- [src/ast/unaryOps/logicUnaryOp.h](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/logicUnaryOp.h:1)
- [src/ast/unaryOps/logicUnaryOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/logicUnaryOp.cpp:1)

```cpp
class LogicUnaryOp : public UnaryOp {
public:
    explicit LogicUnaryOp(std::unique_ptr<ASTnode> arg);
    std::string ToString() const override;
};
```

```cpp
LogicUnaryOp::LogicUnaryOp(std::unique_ptr<ASTnode> arg)
    : UnaryOp(std::move(arg)) {}

std::string LogicUnaryOp::ToString() const {
    return "!(" + operand->ToString() + ")";
}
```

## Condicionales

### `ElifBranch`

Archivo:

- [src/ast/conditionals/elifStmt.h](/home/jean/School/Compilacion/Hulk/src/ast/conditionals/elifStmt.h:1)

`ElifBranch` no es un nodo AST independiente; es una estructura auxiliar usada dentro de `IfStmt`.

```cpp
struct ElifBranch {
    std::unique_ptr<ASTnode> condition;
    std::unique_ptr<ASTnode> body;

    ElifBranch(std::unique_ptr<ASTnode> cond, std::unique_ptr<ASTnode> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};
```

### `IfStmt`

Archivos:

- [src/ast/conditionals/ifStmt.h](/home/jean/School/Compilacion/Hulk/src/ast/conditionals/ifStmt.h:1)
- [src/ast/conditionals/ifStmt.cpp](/home/jean/School/Compilacion/Hulk/src/ast/conditionals/ifStmt.cpp:1)

```cpp
class IfStmt : public ASTnode {
private:
    std::unique_ptr<ASTnode> condition;
    std::unique_ptr<ASTnode> thenBranch;
    std::vector<ElifBranch> elifBranches;
    std::unique_ptr<ASTnode> elseBranch;

public:
    IfStmt(std::unique_ptr<ASTnode> condition,
           std::unique_ptr<ASTnode> thenBranch,
           std::vector<ElifBranch> elifBranches,
           std::unique_ptr<ASTnode> elseBranch);

    ASTnode* GetCondition() const;
    ASTnode* GetThenBranch() const;
    const std::vector<ElifBranch>& GetElifBranches() const;
    ASTnode* GetElseBranch() const;
    std::string ToString() const override;
};
```

```cpp
std::string IfStmt::ToString() const {
    std::string result = "if (" + condition->ToString() + ") " + thenBranch->ToString();
    for (const auto& elif : elifBranches)
        result += " elif (" + elif.condition->ToString() + ") " + elif.body->ToString();
    result += " else " + elseBranch->ToString();
    return result;
}
```

## Bucles

### `WhileStmt`

Archivos:

- [src/ast/loops/while.h](/home/jean/School/Compilacion/Hulk/src/ast/loops/while.h:1)
- [src/ast/loops/while.cpp](/home/jean/School/Compilacion/Hulk/src/ast/loops/while.cpp:1)

```cpp
class WhileStmt : public ASTnode {
private:
    std::unique_ptr<ASTnode> condition;
    std::unique_ptr<ASTnode> body;

public:
    WhileStmt(std::unique_ptr<ASTnode> condition, std::unique_ptr<ASTnode> body);
    ASTnode* GetCondition() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

```cpp
std::string WhileStmt::ToString() const {
    return "while (" + condition->ToString() + ") " + body->ToString();
}
```

### `For`

Archivos:

- [src/ast/loops/for.h](/home/jean/School/Compilacion/Hulk/src/ast/loops/for.h:1)
- [src/ast/loops/for.cpp](/home/jean/School/Compilacion/Hulk/src/ast/loops/for.cpp:1)

```cpp
class For : public ASTnode {
private:
    std::string varName;
    std::unique_ptr<ASTnode> iterable;
    std::unique_ptr<ASTnode> body;

public:
    For(const std::string& varName, std::unique_ptr<ASTnode> iterable,
        std::unique_ptr<ASTnode> body);

    const std::string& GetVarName() const;
    ASTnode* GetIterable() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

```cpp
std::string For::ToString() const {
    return "for " + varName + " in " + iterable->ToString() + ":\n" + body->ToString();
}
```

## Funciones

### `Param`

Archivo:

- [src/ast/functions/param.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/param.h:1)

`Param` es una estructura auxiliar, no un nodo AST.

```cpp
struct Param {
    std::string name;
    std::string typeAnnotation;

    explicit Param(const std::string& name)
        : name(name), typeAnnotation("") {}

    Param(const std::string& name, const std::string& typeAnnotation)
        : name(name), typeAnnotation(typeAnnotation) {}

    bool HasTypeAnnotation() const { return !typeAnnotation.empty(); }
};
```

### `FunctionDecl`

Archivos:

- [src/ast/functions/functionDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionDecl.h:1)
- [src/ast/functions/functionDecl.cpp](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionDecl.cpp:1)

```cpp
class FunctionDecl : public ASTnode {
private:
    std::string name;
    std::vector<Param> params;
    std::string returnTypeAnnotation;
    std::unique_ptr<ASTnode> body;

public:
    FunctionDecl(const std::string& name, std::vector<Param> params,
                 std::unique_ptr<ASTnode> body);

    FunctionDecl(const std::string& name, std::vector<Param> params,
                 const std::string& returnTypeAnnotation,
                 std::unique_ptr<ASTnode> body);

    const std::string& GetName() const;
    const std::vector<Param>& GetParams() const;
    const std::string& GetReturnTypeAnnotation() const;
    bool HasReturnTypeAnnotation() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

```cpp
std::string FunctionDecl::ToString() const {
    std::string result = "function " + name + "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) result += ", ";
        result += params[i].name;
        if (params[i].HasTypeAnnotation())
            result += " : " + params[i].typeAnnotation;
    }
    result += ")";
    if (!returnTypeAnnotation.empty())
        result += " : " + returnTypeAnnotation;
    result += " => " + body->ToString();
    return result;
}
```

### `FunctionCall`

Archivos:

- [src/ast/functions/functionCall.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionCall.h:1)
- [src/ast/functions/functionCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionCall.cpp:1)

```cpp
class FunctionCall : public ASTnode {
private:
    std::string name;
    std::vector<std::unique_ptr<ASTnode>> args;

public:
    FunctionCall(const std::string& name,
                 std::vector<std::unique_ptr<ASTnode>> args);

    const std::string& GetName() const;
    const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;
    std::string ToString() const override;
};
```

```cpp
std::string FunctionCall::ToString() const {
    std::string result = name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i]->ToString();
    }
    result += ")";
    return result;
}
```

### `Lambda`

Archivos:

- [src/ast/functions/lambda.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/lambda.h:1)
- [src/ast/functions/lambda.cpp](/home/jean/School/Compilacion/Hulk/src/ast/functions/lambda.cpp:1)

```cpp
class Lambda : public ASTnode {
private:
    std::vector<Param> params;
    std::string returnTypeAnnotation;
    std::unique_ptr<ASTnode> body;

public:
    Lambda(std::vector<Param> params, std::unique_ptr<ASTnode> body);
    Lambda(std::vector<Param> params, const std::string& returnTypeAnnotation,
           std::unique_ptr<ASTnode> body);

    const std::vector<Param>& GetParams() const;
    const std::string& GetReturnTypeAnnotation() const;
    bool HasReturnTypeAnnotation() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

```cpp
std::string Lambda::ToString() const {
    std::string result = "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) result += ", ";
        result += params[i].name;
        if (params[i].HasTypeAnnotation())
            result += " : " + params[i].typeAnnotation;
    }
    result += ")";
    if (!returnTypeAnnotation.empty())
        result += " : " + returnTypeAnnotation;
    result += " => " + body->ToString();
    return result;
}
```

## Funciones del dominio / builtins

### `Print`

Archivos:

- [src/ast/domainFunctions/print.h](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/print.h:1)
- [src/ast/domainFunctions/print.cpp](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/print.cpp:1)

```cpp
class Print : public ASTnode {
private:
    std::unique_ptr<ASTnode> expr;

public:
    explicit Print(std::unique_ptr<ASTnode> expression);
    ASTnode* GetExpr() const;
    std::string ToString() const override;
};
```

```cpp
Print::Print(std::unique_ptr<ASTnode> expression)
    : expr(std::move(expression)) {}

std::string Print::ToString() const {
    return "print(" + (expr ? expr->ToString() : "null") + ")";
}
```

### `BuiltinFunc` y `BuiltinCall`

Archivos:

- [src/ast/domainFunctions/builtinCall.h](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/builtinCall.h:1)
- [src/ast/domainFunctions/builtinCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/builtinCall.cpp:1)

```cpp
enum class BuiltinFunc { Exp, Log, Rand, Range };

class BuiltinCall : public ASTnode {
private:
    BuiltinFunc func;
    std::vector<std::unique_ptr<ASTnode>> args;

public:
    BuiltinCall(BuiltinFunc func, std::vector<std::unique_ptr<ASTnode>> args);
    BuiltinFunc GetFunc() const;
    const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;
    std::string ToString() const override;
};
```

```cpp
std::string BuiltinCall::ToString() const {
    std::string name;
    switch (func) {
        case BuiltinFunc::Exp:   name = "exp"; break;
        case BuiltinFunc::Log:   name = "log"; break;
        case BuiltinFunc::Rand:  name = "rand"; break;
        case BuiltinFunc::Range: name = "range"; break;
    }
    std::string result = name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i]->ToString();
    }
    result += ")";
    return result;
}
```

## Tipos

### `IsExpr`

Archivos:

- [src/ast/types/isExpr.h](/home/jean/School/Compilacion/Hulk/src/ast/types/isExpr.h:1)
- [src/ast/types/isExpr.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/isExpr.cpp:1)

```cpp
class IsExpr : public ASTnode {
private:
    std::unique_ptr<ASTnode> expr;
    std::string typeName;

public:
    IsExpr(std::unique_ptr<ASTnode> expr, const std::string& typeName);
    ASTnode* GetExpr() const;
    const std::string& GetTypeName() const;
    std::string ToString() const override;
};
```

```cpp
std::string IsExpr::ToString() const {
    return expr->ToString() + " is " + typeName;
}
```

### `AsExpr`

Archivos:

- [src/ast/types/asExpr.h](/home/jean/School/Compilacion/Hulk/src/ast/types/asExpr.h:1)
- [src/ast/types/asExpr.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/asExpr.cpp:1)

```cpp
class AsExpr : public ASTnode {
private:
    std::unique_ptr<ASTnode> expr;
    std::string typeName;

public:
    AsExpr(std::unique_ptr<ASTnode> expr, const std::string& typeName);
    ASTnode* GetExpr() const;
    const std::string& GetTypeName() const;
    std::string ToString() const override;
};
```

```cpp
std::string AsExpr::ToString() const {
    return expr->ToString() + " as " + typeName;
}
```

### `NewExpr`

Archivos:

- [src/ast/types/newExpr.h](/home/jean/School/Compilacion/Hulk/src/ast/types/newExpr.h:1)
- [src/ast/types/newExpr.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/newExpr.cpp:1)

```cpp
class NewExpr : public ASTnode {
private:
    std::string typeName;
    std::vector<std::unique_ptr<ASTnode>> args;

public:
    NewExpr(const std::string& typeName,
            std::vector<std::unique_ptr<ASTnode>> args);

    const std::string& GetTypeName() const;
    const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;
    std::string ToString() const override;
};
```

```cpp
std::string NewExpr::ToString() const {
    std::string result = "new " + typeName + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i]->ToString();
    }
    result += ")";
    return result;
}
```

### `MemberAccess`

Archivos:

- [src/ast/types/memberAccess.h](/home/jean/School/Compilacion/Hulk/src/ast/types/memberAccess.h:1)
- [src/ast/types/memberAccess.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/memberAccess.cpp:1)

```cpp
class MemberAccess : public ASTnode {
private:
    std::unique_ptr<ASTnode> object;
    std::string memberName;

public:
    MemberAccess(std::unique_ptr<ASTnode> object,
                 const std::string& memberName);

    ASTnode* GetObject() const;
    const std::string& GetMemberName() const;
    std::string ToString() const override;
};
```

```cpp
std::string MemberAccess::ToString() const {
    return object->ToString() + "." + memberName;
}
```

### `MethodCall`

Archivos:

- [src/ast/types/methodCall.h](/home/jean/School/Compilacion/Hulk/src/ast/types/methodCall.h:1)
- [src/ast/types/methodCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/methodCall.cpp:1)

```cpp
class MethodCall : public ASTnode {
private:
    std::unique_ptr<ASTnode> object;
    std::string methodName;
    std::vector<std::unique_ptr<ASTnode>> args;

public:
    MethodCall(std::unique_ptr<ASTnode> object,
               const std::string& methodName,
               std::vector<std::unique_ptr<ASTnode>> args);

    ASTnode* GetObject() const;
    const std::string& GetMethodName() const;
    const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;
    std::string ToString() const override;
};
```

```cpp
std::string MethodCall::ToString() const {
    std::string result = object->ToString() + "." + methodName + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i]->ToString();
    }
    result += ")";
    return result;
}
```

### `TypeMemberAttribute`

Archivos:

- [src/ast/types/typeMemberAttribute.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberAttribute.h:1)
- [src/ast/types/typeMemberAttribute.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberAttribute.cpp:1)

```cpp
class TypeMemberAttribute : public ASTnode {
private:
    std::string name;
    std::string typeAnnotation;
    std::unique_ptr<ASTnode> initializer;

public:
    TypeMemberAttribute(const std::string& name,
                        std::unique_ptr<ASTnode> initializer);
    TypeMemberAttribute(const std::string& name,
                        const std::string& typeAnnotation,
                        std::unique_ptr<ASTnode> initializer);

    const std::string& GetName() const;
    const std::string& GetTypeAnnotation() const;
    bool HasTypeAnnotation() const;
    ASTnode* GetInitializer() const;
    std::string ToString() const override;
};
```

```cpp
std::string TypeMemberAttribute::ToString() const {
    std::string result = name;
    if (!typeAnnotation.empty())
        result += " : " + typeAnnotation;
    result += " = " + initializer->ToString();
    return result;
}
```

### `TypeMemberMethod`

Archivos:

- [src/ast/types/typeMemberMethod.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberMethod.h:1)
- [src/ast/types/typeMemberMethod.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberMethod.cpp:1)

```cpp
class TypeMemberMethod : public ASTnode {
private:
    std::string name;
    std::vector<Param> params;
    std::string returnTypeAnnotation;
    std::unique_ptr<ASTnode> body;

public:
    TypeMemberMethod(const std::string& name, std::vector<Param> params,
                     std::unique_ptr<ASTnode> body);
    TypeMemberMethod(const std::string& name, std::vector<Param> params,
                     const std::string& returnTypeAnnotation,
                     std::unique_ptr<ASTnode> body);

    const std::string& GetName() const;
    const std::vector<Param>& GetParams() const;
    const std::string& GetReturnTypeAnnotation() const;
    bool HasReturnTypeAnnotation() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

```cpp
std::string TypeMemberMethod::ToString() const {
    std::string result = name + "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) result += ", ";
        result += params[i].name;
        if (params[i].HasTypeAnnotation())
            result += " : " + params[i].typeAnnotation;
    }
    result += ")";
    if (!returnTypeAnnotation.empty())
        result += " : " + returnTypeAnnotation;
    result += " => " + body->ToString();
    return result;
}
```

### `TypeMember`

Archivo:

- [src/ast/types/typeDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeDecl.h:14)

`TypeMember` es auxiliar, no un nodo AST independiente.

```cpp
struct TypeMember {
    enum class Kind { Attribute, Method };
    Kind kind;
    std::unique_ptr<ASTnode> node;

    TypeMember(Kind kind, std::unique_ptr<ASTnode> node)
        : kind(kind), node(std::move(node)) {}
};
```

### `TypeDecl`

Archivos:

- [src/ast/types/typeDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeDecl.h:1)
- [src/ast/types/typeDecl.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/typeDecl.cpp:1)

```cpp
class TypeDecl : public ASTnode {
private:
    std::string name;
    std::vector<Param> ctorParams;
    std::string parentName;
    std::vector<std::unique_ptr<ASTnode>> parentArgs;
    std::vector<TypeMember> members;

public:
    TypeDecl(const std::string& name, std::vector<TypeMember> members);
    TypeDecl(const std::string& name, std::vector<Param> ctorParams,
             std::vector<TypeMember> members);
    TypeDecl(const std::string& name, const std::string& parentName,
             std::vector<TypeMember> members);
    TypeDecl(const std::string& name, std::vector<Param> ctorParams,
             const std::string& parentName,
             std::vector<std::unique_ptr<ASTnode>> parentArgs,
             std::vector<TypeMember> members);

    const std::string& GetName() const;
    const std::vector<Param>& GetCtorParams() const;
    bool HasCtorParams() const;
    const std::string& GetParentName() const;
    bool HasParent() const;
    const std::vector<std::unique_ptr<ASTnode>>& GetParentArgs() const;
    const std::vector<TypeMember>& GetMembers() const;
    std::string ToString() const override;
};
```

```cpp
std::string TypeDecl::ToString() const {
    std::string result = "type " + name;
    if (!ctorParams.empty()) {
        result += "(";
        for (size_t i = 0; i < ctorParams.size(); ++i) {
            if (i > 0) result += ", ";
            result += ctorParams[i].name;
            if (ctorParams[i].HasTypeAnnotation())
                result += " : " + ctorParams[i].typeAnnotation;
        }
        result += ")";
    }
    if (!parentName.empty()) {
        result += " inherits " + parentName;
        if (!parentArgs.empty()) {
            result += "(";
            for (size_t i = 0; i < parentArgs.size(); ++i) {
                if (i > 0) result += ", ";
                result += parentArgs[i]->ToString();
            }
            result += ")";
        }
    }
    result += " { ... }";
    return result;
}
```

## Protocolos

### `ProtocolMethodSig`

Archivo:

- [src/ast/protocols/protocolMethodSig.h](/home/jean/School/Compilacion/Hulk/src/ast/protocols/protocolMethodSig.h:1)

`ProtocolMethodSig` es auxiliar, no un nodo AST independiente.

```cpp
struct ProtocolMethodSig {
    std::string name;
    std::vector<Param> params;
    std::string returnType;

    ProtocolMethodSig(const std::string& name,
                      std::vector<Param> params,
                      const std::string& returnType)
        : name(name), params(std::move(params)), returnType(returnType) {}
};
```

### `ProtocolDecl`

Archivos:

- [src/ast/protocols/protocolDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/protocols/protocolDecl.h:1)
- [src/ast/protocols/protocolDecl.cpp](/home/jean/School/Compilacion/Hulk/src/ast/protocols/protocolDecl.cpp:1)

```cpp
class ProtocolDecl : public ASTnode {
private:
    std::string name;
    std::string parentName;
    std::vector<ProtocolMethodSig> methodSigs;

public:
    ProtocolDecl(const std::string& name,
                 std::vector<ProtocolMethodSig> methodSigs);
    ProtocolDecl(const std::string& name, const std::string& parentName,
                 std::vector<ProtocolMethodSig> methodSigs);

    const std::string& GetName() const;
    const std::string& GetParentName() const;
    bool HasParent() const;
    const std::vector<ProtocolMethodSig>& GetMethodSigs() const;
    std::string ToString() const override;
};
```

```cpp
std::string ProtocolDecl::ToString() const {
    std::string result = "protocol " + name;
    if (!parentName.empty())
        result += " extends " + parentName;
    result += " { ";
    for (const auto& sig : methodSigs) {
        result += sig.name + "(";
        for (size_t i = 0; i < sig.params.size(); ++i) {
            if (i > 0) result += ", ";
            result += sig.params[i].name;
            if (sig.params[i].HasTypeAnnotation())
                result += " : " + sig.params[i].typeAnnotation;
        }
        result += ") : " + sig.returnType + "; ";
    }
    result += "}";
    return result;
}
```

## Otros nodos

### `Program`

Archivos:

- [src/ast/others/program.h](/home/jean/School/Compilacion/Hulk/src/ast/others/program.h:1)
- [src/ast/others/program.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/program.cpp:1)

```cpp
class Program : public ASTnode {
private:
    std::vector<std::unique_ptr<ASTnode>> declarations;
    std::unique_ptr<ASTnode> globalExpr;

public:
    Program(std::vector<std::unique_ptr<ASTnode>> declarations,
            std::unique_ptr<ASTnode> globalExpr);

    const std::vector<std::unique_ptr<ASTnode>>& GetDeclarations() const;
    ASTnode* GetGlobalExpr() const;
    std::string ToString() const override;
};
```

```cpp
std::string Program::ToString() const {
    std::string result;
    for (const auto& decl : declarations)
        result += decl->ToString() + "\n";
    result += globalExpr->ToString();
    return result;
}
```

### `ExprBlock`

Archivos:

- [src/ast/others/exprBlock.h](/home/jean/School/Compilacion/Hulk/src/ast/others/exprBlock.h:1)
- [src/ast/others/exprBlock.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/exprBlock.cpp:1)

```cpp
class ExprBlock : public ASTnode {
private:
    std::vector<std::unique_ptr<ASTnode>> exprs;

public:
    explicit ExprBlock(std::vector<std::unique_ptr<ASTnode>> exprs);
    const std::vector<std::unique_ptr<ASTnode>>& GetExprs() const;
    ASTnode* GetLast() const;
    std::string ToString() const override;
};
```

```cpp
std::string ExprBlock::ToString() const {
    std::string result = "{ ";
    for (const auto& e : exprs)
        result += e->ToString() + "; ";
    result += "}";
    return result;
}
```

### `Group`

Archivos:

- [src/ast/others/group.h](/home/jean/School/Compilacion/Hulk/src/ast/others/group.h:1)
- [src/ast/others/group.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/group.cpp:1)

```cpp
class Group : public ASTnode {
private:
    std::unique_ptr<ASTnode> expr;

public:
    explicit Group(std::unique_ptr<ASTnode> expression);
    ASTnode* GetExpr() const;
    std::string ToString() const override;
};
```

```cpp
std::string Group::ToString() const {
    return "(" + (expr ? expr->ToString() : "null") + ")";
}
```

### `SelfRef`

Archivos:

- [src/ast/others/selfRef.h](/home/jean/School/Compilacion/Hulk/src/ast/others/selfRef.h:1)
- [src/ast/others/selfRef.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/selfRef.cpp:1)

```cpp
class SelfRef : public ASTnode {
public:
    SelfRef() = default;
    std::string ToString() const override;
};
```

```cpp
std::string SelfRef::ToString() const {
    return "self";
}
```

### `BaseCall`

Archivos:

- [src/ast/others/baseCall.h](/home/jean/School/Compilacion/Hulk/src/ast/others/baseCall.h:1)
- [src/ast/others/baseCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/baseCall.cpp:1)

```cpp
class BaseCall : public ASTnode {
private:
    std::vector<std::unique_ptr<ASTnode>> args;

public:
    explicit BaseCall(std::vector<std::unique_ptr<ASTnode>> args);
    const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;
    std::string ToString() const override;
};
```

```cpp
std::string BaseCall::ToString() const {
    std::string result = "base(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i]->ToString();
    }
    result += ")";
    return result;
}
```

## Vectores

### `VectorLiteral`

Archivos:

- [src/ast/vectors/vectorLiteral.h](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorLiteral.h:1)
- [src/ast/vectors/vectorLiteral.cpp](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorLiteral.cpp:1)

```cpp
class VectorLiteral : public ASTnode {
private:
    std::vector<std::unique_ptr<ASTnode>> elements;

public:
    explicit VectorLiteral(std::vector<std::unique_ptr<ASTnode>> elements);
    const std::vector<std::unique_ptr<ASTnode>>& GetElements() const;
    std::string ToString() const override;
};
```

```cpp
std::string VectorLiteral::ToString() const {
    std::string result = "[";
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) result += ", ";
        result += elements[i]->ToString();
    }
    result += "]";
    return result;
}
```

### `VectorIndex`

Archivos:

- [src/ast/vectors/vectorIndex.h](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorIndex.h:1)
- [src/ast/vectors/vectorIndex.cpp](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorIndex.cpp:1)

```cpp
class VectorIndex : public ASTnode {
private:
    std::unique_ptr<ASTnode> vector;
    std::unique_ptr<ASTnode> index;

public:
    VectorIndex(std::unique_ptr<ASTnode> vector,
                std::unique_ptr<ASTnode> index);

    ASTnode* GetVector() const;
    ASTnode* GetIndex() const;
    std::string ToString() const override;
};
```

```cpp
std::string VectorIndex::ToString() const {
    return vector->ToString() + "[" + index->ToString() + "]";
}
```

### `VectorGenerator`

Archivos:

- [src/ast/vectors/vectorGenerator.h](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorGenerator.h:1)
- [src/ast/vectors/vectorGenerator.cpp](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorGenerator.cpp:1)

```cpp
class VectorGenerator : public ASTnode {
private:
    std::unique_ptr<ASTnode> body;
    std::string varName;
    std::unique_ptr<ASTnode> iterable;

public:
    VectorGenerator(std::unique_ptr<ASTnode> body,
                    const std::string& varName,
                    std::unique_ptr<ASTnode> iterable);

    ASTnode* GetBody() const;
    const std::string& GetVarName() const;
    ASTnode* GetIterable() const;
    std::string ToString() const override;
};
```

```cpp
std::string VectorGenerator::ToString() const {
    return "[" + body->ToString() + " | " + varName + " in " + iterable->ToString() + "]";
}
```

## Resumen final

### Nodos AST concretos identificados

- `Number`
- `Boolean`
- `String`
- `VariableReference`
- `VariableBinding`
- `LetIn`
- `DestructiveAssign`
- `DestructiveAssignMember`
- `ArithmeticBinOp`
- `LogicBinOp`
- `StringBinOp`
- `ArithmeticUnaryOp`
- `LogicUnaryOp`
- `IfStmt`
- `WhileStmt`
- `For`
- `FunctionDecl`
- `FunctionCall`
- `Lambda`
- `Print`
- `BuiltinCall`
- `IsExpr`
- `AsExpr`
- `NewExpr`
- `MemberAccess`
- `MethodCall`
- `TypeMemberAttribute`
- `TypeMemberMethod`
- `TypeDecl`
- `ProtocolDecl`
- `Program`
- `ExprBlock`
- `Group`
- `SelfRef`
- `BaseCall`
- `VectorLiteral`
- `VectorIndex`
- `VectorGenerator`

### Estructuras auxiliares del modelo

- `ASTnode`
- `BinOp`
- `UnaryOp`
- `Param`
- `ElifBranch`
- `TypeMember`
- `ProtocolMethodSig`
