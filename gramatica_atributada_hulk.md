# Gramática Atributada de HULK

> Basada en la especificación oficial: https://matcom.github.io/hulk/appendix-hulk-syntax.html

---

## Guía para Leer esta Gramática

### ¿Qué es una Gramática Atributada?

Una **gramática atributada** es una gramática formal (que define la sintaxis de un lenguaje) extendida con **atributos** y **reglas semánticas**. Sirve para especificar formalmente no solo *qué* programas son sintácticamente válidos, sino también *qué significan* (semántica estática: tipos, alcance, etc.).

Tiene tres componentes:

| Componente | Descripción | Ejemplo |
|------------|-------------|---------|
| **Producción** | Regla sintáctica: cómo se forma una construcción | `expr → expr '+' expr` |
| **Atributos** | Información asociada a cada nodo del árbol | `expr.type`, `expr.val` |
| **Reglas semánticas** | Cómo se calculan los atributos | `expr₀.type↑ = Number` |

---

### Cómo leer una Producción

Una producción tiene la forma:

```
lado_izquierdo → lado_derecho
```

- El **lado izquierdo** es el símbolo que se está definiendo (no-terminal).
- El **lado derecho** es la secuencia de símbolos que lo forman (terminales y no-terminales).
- Las palabras entre comillas simples `'if'`, `'+'`, `';'` son **terminales** (tokens literales).
- Las palabras sin comillas (`expr`, `ID`, `type_ann`) son **no-terminales** (se definen en otras producciones).

**Ejemplo:**
```
expr → 'if' '(' expr_cond ')' expr_then 'else' expr_else
```
Se lee: *"Una expresión puede ser: la palabra `if`, seguida de `(`, seguida de una expresión de condición, seguida de `)`, seguida de una expresión then, seguida de `else`, seguida de una expresión else."*

---

### Cómo leer los Atributos

Cada símbolo en la gramática puede tener **atributos**: datos calculados durante el análisis semántico.

#### Tipos de atributos

**`↑` — Atributo sintetizado**
Se calcula **de hijos a padre**: el nodo lo computa a partir de sus hijos.
```
expr.type↑ = Number
```
Significa: *"el atributo `type` de este nodo `expr` (que sube hacia el padre) vale `Number`"*.

**`↓` — Atributo heredado**
Se pasa **de padre a hijo**: el padre le da información al hijo.
```
expr.env↓ = entorno_actual
```
Significa: *"el padre le pasa al nodo `expr` el entorno actual de variables"*.

#### Subíndices para distinguir repeticiones

Cuando el mismo no-terminal aparece varias veces en una producción, se numeran para distinguirlos:

```
expr₀ → expr₁ '+' expr₂
    expr₀.type↑ = Number
```
- `expr₀` es el **resultado** (el padre, lado izquierdo).
- `expr₁` y `expr₂` son los **operandos** (los hijos, lado derecho).

---

### Cómo leer las Reglas Semánticas

Las reglas semánticas aparecen **indentadas debajo de la producción** y describen cómo se calculan los atributos.

#### Regla de asignación de atributo
```
expr.type↑ = Number
```
*"El tipo de esta expresión (que se propaga hacia arriba) es `Number`."*

#### Regla de propagación de entorno
```
expr_body.env↓ = expr.env↓ ∪ { ID → T }
```
*"Al hijo `expr_body` se le pasa el entorno actual más una nueva entrada: la variable `ID` tiene tipo `T`."*

#### Regla de verificación (`check`)
```
check(conforms(expr₁.type↑, Number), "Tipo no numérico en '+'")
```
*"Verificar que el tipo del operando izquierdo conforma con `Number`. Si no, emitir error semántico: 'Tipo no numérico en +'."*  
Si `check` falla → **error de compilación**.

#### Regla `conforms(T1, T2)`
```
conforms(expr.type↑, Boolean)
```
*"El tipo de esta expresión debe ser `Boolean` o un subtipo de él."* Es la relación `T1 <= T2`.

#### Regla `LCA(T1, T2)`
```
expr.type↑ = LCA(expr_then.type↑, expr_else.type↑)
```
*"El tipo de la expresión completa es el **mínimo ancestro común** de los tipos de las dos ramas."*  
Por ejemplo: si `then` retorna `Dog` y `else` retorna `Cat`, y ambos heredan de `Animal`, el tipo del `if` es `Animal`.

---

### Ejemplo Completo Anotado

Tomemos la producción del `let`:

