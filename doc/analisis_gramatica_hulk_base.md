# Análisis de correctitud de la gramática de HULK

## Alcance evaluado

Este análisis toma como referencia **HULK base hasta type checking**:
- expresiones, bloques, builtins, funciones, variables, condicionales, bucles, tipos, herencia, polimorfismo y anotaciones de tipo;
- **no** incluye como obligatorias las extensiones posteriores: protocolos, iterables tipados, vectores, functors/lambdas y macros.

Ese recorte coincide con el mínimo académico del proyecto: llegar al lenguaje hasta verificación de tipos y dejar la extensión como una fase posterior.

---

## Veredicto corto

La gramática propuesta es una **muy buena base** para un parser de HULK base y está **bien orientada para una estrategia LR/LALR**, porque:

- separa declaraciones globales de la expresión final del programa;
- modela correctamente que `if`, `while`, `for`, `let` y los bloques son **expresiones**;
- incluye funciones globales, tipos, herencia, métodos, atributos, `new`, llamadas, acceso a miembros, `:=`, `is` y `as`;
- codifica precedencia y asociatividad de operadores de forma clara.

Sin embargo, **yo no la daría todavía por cerrada como “correcta final”**. La dejaría como **casi correcta**, con **2 ajustes importantes** y **1 aclaración de alcance**:

1. **`is` y `as` están demasiado permisivos** en la forma actual.
2. **`type_ref` es demasiado estrecha** si más adelante quieren soportar extensiones tipadas como `T*`, `T[]` o tipos de functor.
3. **No cubre extensiones posteriores a A.8**; eso está bien si ese es el alcance base, pero debe quedar explícito.

---

## Qué sí cubre bien

### 1. Estructura global del programa

```bnf
program ::= decl* expr opt_semi
```

Esto captura bien la idea de HULK: cero o más declaraciones globales y una única expresión global final.

### 2. Funciones

Las dos formas documentadas quedan cubiertas:

- inline con `=> expr ;`
- full-form con bloque

```bnf
function_decl
    ::= "function" IDENT "(" params_opt ")" return_ann_opt "=>" expr ";"
     |  "function" IDENT "(" params_opt ")" return_ann_opt block
```

### 3. Variables y `let`

La lista de bindings y el cuerpo como expresión están bien modelados:

```bnf
let_expr ::= "let" binding_list "in" expr
binding_list ::= binding ("," binding)*
binding ::= IDENT type_ann_opt "=" expr
```

Eso además respeta el comportamiento natural de HULK para múltiples variables en `let`.

### 4. Control de flujo como expresiones

```bnf
if_expr    ::= "if" "(" expr ")" expr elif_clause* "else" expr
while_expr ::= "while" "(" expr ")" expr
for_expr   ::= "for" "(" IDENT "in" expr ")" expr
```

Esto está alineado con HULK: `if`, `while` y `for` retornan valor.

### 5. Tipos, miembros, herencia y construcción de objetos

```bnf
type_decl ::= "type" IDENT ctor_params_opt inherits_opt "{" type_member* "}"
```

La estructura general es buena y suficiente para cubrir:

- tipos sin parámetros;
- tipos con parámetros tipo constructor;
- herencia simple;
- inicialización de atributos;
- métodos;
- instanciación con `new`.

### 6. Expresiones y precedencia

La escalera:

```bnf
logic_or -> logic_and -> equality -> relation -> concat -> additive -> multiplicative -> power -> unary -> postfix -> primary
```

está bien pensada para un parser LR/LALR. Además:

- `^` queda asociativo a derecha;
- `+`, `-`, `*`, `/`, `%`, `@`, `@@`, `&`, `|` quedan naturalmente a izquierda;
- llamadas y acceso a miembros quedan en postfix, que es donde deben estar.

### 7. Bloques

```bnf
block ::= "{" block_body_opt "}"
block_body_opt ::= expr (";" expr)* ";"? | ε
```

Esto modela correctamente el bloque como lista de expresiones cuyo valor es el de la última.

---

## Qué le falta o qué conviene corregir

## Aclaración 1: el alcance

Si el objetivo es **HULK base hasta A.8**, entonces la gramática **no está incompleta por no tener**:

