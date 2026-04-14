# Informe técnico final del AST

Este documento describe la versión actual del AST del proyecto HULK, tomando como fuente el código de `src/ast/`. La meta de este informe es dejar una referencia técnica final sobre:

- la arquitectura general del árbol;
- las abstracciones base;
- cada nodo concreto implementado;
- las estructuras auxiliares que acompañan al AST;
- la forma textual que usa cada nodo mediante `ToString()`;
- observaciones de diseño útiles para parser, chequeo semántico y generación posterior.

## 1. Vista general

El AST del proyecto está organizado por dominios sintácticos y semánticos:

- `abs_nodes`: jerarquía base del árbol;
- `literales`: nodos de constantes;
- `variables`: referencias, bindings y `let`;
- `assignments`: asignación destructiva;
- `binOps`: operaciones binarias;
- `unaryOps`: operaciones unarias;
- `conditionals`: condicionales;
- `loops`: estructuras iterativas;
- `functions`: funciones, llamadas y lambdas;
- `domainFunctions`: builtins modelados como nodos;
- `types`: tipos, miembros, acceso, `new`, `is`, `as`;
- `protocols`: declaraciones de protocolo;
- `others`: nodos estructurales o de soporte;
- `vectors`: construcciones de vectores.

La jerarquía está basada en polimorfismo clásico con una clase abstracta raíz y uso extensivo de `std::unique_ptr<ASTnode>` para modelar propiedad exclusiva de los hijos.

## 2. Principios de diseño observados

### 2.1 Nodo base único

Todo nodo del árbol deriva de `ASTnode`, que define una interfaz mínima:

- destructor virtual;
- `ToString()` puro.

Esto permite:

- almacenamiento heterogéneo por punteros base;
- recorrido polimórfico;
- serialización textual simple del árbol.

### 2.2 Ownership explícito

La mayoría de los hijos del AST se almacenan como `std::unique_ptr<ASTnode>`, lo que indica:

- propiedad exclusiva del nodo padre sobre sus hijos;
- ausencia de compartición implícita de subárboles;
- destrucción recursiva segura al liberar la raíz.

### 2.3 Representación textual integrada

Todos los nodos concretos implementan `ToString()`. Esto sugiere que el AST actual ya está pensado para:

- depuración;
- pruebas estructurales;
- inspección manual del parseo.

No es todavía un visitor formal, pero sí un mecanismo uniforme de introspección.

### 2.4 Estructuras auxiliares fuera de la jerarquía

Algunos elementos sintácticos no son nodos AST, sino structs auxiliares:

- `Param`;
- `ElifBranch`;
- `TypeMember`;
- `ProtocolMethodSig`.

Esto es una decisión razonable cuando el elemento:

- no necesita polimorfismo propio;
- vive contenido en otro nodo;
- representa metadata o una subestructura cerrada.

## 3. Jerarquía base

### 3.1 `ASTnode`

Archivo: [src/ast/abs_nodes/ast.h](/home/jean/School/Compilacion/Hulk/src/ast/abs_nodes/ast.h:1)

```cpp
class ASTnode {
public:
    virtual ~ASTnode() = default;
    virtual std::string ToString() const = 0;
};
```

Rol:

- raíz abstracta de todo el árbol;
- contrato mínimo para cualquier fase que consuma el AST.

### 3.2 `BinOp`

Archivo: [src/ast/abs_nodes/binOp.h](/home/jean/School/Compilacion/Hulk/src/ast/abs_nodes/binOp.h:1)

```cpp
class BinOp : public ASTnode {
protected:
    std::unique_ptr<ASTnode> left;
    std::unique_ptr<ASTnode> right;

public:
    BinOp(std::unique_ptr<ASTnode> leftNode, std::unique_ptr<ASTnode> rightNode)
        : left(std::move(leftNode)), right(std::move(rightNode)) {}

    ASTnode* GetLeft() const { return left.get(); }
    ASTnode* GetRight() const { return right.get(); }
};
```

Rol:

- factoriza la estructura común de operaciones binarias;
- reduce repetición en operadores aritméticos, lógicos y de strings.

### 3.3 `UnaryOp`

Archivo: [src/ast/abs_nodes/unaryOp.h](/home/jean/School/Compilacion/Hulk/src/ast/abs_nodes/unaryOp.h:1)