```
expr → 'let' binding_list 'in' expr_body

    binding_list.env↓ = expr.env↓       -- (1)
    expr_body.env↓    = binding_list.env↑  -- (2)
    expr.type↑        = expr_body.type↑    -- (3)
```

Línea a línea:

| # | Regla | Significado |
|---|-------|-------------|
| (1) | `binding_list.env↓ = expr.env↓` | El entorno actual se **hereda** hacia la lista de bindings |
| (2) | `expr_body.env↓ = binding_list.env↑` | El cuerpo recibe el entorno **extendido** con las nuevas variables declaradas |
| (3) | `expr.type↑ = expr_body.type↑` | El tipo del `let` completo es el tipo del cuerpo (sube hacia el padre) |

**Flujo de información:**
```
        [padre del let]
               ↓ env↓
           expr (let)
          ↓          ↑ type↑
   binding_list    expr_body
    ↓ env↓           ↓ env↓ (= binding_list.env↑)
    (declara vars)   (usa las vars nuevas)
```

---

### Símbolos Especiales Usados

| Símbolo | Significado |
|---------|-------------|
| `→` | "se define como" (producción) |
| `↑` | atributo sintetizado (sube de hijo a padre) |
| `↓` | atributo heredado (baja de padre a hijo) |
| `∪` | unión de entornos/conjuntos |
| `∈` | "pertenece a" (el símbolo está en el entorno) |
| `∉` | "no pertenece a" |
| `\|` | alternativa (en BNF: "o bien") |
| `ε` | producción vacía (no genera nada) |
| `::=` | notación BNF equivalente a `→` |
| `*` | cero o más repeticiones (en BNF) |
| `+` | una o más repeticiones (en BNF) |
| `?` | cero o una vez / opcional (en BNF) |
| `++` | concatenación de listas |
| `{}` | conjunto o entorno vacío |
| `[]` | lista vacía |
| `[i]` | elemento i-ésimo de una lista |
| `\|lista\|` | longitud de una lista |

---

### Flujo General del Compilador con esta Gramática

```
Código fuente
      ↓
[Análisis léxico]  →  tokens (ID, NUMBER, '+', 'if', ...)
      ↓
[Análisis sintáctico]  →  árbol de parsing (árbol de derivación)
      ↓
[Evaluación de atributos heredados ↓]  →  entornos, contexto
      ↓
[Evaluación de atributos sintetizados ↑]  →  tipos inferidos
      ↓
[Ejecución de checks]  →  errores semánticos (tipos, alcance, aridad)
      ↓
Árbol semántico anotado → generación de código
```

---

## Convenciones de Notación

- **Atributos sintetizados** (`↑`): calculados desde los hijos hacia el padre.
- **Atributos heredados** (`↓`): pasados desde el padre hacia los hijos.
- `type(X)` — tipo estático inferido/verificado del nodo X.
- `val(X)` — valor semántico del nodo X.
- `env` — entorno (tabla de símbolos) heredado.
- `env'` — entorno extendido (con nuevos símbolos).
- `conforms(T1, T2)` — `T1 <= T2` (T1 conforma con T2).
- `LCA(T1, T2)` — mínimo ancestro común en la jerarquía de tipos.
- `check(cond, msg)` — error semántico si la condición falla.

---

## 1. Estructura del Programa

```
program → decl_list global_expr

    decl_list.env↓ = env_global_inicial
    global_expr.env↓ = decl_list.env↑

    program.type↑ = global_expr.type↑
```

```
decl_list → ε
    decl_list.env↑ = decl_list.env↓

decl_list → decl_list decl
    decl_list₁.env↓ = decl_list₀.env↓
    decl.env↓ = decl_list₁.env↑
    decl_list₀.env↑ = decl.env↑

decl → function_decl
decl → type_decl
decl → protocol_decl
```

---

## 2. Expresiones

### 2.1 Expresión Global

```
global_expr → expr ';'
    expr.env↓ = global_expr.env↓
    global_expr.type↑ = expr.type↑
```

### 2.2 Bloque de Expresiones

```
expr → expr_block

expr_block → '{' stmt_list '}'
    stmt_list.env↓ = expr_block.env↓
    expr_block.type↑ = stmt_list.last_type↑

stmt_list → expr ';'
    expr.env↓ = stmt_list.env↓
    stmt_list.last_type↑ = expr.type↑

stmt_list → stmt_list expr ';'
    stmt_list₁.env↓ = stmt_list₀.env↓
    expr.env↓ = stmt_list₁.env↓
    stmt_list₀.last_type↑ = expr.type↑
```

### 2.3 Literales