- `protocol`
- `extends` de protocolos
- `T*`
- `T[]`
- vectores explícitos e implícitos
- lambdas
- tipos de functor `(T1, ..., Tn) -> R`
- `def`, `match`, `case`, macros

Eso no es un error; simplemente es **alcance base**.

Pero si ustedes quieren vender esta gramática como “la gramática de HULK completa”, entonces **sí le faltan muchas construcciones**.

## Problema 2: `is` y `as` quedaron demasiado permisivos

La regla actual es:

```bnf
relation
    ::= relation "<"  concat
     |  relation ">"  concat
     |  relation "<=" concat
     |  relation ">=" concat
     |  relation "is" type_ref
     |  relation "as" type_ref
     |  concat
```

Esto permite cadenas como:

- `x is A is B`
- `x as A as B`
- `a < b is T`

Syntácticamente eso sale, aunque en HULK normal esas formas no son deseables como sintaxis base.

### Recomendación

Separar explícitamente el nivel de test/cast del nivel relacional.

Una versión más controlada es:

```bnf
equality
    ::= equality "==" relation
     |  equality "!=" relation
     |  relation

relation
    ::= relation "<"  type_test_expr
     |  relation ">"  type_test_expr
     |  relation "<=" type_test_expr
     |  relation ">=" type_test_expr
     |  type_test_expr

type_test_expr
    ::= concat
     |  concat "is" type_ref
     |  concat "as" type_ref
```

Con esto:

- `x is T` y `x as T` siguen válidos;
- evitas la recursión rara de `is`/`as` sobre sí mismas;
- dejas mejor preparado el AST semántico.

## Problema 3: `type_ref ::= IDENT` es correcto hoy, pero malo para extender

Para HULK base nominal, esta regla funciona:

```bnf
type_ref ::= IDENT
```

Pero más adelante se queda corta para:

- iterables tipados: `Number*`
- vectores: `Number[]`
- tipos functor: `(Number) -> Boolean`

### Recomendación de diseño

Desde ahora, cambien el nombre conceptual de `type_ref` a algo más flexible, por ejemplo `type_expr`, aunque por ahora solo derive a `IDENT`.

Versión base:

```bnf
type_expr ::= IDENT
```

Versión extensible:

```bnf
type_expr
    ::= IDENT
     |  type_expr "*"
     |  type_expr "[" "]"
     |  "(" type_expr_list_opt ")" "->" type_expr
```

No hace falta activar eso ya, pero sí conviene dejar el punto de extensión diseñado.

---

## Recomendación global

## Mi conclusión práctica

### Si el parser será LR/LALR

La gramática está **bien encaminada** y la estrategia general es correcta.

### Si el parser fuera LL(1) o recursive descent puro

No usaría esta gramática tal cual, porque:

- tiene recursión izquierda en expresiones;
- la jerarquía de operadores está pensada más para LR que para LL.

Para LL habría que transformarla.

---

## Versión recomendada de la gramática base

A continuación dejo una **versión recomendada** que conserva casi toda su estructura, pero corrige el punto de `is/as` y deja más claro el alcance base.