```cpp
class UnaryOp : public ASTnode {
protected:
    std::unique_ptr<ASTnode> operand;

public:
    explicit UnaryOp(std::unique_ptr<ASTnode> arg)
        : operand(std::move(arg)) {}

    ASTnode* GetOperand() const {
        return operand.get();
    }
};
```

Rol:

- factoriza la estructura de operadores unarios;
- centraliza el manejo del operando único.

## 4. Inventario técnico de nodos

### 4.1 Literales

#### `Number`

Archivos:

- [src/ast/literales/number.h](/home/jean/School/Compilacion/Hulk/src/ast/literales/number.h:1)
- [src/ast/literales/number.cpp](/home/jean/School/Compilacion/Hulk/src/ast/literales/number.cpp:1)

Payload:

- `double value`

Forma textual:

```cpp
return "NumberLiteral: " + std::to_string(value);
```

Código:

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

#### `Boolean`

Archivos:

- [src/ast/literales/boolean.h](/home/jean/School/Compilacion/Hulk/src/ast/literales/boolean.h:1)
- [src/ast/literales/boolean.cpp](/home/jean/School/Compilacion/Hulk/src/ast/literales/boolean.cpp:1)

Payload:

- `bool value`

Forma textual:

```cpp
return "BooleanLiteral: " + std::string(value ? "true" : "false");
```

Código:

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

#### `String`

Archivos:

- [src/ast/literales/string.h](/home/jean/School/Compilacion/Hulk/src/ast/literales/string.h:1)
- [src/ast/literales/string.cpp](/home/jean/School/Compilacion/Hulk/src/ast/literales/string.cpp:1)

Payload:

- `std::string value`

Forma textual:

```cpp
return "StringLiteral: " + value;
```

Código:

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

### 4.2 Variables y bindings

#### `VariableReference`

Archivos:

- [src/ast/variables/variableReference.h](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableReference.h:1)
- [src/ast/variables/variableReference.cpp](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableReference.cpp:1)

Payload:

- `std::string name`

Forma textual:

```cpp
return "Variable(" + name + ")";
```

Código:

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

#### `VariableBinding`

Archivos:

- [src/ast/variables/variableBinding.h](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableBinding.h:1)
- [src/ast/variables/variableBinding.cpp](/home/jean/School/Compilacion/Hulk/src/ast/variables/variableBinding.cpp:1)

Payload:

- `std::string name`
- `std::string typeAnnotation`
- `std::unique_ptr<ASTnode> initializer`

Forma textual:

```cpp
name [: type] = initializer
```

Código:

```cpp
class VariableBinding : public ASTnode {
private:
    std::string name;
    std::string typeAnnotation;
    std::unique_ptr<ASTnode> initializer;

public:
    VariableBinding(const std::string& name,
                    std::unique_ptr<ASTnode> initializer);

    VariableBinding(const std::string& name,
                    const std::string& typeAnnotation,
                    std::unique_ptr<ASTnode> initializer);

    const std::string& GetName() const;
    const std::string& GetTypeAnnotation() const;
    bool HasTypeAnnotation() const;
    ASTnode* GetInitializer() const;
    std::string ToString() const override;
};
```

#### `LetIn`

Archivos:

- [src/ast/variables/letIn.h](/home/jean/School/Compilacion/Hulk/src/ast/variables/letIn.h:1)
- [src/ast/variables/letIn.cpp](/home/jean/School/Compilacion/Hulk/src/ast/variables/letIn.cpp:1)

Payload:

- `std::vector<std::unique_ptr<VariableBinding>> bindings`
- `std::unique_ptr<ASTnode> body`

Forma textual:

```cpp
let b1, b2, ... in body
```

Código:

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

### 4.3 Asignaciones

#### `DestructiveAssign`

Archivos:

- [src/ast/assignments/desctructiveAssign.h](/home/jean/School/Compilacion/Hulk/src/ast/assignments/desctructiveAssign.h:1)
- [src/ast/assignments/desctructiveAssign.cpp](/home/jean/School/Compilacion/Hulk/src/ast/assignments/desctructiveAssign.cpp:1)

Payload:

- `std::string name`
- `std::unique_ptr<ASTnode> value`

Forma textual:

```cpp
name := value
```

Código:

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

#### `DestructiveAssignMember`

Archivos:

- [src/ast/assignments/destructiveAssignMember.h](/home/jean/School/Compilacion/Hulk/src/ast/assignments/destructiveAssignMember.h:1)
- [src/ast/assignments/destructiveAssignMember.cpp](/home/jean/School/Compilacion/Hulk/src/ast/assignments/destructiveAssignMember.cpp:1)

Payload:

- `std::unique_ptr<ASTnode> object`
- `std::string memberName`
- `std::unique_ptr<ASTnode> value`

Forma textual:

```cpp
object.member := value
```

Código:

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

### 4.4 Operaciones binarias

#### `ArithmeticOp` / `ArithmeticBinOp`

Archivos:

- [src/ast/binOps/arithmeticBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/arithmeticBinOp.h:1)
- [src/ast/binOps/arithmeticBinOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/binOps/arithmeticBinOp.cpp:1)

Operadores:

- `Plus`
- `Minus`
- `Mult`
- `Div`
- `Pow`

Payload:

- `left`
- `right`
- `ArithmeticOp op`

Forma textual:

```cpp
(left op right)
```

Código:

```cpp
enum class ArithmeticOp { Plus, Minus, Mult, Div, Pow };

class ArithmeticBinOp : public BinOp {
private:
    ArithmeticOp op;

public:
    ArithmeticBinOp(std::unique_ptr<ASTnode> left,
                    ArithmeticOp operation,
                    std::unique_ptr<ASTnode> right);

    ArithmeticOp GetOperator() const { return op; }
    std::string ToString() const override;
};
```

#### `LogicOp` / `LogicBinOp`

Archivos:

- [src/ast/binOps/logicBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/logicBinOp.h:1)
- [src/ast/binOps/logicBinOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/binOps/logicBinOp.cpp:1)

Operadores:

- `And`
- `Or`
- `Greater`
- `Less`
- `Equal`
- `NotEqual`
- `GreaterEqual`
- `LessEqual`

Payload:

- `left`
- `right`
- `LogicOp op`

Código:

```cpp
enum class LogicOp {
    And, Or, Greater, Less, Equal, NotEqual, GreaterEqual, LessEqual
};

class LogicBinOp : public BinOp {
private:
    LogicOp op;

public:
    LogicBinOp(std::unique_ptr<ASTnode> left,
               LogicOp operation,
               std::unique_ptr<ASTnode> right);

    LogicOp GetOperator() const { return op; }
    std::string ToString() const override;
};
```

#### `StringOp` / `StringBinOp`

Archivos:

- [src/ast/binOps/stringBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/stringBinOp.h:1)
- [src/ast/binOps/stringBinOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/binOps/stringBinOp.cpp:1)

Operadores:

- `Concat`
- `SpaceConcat`

Payload:

- `left`
- `right`
- `StringOp op`

Código:

```cpp
enum class StringOp { Concat, SpaceConcat };

class StringBinOp : public BinOp {
private:
    StringOp op;

public:
    StringBinOp(std::unique_ptr<ASTnode> left,
                StringOp operation,
                std::unique_ptr<ASTnode> right);

    StringOp GetOperator() const;
    std::string ToString() const override;
};
```

Observación:

- este nodo ya contempla `@@` mediante `StringOp::SpaceConcat`, aunque el lexer actual no expone todavía ese token como parte de su contrato.

### 4.5 Operaciones unarias

#### `ArithUnaryType` / `ArithmeticUnaryOp`

Archivos:

- [src/ast/unaryOps/arithmeticUnaryOp.h](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/arithmeticUnaryOp.h:1)
- [src/ast/unaryOps/arithmeticUnaryOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/arithmeticUnaryOp.cpp:1)

Operadores:

- `Minus`
- `Sin`
- `Cos`
- `Sqrt`

Payload:

- `operand`
- `ArithUnaryType type`

Código:

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

#### `LogicUnaryOp`

Archivos:

- [src/ast/unaryOps/logicUnaryOp.h](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/logicUnaryOp.h:1)
- [src/ast/unaryOps/logicUnaryOp.cpp](/home/jean/School/Compilacion/Hulk/src/ast/unaryOps/logicUnaryOp.cpp:1)