```
expr → NUMBER
    expr.type↑ = Number
    expr.val↑  = valor numérico (32-bit float)

expr → STRING
    expr.type↑ = String
    expr.val↑  = secuencia de caracteres entre comillas dobles

expr → 'true'
    expr.type↑ = Boolean
    expr.val↑  = true

expr → 'false'
    expr.type↑ = Boolean
    expr.val↑  = false
```

### 2.4 Expresiones Aritméticas

```
expr → expr '+' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en '+'")
    check(conforms(expr₂.type↑, Number), "Tipo no numérico en '+'")
    expr₀.type↑ = Number

expr → expr '-' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en '-'")
    check(conforms(expr₂.type↑, Number), "Tipo no numérico en '-'")
    expr₀.type↑ = Number

expr → expr '*' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en '*'")
    check(conforms(expr₂.type↑, Number), "Tipo no numérico en '*'")
    expr₀.type↑ = Number

expr → expr '/' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en '/'")
    check(conforms(expr₂.type↑, Number), "Tipo no numérico en '/'")
    expr₀.type↑ = Number

expr → expr '^' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en '^'")
    check(conforms(expr₂.type↑, Number), "Tipo no numérico en '^'")
    expr₀.type↑ = Number

expr → '-' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en unario '-'")
    expr₀.type↑ = Number

expr → '(' expr ')'
    expr₀.type↑ = expr₁.type↑
```

**Precedencia (mayor a menor):** `^` > `*`, `/` > `+`, `-`  
**Asociatividad:** todos izquierda, excepto `^` que es derecha.

### 2.5 Concatenación de Cadenas

```
expr → expr '@' expr
    check(expr₁.type↑ != null, "Operando inválido en '@'")
    check(expr₂.type↑ != null, "Operando inválido en '@'")
    expr₀.type↑ = String
    -- '@' concatena directamente; '@@' inserta espacio entre operandos

expr → expr '@@' expr
    expr₀.type↑ = String
```

### 2.6 Expresiones Booleanas

```
expr → expr '&' expr
    check(conforms(expr₁.type↑, Boolean), "Tipo no booleano en '&'")
    check(conforms(expr₂.type↑, Boolean), "Tipo no booleano en '&'")
    expr₀.type↑ = Boolean

expr → expr '|' expr
    check(conforms(expr₁.type↑, Boolean), "Tipo no booleano en '|'")
    check(conforms(expr₂.type↑, Boolean), "Tipo no booleano en '|'")
    expr₀.type↑ = Boolean

expr → '!' expr
    check(conforms(expr₁.type↑, Boolean), "Tipo no booleano en '!'")
    expr₀.type↑ = Boolean
```

### 2.7 Comparaciones

```
expr → expr '==' expr
    expr₀.type↑ = Boolean

expr → expr '!=' expr
    expr₀.type↑ = Boolean

expr → expr '<' expr
    check(conforms(expr₁.type↑, Number), "Tipo no numérico en '<'")
    check(conforms(expr₂.type↑, Number), "Tipo no numérico en '<'")
    expr₀.type↑ = Boolean

expr → expr '>' expr
    expr₀.type↑ = Boolean

expr → expr '<=' expr
    expr₀.type↑ = Boolean

expr → expr '>=' expr
    expr₀.type↑ = Boolean
```

### 2.8 Identificadores y Asignación

```
expr → ID
    check(ID ∈ expr.env↓, "Variable no declarada: ID")
    expr.type↑ = expr.env↓[ID].type

expr → ID ':=' expr₁
    check(ID ∈ expr.env↓, "Variable no declarada: ID")
    check(conforms(expr₁.type↑, expr.env↓[ID].type),
          "Tipo incompatible en asignación destructiva")
    expr₀.type↑ = expr₁.type↑
```

---

## 3. Funciones

### 3.1 Declaración de Función

```
function_decl → 'function' ID '(' param_list ')' '=>' expr ';'
    -- Función inline (cuerpo: expresión simple)
    param_list.env↓ = function_decl.env↓
    expr.env↓ = param_list.env↑   -- entorno extendido con parámetros
    function_decl.env↑ = function_decl.env↓ ∪ { ID → FuncType(param_list.types↑, expr.type↑) }

function_decl → 'function' ID '(' param_list ')' expr_block
    -- Función full-form (cuerpo: bloque)
    param_list.env↓ = function_decl.env↓
    expr_block.env↓ = param_list.env↑
    function_decl.env↑ = function_decl.env↓ ∪ { ID → FuncType(param_list.types↑, expr_block.type↑) }

-- Con anotación de tipo de retorno
function_decl → 'function' ID '(' param_list ')' ':' type_ann '=>' expr ';'
    check(conforms(expr.type↑, type_ann.type↑), "Tipo de retorno incorrecto")
    function_decl.env↑ = function_decl.env↓ ∪ { ID → FuncType(param_list.types↑, type_ann.type↑) }
```