```bnf
program
    ::= decl* expr opt_semi

opt_semi
    ::= ";"
     |  ε


decl
    ::= function_decl
     |  type_decl


function_decl
    ::= "function" IDENT "(" params_opt ")" return_ann_opt "=>" expr ";"
     |  "function" IDENT "(" params_opt ")" return_ann_opt block


type_decl
    ::= "type" IDENT ctor_params_opt inherits_opt "{" type_member* "}"


ctor_params_opt
    ::= "(" params_opt ")"
     |  ε


inherits_opt
    ::= "inherits" IDENT parent_args_opt
     |  ε


parent_args_opt
    ::= "(" args_opt ")"
     |  ε


type_member
    ::= attr_decl
     |  method_decl


attr_decl
    ::= IDENT type_ann_opt "=" expr ";"


method_decl
    ::= IDENT "(" params_opt ")" return_ann_opt "=>" expr ";"
     |  IDENT "(" params_opt ")" return_ann_opt block


params_opt
    ::= param_list
     |  ε


param_list
    ::= param ("," param)*


param
    ::= IDENT type_ann_opt


return_ann_opt
    ::= ":" type_expr
     |  ε


type_ann_opt
    ::= ":" type_expr
     |  ε


type_expr
    ::= IDENT


expr
    ::= let_expr
     |  if_expr
     |  while_expr
     |  for_expr
     |  assign_expr


let_expr
    ::= "let" binding_list "in" expr


binding_list
    ::= binding ("," binding)*


binding
    ::= IDENT type_ann_opt "=" expr


if_expr
    ::= "if" "(" expr ")" expr elif_clause* "else" expr


elif_clause
    ::= "elif" "(" expr ")" expr


while_expr
    ::= "while" "(" expr ")" expr


for_expr
    ::= "for" "(" IDENT "in" expr ")" expr


assign_expr
    ::= lvalue ":=" expr
     |  logic_or


lvalue
    ::= IDENT
     |  postfix "." IDENT


logic_or
    ::= logic_or "|" logic_and
     |  logic_and


logic_and
    ::= logic_and "&" equality
     |  equality


equality
    ::= equality "==" relation
     |  equality "!=" relation
     |  relation


relation
    ::= relation "<"  type_test_expr
     |  relation ">"  type_test_expr
     |  relation "<=" type_test_expr
     |  relation ">=" type_test_expr
     |  type_test_expr


type_test_expr
    ::= concat
     |  concat "is" type_expr
     |  concat "as" type_expr


concat
    ::= concat "@"  additive
     |  concat "@@" additive
     |  additive


additive
    ::= additive "+" multiplicative
     |  additive "-" multiplicative
     |  multiplicative


multiplicative
    ::= multiplicative "*" power
     |  multiplicative "/" power
     |  multiplicative "%" power
     |  power


power
    ::= unary "^" power
     |  unary


unary
    ::= "!" unary
     |  "-" unary
     |  postfix


postfix
    ::= primary
     |  postfix "(" args_opt ")"
     |  postfix "." IDENT


primary
    ::= literal
     |  IDENT
     |  "new" IDENT "(" args_opt ")"
     |  "(" expr ")"
     |  block


args_opt
    ::= arg_list
     |  ε


arg_list
    ::= expr ("," expr)*


block
    ::= "{" block_body_opt "}"


block_body_opt
    ::= expr (";" expr)* ";"?
     |  ε


literal
    ::= NUMBER
     |  STRING
     |  "true"
     |  "false"
```

---

## Explicación de cada producción

## 1. Programa

### `program ::= decl* expr opt_semi`

Define la unidad completa de compilación:

- primero van cero o más declaraciones globales;
- al final va una sola expresión global, que es el entrypoint;
- puede terminar o no en `;`.

### `opt_semi ::= ";" | ε`

Permite tolerar el `;` final del programa sin volverlo obligatorio.

---

## 2. Declaraciones globales

### `decl ::= function_decl | type_decl`

En la versión base, el programa solo declara funciones y tipos.

### `function_decl`

Modela las dos formas de función:

- inline con `=> expr ;`
- full-form con `block`

### `type_decl`

Declara un tipo nominal:

- nombre del tipo;
- parámetros opcionales de construcción;
- herencia opcional;
- cuerpo con atributos y métodos.

---

## 3. Parámetros, anotaciones y referencias de tipo

### `ctor_params_opt ::= "(" params_opt ")" | ε`

Permite que un tipo tenga o no parámetros tipo constructor.

### `inherits_opt ::= "inherits" IDENT parent_args_opt | ε`

Permite herencia simple opcional.

### `parent_args_opt ::= "(" args_opt ")" | ε`

Permite pasar expresiones al constructor lógico del padre.

### `params_opt`, `param_list`, `param`

Definen listas de parámetros formales con anotación opcional.

### `return_ann_opt`, `type_ann_opt`

Permiten poner anotaciones de tipo tanto en retorno como en símbolos definidos.

### `type_expr ::= IDENT`

En la base, una referencia de tipo es un identificador nominal.

---

## 4. Miembros de tipo

### `type_member ::= attr_decl | method_decl`

El cuerpo de un tipo contiene dos clases de miembros.

### `attr_decl ::= IDENT type_ann_opt "=" expr ";"`

Un atributo siempre se inicializa con una expresión.

### `method_decl`

Un método puede escribirse igual que una función inline o con bloque.

---

## 5. Expresiones de control y binding