Payload:

- `operand`

Forma textual:

```cpp
!(operand)
```

Código:

```cpp
class LogicUnaryOp : public UnaryOp {
public:
    explicit LogicUnaryOp(std::unique_ptr<ASTnode> arg);
    std::string ToString() const override;
};
```

### 4.6 Condicionales

#### `ElifBranch`

Archivo:

- [src/ast/conditionals/elifStmt.h](/home/jean/School/Compilacion/Hulk/src/ast/conditionals/elifStmt.h:1)

Rol:

- auxiliar estructural para las ramas `elif`;
- no es un nodo AST independiente.

Código:

```cpp
struct ElifBranch {
    std::unique_ptr<ASTnode> condition;
    std::unique_ptr<ASTnode> body;

    ElifBranch(std::unique_ptr<ASTnode> cond, std::unique_ptr<ASTnode> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};
```

#### `IfStmt`

Archivos:

- [src/ast/conditionals/ifStmt.h](/home/jean/School/Compilacion/Hulk/src/ast/conditionals/ifStmt.h:1)
- [src/ast/conditionals/ifStmt.cpp](/home/jean/School/Compilacion/Hulk/src/ast/conditionals/ifStmt.cpp:1)

Payload:

- `condition`
- `thenBranch`
- `std::vector<ElifBranch> elifBranches`
- `elseBranch`

Código:

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

### 4.7 Bucles

#### `WhileStmt`

Archivos:

- [src/ast/loops/while.h](/home/jean/School/Compilacion/Hulk/src/ast/loops/while.h:1)
- [src/ast/loops/while.cpp](/home/jean/School/Compilacion/Hulk/src/ast/loops/while.cpp:1)

Payload:

- `condition`
- `body`

Código:

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

#### `For`

Archivos:

- [src/ast/loops/for.h](/home/jean/School/Compilacion/Hulk/src/ast/loops/for.h:1)
- [src/ast/loops/for.cpp](/home/jean/School/Compilacion/Hulk/src/ast/loops/for.cpp:1)

Payload:

- `std::string varName`
- `iterable`
- `body`

Código:

```cpp
class For : public ASTnode {
private:
    std::string varName;
    std::unique_ptr<ASTnode> iterable;
    std::unique_ptr<ASTnode> body;

public:
    For(const std::string& varName,
        std::unique_ptr<ASTnode> iterable,
        std::unique_ptr<ASTnode> body);

    const std::string& GetVarName() const;
    ASTnode* GetIterable() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

### 4.8 Funciones y llamadas

#### `Param`

Archivo:

- [src/ast/functions/param.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/param.h:1)

Rol:

- auxiliar para parámetros;
- no forma parte de la jerarquía AST.

Código:

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

#### `FunctionDecl`

Archivos:

- [src/ast/functions/functionDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionDecl.h:1)
- [src/ast/functions/functionDecl.cpp](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionDecl.cpp:1)

Payload:

- `std::string name`
- `std::vector<Param> params`
- `std::string returnTypeAnnotation`
- `std::unique_ptr<ASTnode> body`

Código:

```cpp
class FunctionDecl : public ASTnode {
private:
    std::string name;
    std::vector<Param> params;
    std::string returnTypeAnnotation;
    std::unique_ptr<ASTnode> body;

public:
    FunctionDecl(const std::string& name,
                 std::vector<Param> params,
                 std::unique_ptr<ASTnode> body);