### 3.2 Lista de Parámetros

```
param_list → ε
    param_list.env↑  = param_list.env↓
    param_list.types↑ = []

param_list → param
    param.env↓ = param_list.env↓
    param_list.env↑  = param_list.env↓ ∪ { param.name↑ → param.type↑ }
    param_list.types↑ = [ param.type↑ ]

param_list → param_list ',' param
    param_list₁.env↓ = param_list₀.env↓
    param.env↓ = param_list₁.env↑
    param_list₀.env↑  = param.env↑
    param_list₀.types↑ = param_list₁.types↑ ++ [ param.type↑ ]

param → ID
    param.name↑ = ID
    param.type↑ = infer_from_usage(ID)   -- inferencia de tipo

param → ID ':' type_ann
    param.name↑ = ID
    param.type↑ = type_ann.type↑
```

### 3.3 Invocación de Función

```
expr → ID '(' arg_list ')'
    check(ID ∈ expr.env↓, "Función no declarada: ID")
    let FuncType(param_types, ret_type) = expr.env↓[ID]
    check(|arg_list.types↑| == |param_types|, "Aridad incorrecta")
    for i in 0..|param_types|-1:
        check(conforms(arg_list.types↑[i], param_types[i]),
              "Tipo de argumento incorrecto en posición i")
    expr.type↑ = ret_type
```

### 3.4 Lista de Argumentos

```
arg_list → ε
    arg_list.types↑ = []

arg_list → expr
    expr.env↓ = arg_list.env↓
    arg_list.types↑ = [ expr.type↑ ]

arg_list → arg_list ',' expr
    arg_list₁.env↓ = arg_list₀.env↓
    expr.env↓ = arg_list₁.env↓
    arg_list₀.types↑ = arg_list₁.types↑ ++ [ expr.type↑ ]
```

---

## 4. Variables (`let...in`)

```
expr → 'let' binding_list 'in' expr_body

    binding_list.env↓ = expr.env↓
    expr_body.env↓    = binding_list.env↑     -- entorno extendido
    expr.type↑        = expr_body.type↑

binding_list → binding
    binding.env↓ = binding_list.env↓
    binding_list.env↑ = binding_list.env↓ ∪ { binding.name↑ → binding.type↑ }

binding_list → binding_list ',' binding
    binding_list₁.env↓ = binding_list₀.env↓
    binding.env↓ = binding_list₁.env↑        -- cada binding ve los anteriores
    binding_list₀.env↑ = binding.env↑

-- Sin anotación de tipo
binding → ID '=' expr
    expr.env↓ = binding.env↓
    binding.name↑ = ID
    binding.type↑ = expr.type↑               -- inferencia

-- Con anotación de tipo
binding → ID ':' type_ann '=' expr
    expr.env↓ = binding.env↓
    check(conforms(expr.type↑, type_ann.type↑),
          "Tipo de inicialización no conforma con anotación")
    binding.name↑ = ID
    binding.type↑ = type_ann.type↑
```

> **Regla de alcance:** el alcance de cada variable es el cuerpo del `let...in`. Los bindings se evalúan de izquierda a derecha, por lo que un binding puede referirse a variables declaradas antes en la misma lista.

---

## 5. Condicionales

```
expr → 'if' '(' expr_cond ')' expr_then 'else' expr_else
    expr_cond.env↓ = expr.env↓
    expr_then.env↓ = expr.env↓
    expr_else.env↓ = expr.env↓
    check(conforms(expr_cond.type↑, Boolean), "La condición debe ser Boolean")
    expr.type↑ = LCA(expr_then.type↑, expr_else.type↑)

expr → 'if' '(' expr_cond ')' expr_then elif_list 'else' expr_else
    -- Todas las ramas deben ser Boolean en la condición
    check(conforms(expr_cond.type↑, Boolean), "La condición debe ser Boolean")
    expr.type↑ = LCA(expr_then.type↑, LCA(elif_list.type↑, expr_else.type↑))

elif_list → 'elif' '(' expr_cond ')' expr_branch
    check(conforms(expr_cond.type↑, Boolean), "La condición elif debe ser Boolean")
    elif_list.type↑ = expr_branch.type↑

elif_list → elif_list 'elif' '(' expr_cond ')' expr_branch
    check(conforms(expr_cond.type↑, Boolean), "La condición elif debe ser Boolean")
    elif_list₀.type↑ = LCA(elif_list₁.type↑, expr_branch.type↑)
```