### `expr ::= let_expr | if_expr | while_expr | for_expr | assign_expr`

Hace explícito que las construcciones de control son expresiones.

### `let_expr ::= "let" binding_list "in" expr`

Introduce uno o más bindings locales y luego evalúa un cuerpo.

### `binding_list ::= binding ("," binding)*`

Soporta `let` múltiple.

### `binding ::= IDENT type_ann_opt "=" expr`

Cada binding define nombre, anotación opcional e inicialización.

### `if_expr`

Modela un `if` expresivo con `elif*` y `else` obligatorio.

### `while_expr`

Modela el bucle `while` como expresión.

### `for_expr`

Modela el `for (x in expr) body`, que luego puede transpilarse a `while`.

---

## 6. Asignación

### `assign_expr ::= lvalue ":=" expr | logic_or`

Da prioridad más baja a la asignación destructiva.

### `lvalue ::= IDENT | postfix "." IDENT`

Permite asignar a:

- variables simples;
- miembros accedidos por punto.

---

## 7. Expresiones booleanas y de comparación

### `logic_or`, `logic_and`

Definen `|` y `&` con precedencia estándar.

### `equality`

Define `==` y `!=`.

### `relation`

Define `<`, `>`, `<=`, `>=`.

### `type_test_expr`

Agrupa `is` y `as` sin dejarlos encadenarse arbitrariamente.

---

## 8. Concatenación y aritmética

### `concat`

Define `@` y `@@`.

### `additive`

Define `+` y `-` binarios.

### `multiplicative`

Define `*`, `/` y `%`.

### `power`

Define la potencia `^` con asociatividad a derecha.

### `unary`

Define `!` y `-` unario.

---

## 9. Postfijos, primarias y llamadas

### `postfix`

Permite:

- llamadas sucesivas;
- acceso a miembros;
- cadenas como `a.b(c).d`.

### `primary`

Las expresiones atómicas son:

- literales;
- identificadores;
- instanciación con `new`;
- subexpresiones parentizadas;
- bloques.

### `args_opt`, `arg_list`

Definen listas de argumentos actuales.

---

## 10. Bloques y literales

### `block ::= "{" block_body_opt "}"`

Hace del bloque una expresión primaria.

### `block_body_opt ::= expr (";" expr)* ";"? | ε`

Permite un bloque vacío o una secuencia de expresiones separadas por `;`.

### `literal ::= NUMBER | STRING | "true" | "false"`

Define los literales básicos del núcleo del lenguaje.

---

## Cómo extenderla más adelante sin romper el parser

## Extensión 1: protocolos

Agregar:

```bnf
decl ::= function_decl | type_decl | protocol_decl

protocol_decl
    ::= "protocol" IDENT protocol_extends_opt "{" protocol_member* "}"

protocol_extends_opt
    ::= "extends" IDENT
     |  ε

protocol_member
    ::= IDENT "(" typed_params_opt ")" ":" type_expr ";"
```

## Extensión 2: iterables tipados y vectores

Extender `type_expr` y `primary`:

```bnf
type_expr
    ::= IDENT
     |  type_expr "*"
     |  type_expr "[" "]"

primary
    ::= ...
     |  "[" expr_list_opt "]"
     |  "[" expr "|" IDENT "in" expr "]"
```

## Extensión 3: functors y lambdas

Extender `type_expr` y `primary`:

```bnf
type_expr
    ::= ...
     |  "(" type_expr_list_opt ")" "->" type_expr

primary
    ::= ...
     |  "(" params_opt ")" return_ann_opt "=>" expr
```

## Extensión 4: macros

Agregar nuevas declaraciones y pattern matching sobre AST.

---

## Conclusión final

Mi evaluación es esta:

- **Sí**: la gramática tiene una **base sólida** y está bien planteada para el HULK base.
- **Sí**: está **alineada con una implementación por parser LR/LALR**.
- **No del todo**: aún no la marcaría como **versión final correcta**, porque conviene corregir `is/as` y preparar mejor el punto de extensión de tipos.
- **Sí**: es **muy extensible** si desde ahora separan bien `type_expr`, `protocol_decl` y las nuevas formas de `primary`.

En resumen: **no está mal**, **no está lejos**, pero **yo la ajustaría antes de congelarla**.