    FunctionDecl(const std::string& name,
                 std::vector<Param> params,
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

#### `FunctionCall`

Archivos:

- [src/ast/functions/functionCall.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionCall.h:1)
- [src/ast/functions/functionCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/functions/functionCall.cpp:1)

Payload:

- `std::string name`
- `std::vector<std::unique_ptr<ASTnode>> args`

Código:

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

#### `Lambda`

Archivos:

- [src/ast/functions/lambda.h](/home/jean/School/Compilacion/Hulk/src/ast/functions/lambda.h:1)
- [src/ast/functions/lambda.cpp](/home/jean/School/Compilacion/Hulk/src/ast/functions/lambda.cpp:1)

Payload:

- `std::vector<Param> params`
- `std::string returnTypeAnnotation`
- `std::unique_ptr<ASTnode> body`

Código:

```cpp
class Lambda : public ASTnode {
private:
    std::vector<Param> params;
    std::string returnTypeAnnotation;
    std::unique_ptr<ASTnode> body;

public:
    Lambda(std::vector<Param> params,
           std::unique_ptr<ASTnode> body);

    Lambda(std::vector<Param> params,
           const std::string& returnTypeAnnotation,
           std::unique_ptr<ASTnode> body);

    const std::vector<Param>& GetParams() const;
    const std::string& GetReturnTypeAnnotation() const;
    bool HasReturnTypeAnnotation() const;
    ASTnode* GetBody() const;
    std::string ToString() const override;
};
```

### 4.9 Builtins del dominio

#### `Print`

Archivos:

- [src/ast/domainFunctions/print.h](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/print.h:1)
- [src/ast/domainFunctions/print.cpp](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/print.cpp:1)

Payload:

- `std::unique_ptr<ASTnode> expr`

Código:

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

#### `BuiltinFunc` / `BuiltinCall`

Archivos:

- [src/ast/domainFunctions/builtinCall.h](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/builtinCall.h:1)
- [src/ast/domainFunctions/builtinCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/domainFunctions/builtinCall.cpp:1)

Builtins modeladas:

- `Exp`
- `Log`
- `Rand`
- `Range`

Payload:

- `BuiltinFunc func`
- `std::vector<std::unique_ptr<ASTnode>> args`

Código:

```cpp
enum class BuiltinFunc {
    Exp,
    Log,
    Rand,
    Range
};

class BuiltinCall : public ASTnode {
private:
    BuiltinFunc func;
    std::vector<std::unique_ptr<ASTnode>> args;

public:
    BuiltinCall(BuiltinFunc func,
                std::vector<std::unique_ptr<ASTnode>> args);

    BuiltinFunc GetFunc() const;
    const std::vector<std::unique_ptr<ASTnode>>& GetArgs() const;
    std::string ToString() const override;
};
```

### 4.10 Tipos y OO

#### `IsExpr`

Archivos:

- [src/ast/types/isExpr.h](/home/jean/School/Compilacion/Hulk/src/ast/types/isExpr.h:1)
- [src/ast/types/isExpr.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/isExpr.cpp:1)

Payload:

- `expr`
- `std::string typeName`

Código:

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

#### `AsExpr`

Archivos:

- [src/ast/types/asExpr.h](/home/jean/School/Compilacion/Hulk/src/ast/types/asExpr.h:1)
- [src/ast/types/asExpr.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/asExpr.cpp:1)

Payload:

- `expr`
- `std::string typeName`

Código:

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

#### `NewExpr`

Archivos:

- [src/ast/types/newExpr.h](/home/jean/School/Compilacion/Hulk/src/ast/types/newExpr.h:1)
- [src/ast/types/newExpr.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/newExpr.cpp:1)

Payload:

- `std::string typeName`
- `std::vector<std::unique_ptr<ASTnode>> args`

Código:

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

#### `MemberAccess`

Archivos:

- [src/ast/types/memberAccess.h](/home/jean/School/Compilacion/Hulk/src/ast/types/memberAccess.h:1)
- [src/ast/types/memberAccess.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/memberAccess.cpp:1)

Payload:

- `object`
- `std::string memberName`

Código:

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

#### `MethodCall`

Archivos:

- [src/ast/types/methodCall.h](/home/jean/School/Compilacion/Hulk/src/ast/types/methodCall.h:1)
- [src/ast/types/methodCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/methodCall.cpp:1)

Payload:

- `object`
- `std::string methodName`
- `std::vector<std::unique_ptr<ASTnode>> args`

Código:

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

#### `TypeMemberAttribute`

Archivos:

- [src/ast/types/typeMemberAttribute.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberAttribute.h:1)
- [src/ast/types/typeMemberAttribute.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberAttribute.cpp:1)

Payload:

- `std::string name`
- `std::string typeAnnotation`
- `initializer`

Código:

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

#### `TypeMemberMethod`

Archivos:

- [src/ast/types/typeMemberMethod.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberMethod.h:1)
- [src/ast/types/typeMemberMethod.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/typeMemberMethod.cpp:1)

Payload:

- `std::string name`
- `std::vector<Param> params`
- `std::string returnTypeAnnotation`
- `body`

Código:

```cpp
class TypeMemberMethod : public ASTnode {
private:
    std::string name;
    std::vector<Param> params;
    std::string returnTypeAnnotation;
    std::unique_ptr<ASTnode> body;

public:
    TypeMemberMethod(const std::string& name,
                     std::vector<Param> params,
                     std::unique_ptr<ASTnode> body);

    TypeMemberMethod(const std::string& name,
                     std::vector<Param> params,
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

#### `TypeMember`

Archivo:

- [src/ast/types/typeDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeDecl.h:14)

Rol:

- wrapper auxiliar para distinguir miembros atributo y método;
- no es un nodo AST autónomo.

Código:

```cpp
struct TypeMember {
    enum class Kind { Attribute, Method };
    Kind kind;
    std::unique_ptr<ASTnode> node;

    TypeMember(Kind kind, std::unique_ptr<ASTnode> node)
        : kind(kind), node(std::move(node)) {}
};
```

#### `TypeDecl`

Archivos:

- [src/ast/types/typeDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/types/typeDecl.h:1)
- [src/ast/types/typeDecl.cpp](/home/jean/School/Compilacion/Hulk/src/ast/types/typeDecl.cpp:1)

Payload:

- `std::string name`
- `std::vector<Param> ctorParams`
- `std::string parentName`
- `std::vector<std::unique_ptr<ASTnode>> parentArgs`
- `std::vector<TypeMember> members`

Código:

```cpp
class TypeDecl : public ASTnode {
private:
    std::string name;
    std::vector<Param> ctorParams;
    std::string parentName;
    std::vector<std::unique_ptr<ASTnode>> parentArgs;
    std::vector<TypeMember> members;

public:
    TypeDecl(const std::string& name,
             std::vector<TypeMember> members);

    TypeDecl(const std::string& name,
             std::vector<Param> ctorParams,
             std::vector<TypeMember> members);

    TypeDecl(const std::string& name,
             const std::string& parentName,
             std::vector<TypeMember> members);

    TypeDecl(const std::string& name,
             std::vector<Param> ctorParams,
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

### 4.11 Protocolos

#### `ProtocolMethodSig`

Archivo:

- [src/ast/protocols/protocolMethodSig.h](/home/jean/School/Compilacion/Hulk/src/ast/protocols/protocolMethodSig.h:1)

Rol:

- estructura auxiliar para firmas de métodos en protocolos;
- no es nodo AST independiente.

Código:

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

#### `ProtocolDecl`

Archivos:

- [src/ast/protocols/protocolDecl.h](/home/jean/School/Compilacion/Hulk/src/ast/protocols/protocolDecl.h:1)
- [src/ast/protocols/protocolDecl.cpp](/home/jean/School/Compilacion/Hulk/src/ast/protocols/protocolDecl.cpp:1)

Payload:

- `std::string name`
- `std::string parentName`
- `std::vector<ProtocolMethodSig> methodSigs`

Código:

```cpp
class ProtocolDecl : public ASTnode {
private:
    std::string name;
    std::string parentName;
    std::vector<ProtocolMethodSig> methodSigs;

public:
    ProtocolDecl(const std::string& name,
                 std::vector<ProtocolMethodSig> methodSigs);

    ProtocolDecl(const std::string& name,
                 const std::string& parentName,
                 std::vector<ProtocolMethodSig> methodSigs);

    const std::string& GetName() const;
    const std::string& GetParentName() const;
    bool HasParent() const;
    const std::vector<ProtocolMethodSig>& GetMethodSigs() const;
    std::string ToString() const override;
};
```

### 4.12 Nodos estructurales y de soporte

#### `Program`

Archivos:

- [src/ast/others/program.h](/home/jean/School/Compilacion/Hulk/src/ast/others/program.h:1)
- [src/ast/others/program.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/program.cpp:1)

Payload:

- `std::vector<std::unique_ptr<ASTnode>> declarations`
- `std::unique_ptr<ASTnode> globalExpr`

Código:

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

#### `ExprBlock`

Archivos:

- [src/ast/others/exprBlock.h](/home/jean/School/Compilacion/Hulk/src/ast/others/exprBlock.h:1)
- [src/ast/others/exprBlock.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/exprBlock.cpp:1)

Payload:

- `std::vector<std::unique_ptr<ASTnode>> exprs`

Código:

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

#### `Group`

Archivos:

- [src/ast/others/group.h](/home/jean/School/Compilacion/Hulk/src/ast/others/group.h:1)
- [src/ast/others/group.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/group.cpp:1)

Payload:

- `std::unique_ptr<ASTnode> expr`

Código:

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

#### `SelfRef`

Archivos:

- [src/ast/others/selfRef.h](/home/jean/School/Compilacion/Hulk/src/ast/others/selfRef.h:1)
- [src/ast/others/selfRef.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/selfRef.cpp:1)

Payload:

- no tiene payload adicional.

Código:

```cpp
class SelfRef : public ASTnode {
public:
    SelfRef() = default;
    std::string ToString() const override;
};
```

#### `BaseCall`

Archivos:

- [src/ast/others/baseCall.h](/home/jean/School/Compilacion/Hulk/src/ast/others/baseCall.h:1)
- [src/ast/others/baseCall.cpp](/home/jean/School/Compilacion/Hulk/src/ast/others/baseCall.cpp:1)

Payload:

- `std::vector<std::unique_ptr<ASTnode>> args`

Código:

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

### 4.13 Vectores

#### `VectorLiteral`

Archivos:

- [src/ast/vectors/vectorLiteral.h](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorLiteral.h:1)
- [src/ast/vectors/vectorLiteral.cpp](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorLiteral.cpp:1)

Payload:

- `std::vector<std::unique_ptr<ASTnode>> elements`

Código:

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

#### `VectorIndex`

Archivos:

- [src/ast/vectors/vectorIndex.h](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorIndex.h:1)
- [src/ast/vectors/vectorIndex.cpp](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorIndex.cpp:1)

Payload:

- `vector`
- `index`

Código:

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

#### `VectorGenerator`

Archivos:

- [src/ast/vectors/vectorGenerator.h](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorGenerator.h:1)
- [src/ast/vectors/vectorGenerator.cpp](/home/jean/School/Compilacion/Hulk/src/ast/vectors/vectorGenerator.cpp:1)

Payload:

- `body`
- `std::string varName`
- `iterable`

Código:

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

## 5. Resumen del inventario final

### Nodos concretos implementados

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

### Abstracciones base y auxiliares

- `ASTnode`
- `BinOp`
- `UnaryOp`
- `Param`
- `ElifBranch`
- `TypeMember`
- `ProtocolMethodSig`

## 6. Observaciones técnicas finales

### 6.1 Cobertura del lenguaje

El AST ya cubre una porción importante del lenguaje objetivo:

- expresiones literales;
- variables y `let`;
- operadores aritméticos, lógicos y de strings;
- control de flujo;
- funciones y lambdas;
- tipos, miembros, instanciación y acceso;
- protocolos;
- vectores;
- estructura global del programa.

### 6.2 Consistencia del diseño

El diseño es bastante consistente en:

- uso de `unique_ptr`;
- getters simples;
- encapsulación clara del payload;
- implementación uniforme de `ToString()`.

### 6.3 Punto importante para el parser

El AST ya contempla construcciones que el lexer/parser todavía deben cerrar formalmente, por ejemplo:

- `StringOp::SpaceConcat` para `@@`;
- `ProtocolDecl`;
- `BaseCall`;
- `VectorGenerator`.

Eso significa que el diseño del árbol está más adelantado que parte del contrato léxico y, probablemente, más adelantado que el parser actual.

### 6.4 Limitaciones actuales visibles

A nivel técnico, el AST aún no muestra:

- visitors explícitos;
- anotaciones de tipo semántico dentro de los nodos;
- spans o posiciones embebidas por nodo;
- jerarquías separadas entre expresiones, declaraciones y miembros.

Nada de eso invalida el diseño actual, pero sí marca extensiones naturales para las próximas fases.

## 7. Conclusión

La versión actual del AST de HULK es una base sólida para continuar con:

- la implementación del parser;
- el chequeo semántico;
- la interpretación o generación de código;
- la depuración estructural del lenguaje.

El árbol ya modela con bastante detalle el lenguaje base y varias extensiones, y su diseño es suficientemente claro para servir como contrato interno del frontend del compilador.