> El `if` es una **expresión**: su tipo es el mínimo ancestro común (LCA) de todos los tipos de las ramas.

---

## 6. Ciclos

### 6.1 While

```
expr → 'while' '(' expr_cond ')' expr_body
    expr_cond.env↓ = expr.env↓
    expr_body.env↓ = expr.env↓
    check(conforms(expr_cond.type↑, Boolean), "La condición while debe ser Boolean")
    expr.type↑ = expr_body.type↑
```

### 6.2 For (azúcar sintáctica)

```
expr → 'for' '(' ID 'in' expr_iter ')' expr_body
    expr_iter.env↓ = expr.env↓
    check(conforms(expr_iter.type↑, Iterable),
          "La expresión en 'in' debe conformar con Iterable")
    let elem_type = current_type(expr_iter.type↑)  -- tipo de .current()
    expr_body.env↓ = expr.env↓ ∪ { ID → elem_type }
    expr.type↑ = expr_body.type↑
```

**Desazucarado equivalente:**
```
let iterable = expr_iter in
    while (iterable.next())
        let ID = iterable.current() in
            expr_body
```

---

## 7. Tipos (Clases)

### 7.1 Declaración de Tipo

```
type_decl → 'type' ID '{' member_list '}'
    member_list.self_type↓ = ID
    member_list.env↓ = type_decl.env↓ ∪ { 'self' → ID }
    type_decl.env↑ = type_decl.env↓ ∪ { ID → TypeDef(ID, Object, member_list.members↑) }

type_decl → 'type' ID '(' param_list ')' '{' member_list '}'
    -- Con argumentos de constructor
    param_list.env↓ = type_decl.env↓
    member_list.env↓ = param_list.env↑ ∪ { 'self' → ID }
    member_list.self_type↓ = ID
    type_decl.env↑ = type_decl.env↓ ∪ { ID → TypeDef(ID, Object, param_list.types↑, member_list.members↑) }

type_decl → 'type' ID 'inherits' ID_parent '{' member_list '}'
    check(ID_parent ∈ type_decl.env↓, "Tipo padre no declarado")
    check(ID_parent ∉ {Number, String, Boolean}, "No se puede heredar de tipo builtin")
    member_list.env↓ = type_decl.env↓ ∪ { 'self' → ID, 'base' → ID_parent }
    type_decl.env↑ = type_decl.env↓ ∪ { ID → TypeDef(ID, ID_parent, member_list.members↑) }

type_decl → 'type' ID '(' param_list ')' 'inherits' ID_parent '(' arg_list ')' '{' member_list '}'
    check(ID_parent ∈ type_decl.env↓, "Tipo padre no declarado")
    -- Verificar que arg_list conforma con parámetros de ID_parent
    type_decl.env↑ = type_decl.env↓ ∪ { ID → TypeDef(ID, ID_parent, param_list.types↑, member_list.members↑) }
```

### 7.2 Miembros de Tipo

```
member_list → ε
    member_list.members↑ = {}

member_list → member_list member
    member_list₀.members↑ = member_list₁.members↑ ∪ { member.def↑ }

-- Atributo
member → ID '=' expr ';'
    expr.env↓ = member.env↓
    member.def↑ = Attribute(ID, expr.type↑)
    -- Los atributos son privados (solo accesibles vía self)

member → ID ':' type_ann '=' expr ';'
    check(conforms(expr.type↑, type_ann.type↑), "Tipo de atributo incorrecto")
    member.def↑ = Attribute(ID, type_ann.type↑)

-- Método
member → ID '(' param_list ')' '=>' expr ';'
    param_list.env↓ = member.env↓
    expr.env↓ = param_list.env↑
    member.def↑ = Method(ID, param_list.types↑, expr.type↑)
    -- Los métodos son públicos y virtuales

member → ID '(' param_list ')' ':' type_ann '=>' expr ';'
    check(conforms(expr.type↑, type_ann.type↑), "Tipo de retorno de método incorrecto")
    member.def↑ = Method(ID, param_list.types↑, type_ann.type↑)

member → ID '(' param_list ')' expr_block
    member.def↑ = Method(ID, param_list.types↑, expr_block.type↑)
```

### 7.3 Instanciación

```
expr → 'new' ID '(' arg_list ')'
    check(ID ∈ expr.env↓, "Tipo no declarado: ID")
    let TypeDef(_, _, ctor_types, _) = expr.env↓[ID]
    check(|arg_list.types↑| == |ctor_types|, "Aridad de constructor incorrecta")
    for i: check(conforms(arg_list.types↑[i], ctor_types[i]), "Tipo de arg incorrecto")
    expr.type↑ = ID

expr → 'new' ID '(' ')'
    check(ID ∈ expr.env↓, "Tipo no declarado: ID")
    expr.type↑ = ID
```

### 7.4 Acceso a Miembros

```
expr → expr_obj '.' ID
    check(has_attribute(expr_obj.type↑, ID), "Atributo no existe o es privado")
    expr.type↑ = attribute_type(expr_obj.type↑, ID)

expr → expr_obj '.' ID '(' arg_list ')'
    check(has_method(expr_obj.type↑, ID), "Método no existe")
    let MethodDef(_, param_types, ret_type) = lookup_method(expr_obj.type↑, ID)
    check(|arg_list.types↑| == |param_types|, "Aridad de método incorrecta")
    for i: check(conforms(arg_list.types↑[i], param_types[i]), "Tipo de arg incorrecto")
    expr.type↑ = ret_type
```

> **Regla:** Los atributos son **siempre privados** (solo accesibles via `self` dentro de la clase). Los métodos son **siempre públicos y virtuales**.

---

## 8. Anotaciones de Tipo

```
type_ann → ID
    check(ID ∈ builtin_types ∪ user_types, "Tipo no declarado")
    type_ann.type↑ = ID

type_ann → ID '[]'
    type_ann.type↑ = Vector(ID)

type_ann → ID '*'
    type_ann.type↑ = Iterable(ID)

type_ann → '(' type_list ')' '->' type_ann_ret
    type_ann.type↑ = FuncType(type_list.types↑, type_ann_ret.type↑)

type_list → type_ann
    type_list.types↑ = [ type_ann.type↑ ]

type_list → type_list ',' type_ann
    type_list₀.types↑ = type_list₁.types↑ ++ [ type_ann.type↑ ]
```

---

## 9. Sistema de Tipos — Relación de Conformidad

La relación `T1 <= T2` (T1 **conforma con** T2) satisface:

| Regla | Descripción |
|-------|-------------|
| `T <= T` | Reflexividad |
| `T <= Object` | Todo tipo conforma con Object |
| `T1 inherits T2 ⟹ T1 <= T2` | Herencia directa |
| `T1 <= T2, T2 <= T3 ⟹ T1 <= T3` | Transitividad |
| `Number`, `String`, `Boolean` solo conforman consigo mismos y con `Object` |

**Tipos builtin:**
- `Number` — 32-bit float
- `String` — cadena de caracteres
- `Boolean` — `true` / `false`
- `Object` — raíz de la jerarquía

---

## 10. Inferencia de Tipos

La inferencia ocurre **antes** de la verificación. Reglas clave:

```
infer(NUMBER)       = Number
infer(STRING)       = String
infer(BOOL)         = Boolean
infer(e1 + e2)      = Number   (requiere e1, e2 : Number)
infer(e1 @ e2)      = String
infer(e1 & e2)      = Boolean
infer(if c then e1 else e2) = LCA(infer(e1), infer(e2))
infer(expr_block)   = infer(última expresión del bloque)
infer(let x = e in body) → x : infer(e), luego infer(body)
infer(f(args))      = tipo_retorno_de_f
infer(new T(...))   = T
infer(e.m())        = tipo_retorno_de_m_en_T  donde T = infer(e)
```

**Para símbolos no anotados:** se infiere el tipo más específico consistente con todos los usos.

---

## 11. Verificación de Tipos en Tiempo de Ejecución

```
expr → expr 'is' ID
    check(ID ∈ known_types, "Tipo desconocido en 'is'")
    expr₀.type↑ = Boolean
    -- Comprueba en runtime si expr conforma con ID

expr → expr 'as' ID
    check(ID ∈ known_types, "Tipo desconocido en 'as'")
    expr₀.type↑ = ID
    -- Downcast; puede fallar en runtime si tipos son incompatibles
```

---

## 12. Protocolos

### 12.1 Declaración

```
protocol_decl → 'protocol' ID '{' method_sig_list '}'
    method_sig_list.env↓ = protocol_decl.env↓
    protocol_decl.env↑ = protocol_decl.env↓ ∪ { ID → ProtocolDef(ID, {}, method_sig_list.sigs↑) }

protocol_decl → 'protocol' ID 'extends' ID_parent '{' method_sig_list '}'
    check(ID_parent ∈ protocol_decl.env↓, "Protocolo padre no declarado")
    let parent_sigs = protocol_sigs(ID_parent)
    protocol_decl.env↑ = ... ∪ { ID → ProtocolDef(ID, {ID_parent}, parent_sigs ∪ method_sig_list.sigs↑) }
```

### 12.2 Firmas de Métodos

```
method_sig_list → ε
    method_sig_list.sigs↑ = {}

method_sig_list → method_sig_list method_sig
    method_sig_list₀.sigs↑ = method_sig_list₁.sigs↑ ∪ { method_sig.sig↑ }

method_sig → ID '(' param_list ')' ':' type_ann ';'
    method_sig.sig↑ = MethodSig(ID, param_list.types↑, type_ann.type↑)
```

### 12.3 Conformidad con Protocolos (Tipado Estructural)

Un tipo `T` conforma con protocolo `P` (`T <= P`) si:

```
Para cada MethodSig(m, [A₁..Aₙ], R) en P:
    T tiene método m con parámetros [B₁..Bₙ] y retorno S tal que:
        Aᵢ <= Bᵢ  (contravarianza en argumentos)
        S  <= R    (covarianza en retorno)
```

**No se requiere declaración explícita:** la conformidad es implícita (duck typing estructural).

---

## 13. Iterables

```
-- Protocolo builtin
protocol Iterable {
    next(): Boolean;
    current(): Object;
}

-- Protocolo Enumerable
protocol Enumerable {
    iter(): Iterable;
}
```

**Regla semántica del `for`:**
```
check(conforms(expr_iter.type↑, Iterable) ∨ conforms(expr_iter.type↑, Enumerable),
      "La expresión debe ser iterable")
```

---

## 14. Vectores

### 14.1 Literal Explícito

```
expr → '[' expr_list ']'
    -- Todos los elementos deben tener tipos compatibles
    let T = LCA de todos los tipos en expr_list
    expr.type↑ = Vector(T)

expr_list → expr
    expr_list.types↑ = [ expr.type↑ ]

expr_list → expr_list ',' expr
    expr_list₀.types↑ = expr_list₁.types↑ ++ [ expr.type↑ ]
```

### 14.2 Generador Implícito

```
expr → '[' expr_gen '|' ID 'in' expr_iter ']'
    check(conforms(expr_iter.type↑, Iterable), "Fuente no es iterable")
    let elem_type = current_type(expr_iter.type↑)
    expr_gen.env↓ = expr.env↓ ∪ { ID → elem_type }
    expr.type↑ = Vector(expr_gen.type↑)
```

### 14.3 Acceso por Índice

```
expr → expr_vec '[' expr_idx ']'
    check(expr_vec.type↑ = Vector(T), "No es un vector")
    check(conforms(expr_idx.type↑, Number), "El índice debe ser Number")
    expr.type↑ = T
```

---

## 15. Functores (Funciones de Primera Clase)

### 15.1 Lambda

```
expr → '(' param_list ')' '=>' expr_body
    param_list.env↓ = expr.env↓
    expr_body.env↓  = param_list.env↑
    expr.type↑ = FuncType(param_list.types↑, expr_body.type↑)

expr → '(' param_list ')' ':' type_ann '=>' expr_body
    check(conforms(expr_body.type↑, type_ann.type↑), "Tipo de retorno de lambda incorrecto")
    expr.type↑ = FuncType(param_list.types↑, type_ann.type↑)
```

### 15.2 Invocación de Functor

```
expr → expr_func '(' arg_list ')'
    -- Si expr_func.type↑ = FuncType([A₁..Aₙ], R):
    check(conforms(expr_func.type↑, FuncType), "No es invocable")
    for i: check(conforms(arg_list.types↑[i], param_types[i]), "Tipo de arg incorrecto")
    expr.type↑ = R

    -- Equivale a llamar expr_func.invoke(arg_list)
    -- (protocolo invoke implícito)
```

---

## 16. Macros (Expansión en Tiempo de Compilación)

```
macro_decl → 'def' ID '(' macro_param_list ')' expr_block
    -- Las macros se expanden antes del análisis semántico
    macro_param_list.params↑ guardados para expansión

macro_param → ID ':' type_ann          -- argumento regular
macro_param → '@' ID ':' type_ann      -- argumento simbólico (nombre de variable)
macro_param → '*' ID ':' type_ann      -- argumento expresión (bloque de código)
macro_param → '$' ID ':' type_ann      -- placeholder de variable (generada/fresca)
```

**Semántica de expansión:**
- `@x` — captura el **nombre** del argumento (no su valor)
- `*expr` — inyecta el bloque de código directamente en el cuerpo
- `$var` — genera un nombre de variable fresco (hygienic macro)
- Las variables locales del cuerpo se **renombran automáticamente** para evitar colisiones

---

## 17. Funciones Builtin

| Función/Constante | Tipo | Descripción |
|-------------------|------|-------------|
| `print(x)` | `Object → Object` | Imprime y retorna el valor |
| `sqrt(x)` | `Number → Number` | Raíz cuadrada |
| `sin(x)` | `Number → Number` | Seno (radianes) |
| `cos(x)` | `Number → Number` | Coseno (radianes) |
| `exp(x)` | `Number → Number` | e^x |
| `log(base, x)` | `(Number, Number) → Number` | Logaritmo en base `base` |
| `rand()` | `→ Number` | Número aleatorio en [0,1) |
| `range(a, b)` | `(Number, Number) → Iterable` | Rango [a, b) |
| `PI` | `Number` | Constante π |
| `E` | `Number` | Constante e |

---

## 18. Reglas Léxicas

### Identificadores

```
ID     → LETTER (LETTER | DIGIT | '_')*
LETTER → 'a'..'z' | 'A'..'Z'
DIGIT  → '0'..'9'

-- Válidos:  x, x0, x_0, camelCase, snake_case, MyType
-- Inválidos: _x, 8ball  (no pueden empezar con '_' ni dígito)
```

### Literales

```
NUMBER → DIGIT+ ('.' DIGIT+)?

STRING → '"' (CHAR | ESCAPE_SEQ)* '"'
CHAR       → cualquier carácter excepto '"' y '\'
ESCAPE_SEQ → '\"' | '\\' | '\n' | '\t'

BOOL → 'true' | 'false'
```

### Palabras Reservadas

```
true  false  function  let  in  if  elif  else
while  for  type  inherits  new  self  base
protocol  extends  is  as  def
```

---

## 19. Resumen de la Gramática (BNF simplificado)

```bnf
program       ::= decl* expr ';'?

decl          ::= function_decl
               |  type_decl
               |  protocol_decl
               |  macro_decl

function_decl ::= 'function' ID '(' params? ')' (':' type)? ('=>' expr | block) ';'?

type_decl     ::= 'type' ID ('(' params? ')')?
                  ('inherits' ID ('(' args? ')')?)?
                  '{' member* '}'

protocol_decl ::= 'protocol' ID ('extends' ID)? '{' method_sig* '}'

macro_decl    ::= 'def' ID '(' macro_params? ')' block

member        ::= ID (':' type)? '=' expr ';'
               |  ID '(' params? ')' (':' type)? ('=>' expr | block) ';'?

method_sig    ::= ID '(' params? ')' ':' type ';'

params        ::= param (',' param)*
param         ::= ID (':' type)?

args          ::= expr (',' expr)*

type          ::= ID | ID '[]' | ID '*' | '(' types ')' '->' type
types         ::= type (',' type)*

expr          ::= NUMBER | STRING | 'true' | 'false'
               |  ID
               |  ID ':=' expr
               |  expr ('+' | '-' | '*' | '/' | '^') expr
               |  expr ('@' | '@@') expr
               |  expr ('&' | '|') expr
               |  expr ('==' | '!=' | '<' | '>' | '<=' | '>=') expr
               |  '!' expr | '-' expr
               |  '(' expr ')'
               |  ID '(' args? ')'
               |  'let' bindings 'in' expr
               |  'if' '(' expr ')' expr ('elif' '(' expr ')' expr)* 'else' expr
               |  'while' '(' expr ')' expr
               |  'for' '(' ID 'in' expr ')' expr
               |  'new' ID '(' args? ')'
               |  expr '.' ID
               |  expr '.' ID '(' args? ')'
               |  expr 'is' ID
               |  expr 'as' ID
               |  '[' args ']'
               |  '[' expr '|' ID 'in' expr ']'
               |  expr '[' expr ']'
               |  '(' params? ')' (':' type)? '=>' expr
               |  block

block         ::= '{' (expr ';')+ '}'

bindings      ::= binding (',' binding)*
binding       ::= ID (':' type)? '=' expr
```

---

*Gramática atributada elaborada a partir de la especificación oficial del lenguaje HULK — Universidad de La Habana.*
