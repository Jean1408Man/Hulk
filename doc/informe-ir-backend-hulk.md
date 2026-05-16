# Informe tecnico: incorporacion de una capa IR al backend C++20 del compilador HULK

**Fecha:** 2026-05-15  
**Proyecto:** Compilador HULK  
**Tema:** Como introducir una IR en el backend actual, conservar la generacion a C++20 y alinear la implementacion con la orientacion del proyecto, el plan de implementacion y las conferencias de backend.

---

## 1. Resumen ejecutivo

La implementacion actual del Hito D funciona como un backend **source-to-source**:

```text
.hulk
  -> lexer
  -> parser
  -> SemanticAnalyzer
  -> CppCodegen
  -> archivo .cpp
  -> g++ -std=c++20 -Isrc
  -> ejecutable
```

Esto es tecnicamente posible y, para el subconjunto probado, ya esta validado: el informe del hito reporta `make backend-tests -> 16/16 passed` y `make run-tests -> 76/76 passed`. El backend genera C++20, usa un runtime propio `hulk_runtime.hpp`, modela valores como `hulk_rt::Value`, objetos como `ObjectPtr`, y soporta expresiones, variables, funciones, recursividad, objetos, herencia, dispatch dinamico, `self`, `is` y `as` para el core C4-C6.

El problema no es que C++ sea inutil. El problema es que, segun la teoria de las conferencias y el plan de implementacion, el bloque de backend estaba pensado como:

```text
AST tipado
  -> IR
  -> backend / VM / generador final
  -> ejecucion
```

El plan de Persona 3 define explicitamente los cortes 11, 12 y 13 como **IR + backend + runtime**. Las conferencias tambien insisten en que despues del analisis semantico debe aparecer una representacion intermedia independiente del lenguaje fuente y del lenguaje objetivo. El backend actual salta esa capa y va directamente desde el AST semantico a C++20.

La recomendacion principal es:

> Mantener C++20 como backend final portable, pero insertar una capa IR propia antes de generar C++.

El nuevo pipeline recomendado seria:

```text
.hulk
  -> lexer
  -> parser
  -> AST
  -> binding / inferencia / type checking / SemanticAnalyzer
  -> HulkIR
  -> IRToCppEmitter
  -> C++20
  -> g++
  -> ejecutable
```

Esto permite defender que el proyecto si tiene una fase real de generacion de codigo intermedio, sin destruir el backend que ya funciona. La IR se convierte en el contrato entre el frontend semantico y los backends posibles. Hoy puede existir un backend `IR -> C++20`; manana podria existir `IR -> VM`, `IR -> LLVM` o `IR -> ensamblador`.

---

## 2. Fuentes internas usadas

Este informe se basa en los siguientes documentos del proyecto:

1. `orientacion-proyecto.txt`  
   Define las restricciones del proyecto: stack en C/C++/Rust; backend por codigo de maquina, LLVM o maquina virtual propia con codigo intermedio propio.

2. `plan-3-personas-cortes-backend.md`  
   Define Persona 3 como cortes 11, 12 y 13: lowering a IR, backend de ejecucion y runtime/memoria.

3. `informe-implementacion-hito-d-backend.md`  
   Describe el estado actual: backend C++20, runtime propio, CLI, generador directo `CppCodegen`, tests, limitaciones e incongruencias.

4. `13-codegen.pdf`  
   Conferencia sobre generacion de codigo: IR independiente, codigo de tres direcciones, generacion desde AST, tabla de simbolos y optimizaciones basicas.

5. `14-il.pdf`  
   Conferencia sobre lenguaje intermedio/CIL: necesidad de lenguaje intermedio, estructura `.TYPES`, `.DATA`, `.CODE`, instrucciones como `GETATTR`, `SETATTR`, `ALLOCATE`, `PARAM`, `CALL`, `VCALL`, `RETURN`, y advertencia sobre transpiladores.

6. `15-object-code.pdf`  
   Conferencia sobre codigo objeto: dificultad de generar codigo de maquina real, registros, memoria, vtables, heap, GC, VM y JIT.

7. `hulk-docs.pdf`  
   Documentacion didactica de HULK y BANNER IR: BANNER como IR de tres direcciones, lowering, layouts de objetos, secciones `.TYPES`, `.DATA`, `.CODE`, dispatch dinamico y filosofia de valores bajos.

---

## 3. Que es una IR

Una **IR** o **representacion intermedia** es un lenguaje interno del compilador. No es el lenguaje fuente HULK, y tampoco es necesariamente ensamblador, LLVM o C++. Es una forma mas simple, explicita y uniforme de representar el programa despues de que el frontend ya entendio su sintaxis y su semantica.

La IR existe para evitar este salto demasiado grande:

```text
AST de HULK directamente a codigo de maquina
```

Ese salto es dificil porque hay que resolver al mismo tiempo:

- expresiones anidadas;
- orden de evaluacion;
- scopes;
- temporales;
- llamadas;
- recursividad;
- objetos;
- herencia;
- dispatch virtual;
- layout de memoria;
- strings;
- heap;
- registros;
- stack frames;
- convenciones de llamada;
- liberacion de memoria.

La IR separa esos problemas. Primero transforma HULK en un programa mas bajo y explicito:

```text
AST tipado -> IR
```

Despues otro modulo transforma o ejecuta esa IR:

```text
IR -> C++20
IR -> VM
IR -> LLVM
IR -> ensamblador
```

Las conferencias describen la IR como una representacion independiente del lenguaje fuente y objetivo, facil de generar desde el AST, facil de traducir a codigo objeto y adecuada para optimizaciones. El codigo de tres direcciones aparece como formato natural porque cada instruccion contiene, como maximo, dos operandos de entrada y un destino.

Ejemplo:

```hulk
print((1 + 2) * 3);
```

No deberia bajar directamente a una expresion C++ gigante. En IR, lo correcto es aplanarlo:

```text
t0 = const_number 1
t1 = const_number 2
t2 = add t0 t1
t3 = const_number 3
t4 = mul t2 t3
param t4
t5 = call print 1
return t5
```

La idea central es:

> Cada expresion HULK produce una secuencia de instrucciones IR y devuelve el temporal donde quedo su resultado.

---

## 4. Diferencia entre AST, IR, C++ generado y codigo objeto

### 4.1 AST

El AST conserva la estructura sintactica del programa. Es un arbol.

Ejemplo:

```hulk
(1 + 2) * 3
```

AST conceptual:

```text
Mul
├── Add
│   ├── Number(1)
│   └── Number(2)
└── Number(3)
```

El AST es comodo para analisis semantico, inferencia, type checking y resolucion de nombres. No es comodo como representacion final de ejecucion porque tiene anidamiento, scopes implicitos y estructuras de control de alto nivel.

### 4.2 IR

La IR elimina el anidamiento y vuelve explicitos los temporales, etiquetas y saltos.

```text
t0 = 1
t1 = 2
t2 = t0 + t1
t3 = 3
t4 = t2 * t3
```

La IR no deberia depender de que el backend final sea C++20, LLVM o una VM. Puede tener instrucciones especificas para HULK, como `new_object`, `getfield`, `setfield`, `vcall`, `is_type`, `as_type`.

### 4.3 C++ generado

C++ generado es codigo fuente de otro lenguaje. Puede ser un **target portable**, pero no deberia reemplazar a la IR si el objetivo academico es demostrar generacion de codigo intermedio.

El C++ puede ser el backend final:

```text
IR -> C++20 -> g++ -> ejecutable
```

Esto es defendible si se dice claramente que C++20 es el lenguaje objetivo portable y que `g++` se usa como generador nativo final.

### 4.4 Codigo objeto / ensamblador

Codigo objeto o ensamblador esta mucho mas cerca de la maquina. Ahi aparecen registros, pila, heap, llamadas, offsets reales y convenciones de ABI. Las conferencias remarcan que generar codigo de maquina eficiente es tedioso y agrega muchos problemas: registros, memoria, layout, dispatch polimorfico, reserva/liberacion de memoria y eficiencia. Por eso no conviene saltar ahora a ensamblador.

---

## 5. Diagnostico de la implementacion actual

### 5.1 Lo que esta bien

El backend actual ya tiene valor real:

- Tiene CLI:
  ```bash
  ./hulk_backend archivo.hulk -o ejecutable
  ./hulk_backend archivo.hulk --emit-cpp -o salida.cpp
  ```

- Tiene pipeline funcional:
  ```text
  .hulk
    -> lexer
    -> parser
    -> SemanticAnalyzer
    -> CppCodegen
    -> archivo .cpp
    -> g++
    -> ejecutable
  ```

- Tiene runtime propio:
  ```text
  src/backend/runtime/hulk_runtime.hpp
  ```

- Tiene representacion dinamica de valores:
  ```cpp
  hulk_rt::Value = std::variant<Nil, double, std::string, bool, ObjectPtr>
  ```

- Tiene objetos con `type_name` y tabla dinamica de campos.

- Tiene registro dinamico de tipos y metodos.

- Tiene operaciones runtime para:
  - conversiones;
  - aritmetica;
  - logica;
  - comparaciones;
  - concatenacion `@` y `@@`;
  - builtins;
  - campos;
  - `is`;
  - `as`;
  - dispatch dinamico;
  - llamada tipo `base`.

- Genera funciones globales, constructores, inicializadores de tipos, metodos y `main`.

- Resuelve nombres C++ validos, shadowing y simbolos sinteticos.

- Los metodos reciben `self` explicito, lo cual coincide con el modelo de lowering recomendado.

- Tiene pruebas de backend y pruebas generales pasando.

Todo eso debe conservarse.

### 5.2 El punto debil principal

El punto debil es que el codigo actual parece hacer:

```text
AST semantico -> C++20
```

sin una representacion intermedia propia.

Eso contradice parcialmente el plan del backend, que esperaba:

```text
AST tipado -> IR -> backend -> runtime
```

y contradice la intencion de las conferencias, que presentan la IR como paso central despues del analisis semantico.

La solucion no es eliminar C++20. La solucion es insertar:

```text
AST semantico -> IR -> C++20
```

---

## 6. Veredicto sobre compilar a C++20

Compilar a C++20 no es intrinsecamente incorrecto.

Lo incorrecto seria presentarlo como si fuera equivalente a haber implementado la arquitectura completa de IR + backend + runtime descrita por el plan. Si el compilador solo genera C++ desde el AST, entonces academicamente se parece mas a un transpilador que a un backend con IR explicita.

La forma correcta de defenderlo es:

> Nuestro compilador baja HULK a una IR propia. Luego tenemos un backend `IR -> C++20`, donde C++20 funciona como lenguaje objetivo portable. Finalmente, `g++` realiza la generacion de codigo nativo.

Con esa arquitectura, C++20 deja de ser un atajo y se vuelve un backend final.

---

## 7. No conviene cambiar ahora a ensamblador

No recomiendo migrar a ensamblador directo.

La conferencia de codigo objeto enumera problemas que CIL/IR no resuelve por si sola y que aparecen al generar codigo real:

- que datos mover de memoria a registros;
- que registros usar;
- donde vive cada atributo de un objeto;
- que metodo concreto se ejecuta en una llamada polimorfica;
- cuando reservar memoria para objetos;
- como organizar el heap;
- cuando liberar memoria;
- como hacerlo de forma razonablemente eficiente.

Ese trabajo no les ayudaria a cerrar rapido el proyecto. Ahora mismo el mayor valor esta en hacer explicita la capa IR y alinear semantica, no en reescribir todo a bajo nivel.

---

## 8. Arquitectura recomendada

### 8.1 Pipeline objetivo

El pipeline recomendado queda asi:

```text
HULK source
  |
  v
Lexer / Parser
  |
  v
AST
  |
  v
SemanticAnalyzer
  |
  v
AST resuelto y tipado
  |
  v
IRGen
  |
  v
HulkIR
  |
  +--> IRPrinter       (--emit-ir)
  |
  +--> IRToCppEmitter  (--emit-cpp / default executable)
  |
  v
C++20
  |
  v
g++
  |
  v
Executable
```

### 8.2 Pipeline alternativo futuro

Una vez que la IR exista, se pueden agregar backends sin tocar el frontend:

```text
HulkIR -> VM propia
HulkIR -> LLVM IR
HulkIR -> MIPS / ASM
```

Eso es precisamente la ventaja de la IR: desacoplar el lenguaje fuente del destino.

---

## 9. Tipo de IR recomendado para el proyecto actual

Hay dos alternativas.

### 9.1 IR baja tipo BANNER/CIL

Esta es la mas cercana a la teoria de las conferencias:

- todos los valores se representan como enteros/punteros;
- los strings van a `.DATA`;
- los tipos van a `.TYPES`;
- las funciones/metodos van a `.CODE`;
- no hay scopes;
- no hay herencia implicita;
- los campos se resuelven por offset;
- los metodos virtuales se resuelven por slots de vtable;
- instrucciones como `ALLOCATE`, `GETATTR`, `SETATTR`, `TYPEOF`, `PARAM`, `CALL`, `VCALL`, `RETURN`.

Ventaja: maxima alineacion teorica.  
Desventaja: exige reescribir mas runtime y bajar bastante el nivel.

### 9.2 IR de nivel medio basada en `hulk_rt::Value`

Esta es la recomendada para ustedes ahora.

Mantiene la semantica actual del runtime C++:

- valores como `hulk_rt::Value`;
- objetos como `ObjectPtr`;
- strings como `std::string`;
- campos por nombre inicialmente;
- dispatch por nombre inicialmente;
- operaciones runtime reutilizadas.

Pero agrega una IR explicita:

```text
.TYPES
.DATA
.CODE
```

y linealiza expresiones, control de flujo y llamadas.

Ventaja: introduce la capa academica necesaria sin romper lo que ya funciona.  
Desventaja: no es tan baja como BANNER/CIL puro.

Recomendacion:

> Implementen primero una **HulkIR de nivel medio** y dejen BANNER/CIL bajo como evolucion opcional.

---

## 10. Forma propuesta de la IR

### 10.1 Estructura global

```cpp
namespace Hulk::IR {

struct IRProgram {
    std::vector<IRType> types;
    std::vector<IRData> data;
    std::vector<IRFunction> functions;
    std::string entry_function = "hulk_main";
};

struct IRType {
    std::string name;
    std::optional<std::string> parent;
    std::vector<IRField> fields;
    std::vector<IRMethod> methods;
};

struct IRField {
    std::string owner_type;
    std::string name;
    std::string lowered_name; // ejemplo: Point_x
    std::string type_name;
    int slot = -1;            // puede quedar -1 al inicio si usan campos por nombre
};

struct IRMethod {
    std::string owner_type;
    std::string name;
    std::string function_name; // ejemplo: Point_getX
    int slot = -1;             // para vtables futuras
};

struct IRData {
    std::string label;
    std::string value;
};

struct IRFunction {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::string> locals;
    std::vector<IRInstr> body;
};

}
```

### 10.2 Instrucciones

```cpp
enum class IROp {
    NOP,

    // Constantes y movimiento
    CONST_NIL,
    CONST_NUMBER,
    CONST_BOOL,
    CONST_STRING,
    LOAD_DATA,
    MOVE,

    // Aritmetica
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW,
    NEG,

    // Logica
    AND,
    OR,
    NOT,

    // Comparacion
    EQ,
    NEQ,
    LT,
    GT,
    LEQ,
    GEQ,

    // Strings
    CONCAT,
    CONCAT_SP,

    // Control de flujo
    LABEL,
    JUMP,
    JUMP_IF_TRUE,
    JUMP_IF_FALSE,

    // Llamadas
    PARAM,
    CALL,
    RETURN,

    // Objetos
    NEW_OBJECT,
    GETFIELD,
    SETFIELD,
    VCALL,
    SCALL_METHOD,   // llamada estatica a implementacion concreta, util para base()

    // Tipos dinamicos
    IS_TYPE,
    AS_TYPE,

    // Builtins
    BUILTIN_PRINT,
    BUILTIN_SQRT,
    BUILTIN_SIN,
    BUILTIN_COS,
    BUILTIN_EXP,
    BUILTIN_LOG,
    BUILTIN_RAND
};
```

### 10.3 Instruccion concreta

```cpp
struct IRInstr {
    IROp op;

    std::string dest;   // temporal/local destino o label
    std::string src1;   // operando 1, objeto, funcion o label
    std::string src2;   // operando 2, campo, metodo, tipo, etc.

    std::vector<std::string> args;

    double number_value = 0.0;
    bool bool_value = false;
    std::string string_value;

    SourceSpan span;    // opcional: para errores runtime/debug
};
```

Esta estructura es flexible. Para instrucciones simples:

```text
t2 = add t0 t1
```

seria:

```cpp
IRInstr{ .op = IROp::ADD, .dest = "t2", .src1 = "t0", .src2 = "t1" }
```

Para llamadas:

```text
t5 = call fib(t4)
```

seria:

```cpp
IRInstr{
    .op = IROp::CALL,
    .dest = "t5",
    .src1 = "fib",
    .args = {"t4"}
}
```

Tambien se puede usar `PARAM` + `CALL` como en CIL/BANNER. Para emitir C++ es mas simple guardar `args` dentro de `CALL`; para parecerse mas a la conferencia, se puede imprimir como `PARAM` + `CALL` aunque internamente se guarde una lista de argumentos.

---

## 11. Formato textual recomendado

Aunque internamente usen structs C++, conviene imprimir una IR textual legible.

Ejemplo:

```text
.TYPES
type Point parent Object {
  field Point_x : Number slot 0
  field Point_y : Number slot 1
  method getX -> Point_getX slot 0
}

.DATA
s0 = "x: "

.CODE
function Point.__init__(self, x, y) {
  setfield self Point_x x
  setfield self Point_y y
  return self
}

function Point_getX(self) {
  t0 = getfield self Point_x
  return t0
}

function hulk_main() {
  t0 = const_number 3
  t1 = const_number 4
  t2 = new_object Point
  t3 = call Point.__init__(t2, t0, t1)
  t4 = vcall t2 getX()
  t5 = builtin_print(t4)
  return t5
}
```

Esto ya se parece a la teoria:

- `.TYPES` aplana informacion de tipos;
- `.DATA` guarda strings;
- `.CODE` contiene funciones linealizadas;
- los metodos reciben `self`;
- `new`, `getfield`, `setfield`, `vcall` son explicitos.

---

## 12. Como acoplar la IR al proyecto actual

### 12.1 Nueva estructura de carpetas

Propuesta:

```text
src/
  ir/
    ir.h
    ir.cpp
    ir_printer.h
    ir_printer.cpp
    ir_builder.h
    ir_builder.cpp

  backend/
    main.cpp
    backend.h
    backend.cpp
    backend_driver.h
    backend_driver.cpp

    irgen/
      ir_gen.h
      ir_gen.cpp
      lower_expr.cpp
      lower_decl.cpp
      lower_type.cpp

    cpp/
      ir_to_cpp.h
      ir_to_cpp.cpp

    runtime/
      hulk_runtime.hpp

    legacy/
      codegen_cpp.h
      codegen_cpp.cpp
```

No tienen que mover todo de golpe. Pueden dejar `codegen_cpp.*` donde esta y crear `IRToCppEmitter` aparte. Cuando el nuevo pipeline pase las pruebas, el generador directo se puede marcar como legacy o eliminar.

### 12.2 Cambios en el driver

El driver actual hace:

```cpp
SemanticAnalyzer semantic;
semantic.analyze(program);

CppCodegen codegen;
std::string cpp = codegen.generate(program);
```

Debe cambiar a:

```cpp
SemanticAnalyzer semantic;
semantic.analyze(program);

IRGen irgen(semantic.tables(), semantic.type_map(), semantic.resolution_map());
IRProgram ir = irgen.generate(program);

if (options.emit_ir) {
    IRPrinter printer;
    write_file(output, printer.print(ir));
    return;
}

IRToCppEmitter emitter;
std::string cpp = emitter.emit(ir);

if (options.emit_cpp) {
    write_file(output, cpp);
    return;
}

compile_with_gpp(cpp, output_executable);
```

### 12.3 Nuevas opciones CLI

Mantengan las actuales y agreguen:

```bash
./hulk_backend archivo.hulk --emit-ir -o salida.hir
./hulk_backend archivo.hulk --emit-cpp -o salida.cpp
./hulk_backend archivo.hulk -o ejecutable
```

Opcionalmente:

```bash
./hulk_backend archivo.hulk --legacy-cpp -o salida.cpp
```

para comparar el generador anterior con el nuevo.

### 12.4 Regla de oro

El generador C++ no debe mirar el AST.

Debe mirar solo la IR.

```text
Correcto:
AST -> IR -> C++

Incorrecto:
AST -> IR
AST -> C++
```

Si `IRToCppEmitter` todavia necesita consultar nodos AST, entonces la IR no esta capturando suficiente informacion.

---

## 13. Lowering: como bajar cada construccion HULK

### 13.1 Literales

HULK:

```hulk
42
"hola"
true
```

IR:

```text
t0 = const_number 42
t1 = const_string "hola"
t2 = const_bool true
```

### 13.2 Operaciones binarias

HULK:

```hulk
a + b
```

IR:

```text
t0 = lower(a)
t1 = lower(b)
t2 = add t0 t1
```

Plantilla:

```cpp
std::string IRGen::lower_binary(const BinaryExpr& e) {
    auto left = lower_expr(e.left());
    auto right = lower_expr(e.right());
    auto result = new_temp();

    emit(IRInstr{
        .op = map_binary_op(e.op()),
        .dest = result,
        .src1 = left,
        .src2 = right
    });

    return result;
}
```

### 13.3 Bloques

HULK:

```hulk
{
    print(1);
    2 + 3;
}
```

IR:

```text
t0 = const_number 1
t1 = builtin_print(t0)
t2 = const_number 2
t3 = const_number 3
t4 = add t2 t3
return_value = t4
```

El resultado de un bloque es el resultado de su ultima expresion.

### 13.4 `let`

HULK:

```hulk
let x = 10, y = x + 1 in y * 2
```

IR:

```text
t0 = const_number 10
x_0 = move t0

t1 = const_number 1
t2 = add x_0 t1
y_0 = move t2

t3 = const_number 2
t4 = mul y_0 t3
```

El punto importante: el segundo binding ve el primero. Por tanto, el `IRGen` debe tener un ambiente de variables:

```cpp
class IRGenContext {
    std::vector<std::unordered_map<const Symbol*, std::string>> scopes;
};
```

Es mejor mapear simbolos resueltos, no nombres crudos, porque HULK permite shadowing.

### 13.5 Asignacion destructiva

HULK:

```hulk
x := x + 1
```

IR:

```text
t0 = const_number 1
t1 = add x_0 t0
x_0 = move t1
```

La asignacion devuelve el valor asignado. Por eso `lower_expr(assign)` debe retornar el local actualizado o un temporal con el nuevo valor.

### 13.6 `if`

HULK:

```hulk
if (x > 0) x else -x
```

IR:

```text
t0 = const_number 0
t1 = gt x_0 t0
jump_if_false t1 L_else_0

t2 = move x_0
t_result = move t2
jump L_end_0

label L_else_0
t3 = neg x_0
t_result = move t3

label L_end_0
```

En HULK el `if` es expresion; por tanto siempre debe producir un valor.

### 13.7 `while`

HULK:

```hulk
while (x > 0) {
    print(x);
    x := x - 1;
}
```

IR:

```text
label L_while_start_0
t0 = const_number 0
t1 = gt x_0 t0
jump_if_false t1 L_while_end_0

t2 = builtin_print(x_0)
t3 = const_number 1
t4 = sub x_0 t3
x_0 = move t4

jump L_while_start_0
label L_while_end_0
t_result = nil
```

La convencion sobre el valor retornado por `while` debe alinearse con el evaluador y la documentacion. La documentacion de HULK trata los loops como expresiones y el `for` como equivalente a un `while`. Si el evaluador actual usa una convencion especifica, la IR debe copiar esa convencion.

### 13.8 Funciones globales

HULK:

```hulk
function square(x: Number): Number => x * x;
```

IR:

```text
function square(x) {
  t0 = mul x x
  return t0
}
```

La funcion global principal del programa puede llamarse:

```text
function hulk_main() {
  ...
}
```

### 13.9 Llamadas a funcion

HULK:

```hulk
fib(n - 1)
```

IR:

```text
t0 = const_number 1
t1 = sub n t0
t2 = call fib(t1)
```

### 13.10 Objetos: `new`

HULK:

```hulk
new Point(3, 4)
```

IR:

```text
t0 = new_object Point
t1 = const_number 3
t2 = const_number 4
t3 = call Point.__init__(t0, t1, t2)
```

Convencion:

- `new_object Type` solo reserva objeto y le asigna tipo dinamico.
- `Type.__init__` inicializa padre y atributos.
- El resultado util de la expresion `new Point(...)` es `t0`.

### 13.11 Inicializadores de tipos

HULK:

```hulk
type Point(x, y) {
    x = x;
    y = y;
}
```

IR:

```text
function Point.__init__(self, x, y) {
  setfield self Point_x x
  setfield self Point_y y
  return self
}
```

Si hay herencia:

```hulk
type Dog(name) inherits Animal(name) {
    age = 0;
}
```

IR:

```text
function Dog.__init__(self, name) {
  t0 = call Animal.__init__(self, name)
  t1 = const_number 0
  setfield self Dog_age t1
  return self
}
```

### 13.12 Acceso a campos

HULK:

```hulk
self.x
```

IR:

```text
t0 = getfield self Point_x
```

Si deciden mantener campos publicos temporalmente:

```hulk
p.x
```

tambien baja a:

```text
t0 = getfield p Point_x
```

Pero esta es una decision semantica que deben alinear, porque HULK define atributos privados.

### 13.13 Asignacion a campo

HULK:

```hulk
self.x := value
```

IR:

```text
t0 = lower(value)
setfield self Point_x t0
```

### 13.14 Llamada a metodo virtual

HULK:

```hulk
a.speak()
```

IR:

```text
t0 = vcall a speak()
```

El objeto receptor se evalua primero. En la llamada real, `self` es parametro implicito.

### 13.15 `base()`

HULK:

```hulk
base()
```

Dentro de un metodo, no debe ser una llamada virtual normal. Debe llamar a la implementacion del padre o ancestro mas cercano.

IR recomendada:

```text
t0 = scall_method Parent.method(self, args...)
```

o:

```text
t0 = call Parent.method(self, args...)
```

La diferencia con `vcall` es crucial:

- `vcall self name()` podria volver al metodo override del hijo.
- `call Parent.name(self, ...)` llama estaticamente al metodo del padre.

### 13.16 `is`

HULK:

```hulk
x is Dog
```

IR:

```text
t0 = is_type x Dog
```

Debe consultar el tipo dinamico del objeto.

### 13.17 `as`

HULK:

```hulk
x as Dog
```

IR:

```text
t0 = as_type x Dog
```

Debe devolver el objeto casteado si es valido o producir error runtime si no lo es.

---

## 14. Como emitir C++ desde la IR

El nuevo `IRToCppEmitter` debe transformar instrucciones IR a C++ usando el runtime actual.

### 14.1 Funcion IR a funcion C++

IR:

```text
function square(x) {
  t0 = mul x x
  return t0
}
```

C++:

```cpp
hulk_rt::Value hulk_fn_square(const std::vector<hulk_rt::Value>& args) {
    hulk_rt::Value x = args.at(0);
    hulk_rt::Value t0 = hulk_rt::mul(x, x);
    return t0;
}
```

O, si quieren parametros C++ explicitos:

```cpp
hulk_rt::Value hulk_fn_square(hulk_rt::Value x) {
    hulk_rt::Value t0 = hulk_rt::mul(x, x);
    return t0;
}
```

La primera opcion es mas uniforme para `CALL` y `VCALL`.

### 14.2 Constantes

IR:

```text
t0 = const_number 42
t1 = const_string "hola"
t2 = const_bool true
```

C++:

```cpp
hulk_rt::Value t0 = hulk_rt::Value(42.0);
hulk_rt::Value t1 = hulk_rt::Value(std::string("hola"));
hulk_rt::Value t2 = hulk_rt::Value(true);
```

### 14.3 Aritmetica

IR:

```text
t2 = add t0 t1
```

C++:

```cpp
hulk_rt::Value t2 = hulk_rt::add(t0, t1);
```

### 14.4 Labels y saltos

IR:

```text
jump_if_false cond L_else
...
jump L_end
label L_else
...
label L_end
```

C++:

```cpp
if (!hulk_rt::truthy(cond)) goto L_else;
...
goto L_end;
L_else:
...
L_end:
```

Usar `goto` dentro de C++ generado no es un problema. De hecho, se alinea mejor con una IR lineal que reconstruir `if`/`while` estructurados.

### 14.5 Llamadas

IR:

```text
t0 = call fib(t_arg)
```

C++:

```cpp
hulk_rt::Value t0 = hulk_fn_fib({t_arg});
```

### 14.6 New object

IR:

```text
t0 = new_object Point
t1 = call Point.__init__(t0, x, y)
```

C++:

```cpp
hulk_rt::Value t0 = hulk_rt::make_object("Point");
hulk_rt::Value t1 = hulk_ctor_Point({t0, x, y});
```

### 14.7 Field access

IR:

```text
t0 = getfield obj Point_x
setfield obj Point_x value
```

C++ inicial:

```cpp
hulk_rt::Value t0 = hulk_rt::get_field(hulk_rt::as_object(obj), "x");
hulk_rt::set_field(hulk_rt::as_object(obj), "x", value);
```

C++ futuro con slots:

```cpp
hulk_rt::Value t0 = hulk_rt::get_field_slot(hulk_rt::as_object(obj), 0);
hulk_rt::set_field_slot(hulk_rt::as_object(obj), 0, value);
```

### 14.8 VCALL

IR:

```text
t0 = vcall obj speak(args...)
```

C++ inicial:

```cpp
hulk_rt::Value t0 = hulk_rt::call_method(
    hulk_rt::as_object(obj),
    "speak",
    { args... }
);
```

C++ futuro con vtable slots:

```cpp
hulk_rt::Value t0 = hulk_rt::call_method_slot(
    hulk_rt::as_object(obj),
    slot_speak,
    { args... }
);
```

### 14.9 `.TYPES` a registro runtime

IR:

```text
type Dog parent Animal {
  field Animal_name : String slot 0
  method speak -> Dog_speak slot 0
}
```

C++:

```cpp
hulk_rt::register_type("Dog", "Animal");
hulk_rt::register_field("Dog", "name");
hulk_rt::register_method("Dog", "speak", hulk_method_Dog_speak);
```

Aunque el runtime actual use tablas dinamicas por nombre, generar esta informacion desde `.TYPES` ya alinea el backend con la arquitectura teorica.

---

## 15. Plan de migracion incremental

### Fase 0: Congelar baseline

Antes de tocar el backend:

```bash
make backend-tests
make run-tests
```

Guardar resultados. El objetivo es que cada fase preserve esos tests.

### Fase 1: Crear `src/ir`

Entregables:

- `ir.h`
- `ir.cpp`
- `ir_printer.h`
- `ir_printer.cpp`

Tests:

- construir manualmente un `IRProgram`;
- imprimirlo;
- comparar con un `.expected`.

Ejemplo de test:

```cpp
IRProgram p;
p.functions.push_back(make_fib_ir());
EXPECT_EQ(IRPrinter().print(p), expected_text);
```

### Fase 2: Implementar `IRGen` para C4

Cubrir:

- literales;
- aritmetica;
- logica;
- strings;
- builtins;
- bloques;
- `let`;
- `if`;
- `while`.

Agregar:

```bash
./hulk_backend tests/eval/c4_*.hulk --emit-ir
```

### Fase 3: Implementar funciones C5

Cubrir:

- `FunctionDecl`;
- `FunctionCall`;
- parametros;
- recursividad;
- destructivo `:=`.

Meta:

```hulk
function fib(n) => if (n <= 1) n else fib(n-1) + fib(n-2);
print(fib(10));
```

IR textual legible y C++ generado desde IR.

### Fase 4: Reemplazar `CppCodegen` directo por `IRToCppEmitter`

Primero hagan que `IRToCppEmitter` use el runtime actual sin optimizar.

Objetivo:

```text
AST -> IR -> C++ -> g++
```

y no:

```text
AST -> C++
```

### Fase 5: Objetos C6

Cubrir:

- `.TYPES`;
- `new_object`;
- `Type.__init__`;
- `getfield`;
- `setfield`;
- `vcall`;
- `is_type`;
- `as_type`;
- `self`;
- `base`.

### Fase 6: Corregir contradicciones semanticas

Antes de declarar el backend final:

1. Atributos publicos vs privados.
2. `is` entre tipos no relacionados.
3. `base()` con representacion canonica.
4. `self` como identificador especial o nodo propio.
5. `for/range`.
6. Separacion de features core vs extensiones.
7. Parser generado vs `grammar.y`.

### Fase 7 opcional: VM

Solo despues de tener IR y C++ funcionando, implementen una VM si quieren mayor alineacion con el plan original.

Una VM puede interpretar la misma IR:

```text
HULK -> AST -> IR -> VM -> salida
```

Pero no es necesario destruir el backend C++ para llegar ahi.

---

## 16. Que contradice o tensiona la teoria actual

Esta seccion distingue entre **contradiccion fuerte**, **desviacion defendible** y **pendiente de alineacion**.

### 16.1 AST directo a C++20 sin IR explicita

**Estado actual:** el backend genera C++20 directamente desde el AST semantico mediante `CppCodegen`.

**Teoria/plan:** despues del analisis semantico debe haber una IR. El plan de Persona 3 define el corte 11 como lowering a IR y el corte 12 como backend de ejecucion de esa IR.

**Evaluacion:** contradiccion arquitectonica fuerte.

**Correccion:** introducir `IRGen` y `IRToCppEmitter`.

---

### 16.2 C++ como "IR" implicita

**Estado actual:** C++20 cumple parcialmente el rol de representacion intermedia porque todavia no es codigo de maquina; `g++` hace el paso final.

**Teoria/conferencia:** si se usa un subconjunto de otro lenguaje como codigo intermedio, eso se parece a un transpilador y exige modelar formalmente esa representacion.

**Evaluacion:** defendible solo si se formaliza. Sin IR propia, es debil academicamente.

**Correccion:** no decir "C++ es nuestra IR" sin mas. Decir:

```text
HulkIR es nuestra IR.
C++20 es un target portable emitido desde HulkIR.
```

---

### 16.3 Lambdas inmediatas de C++ para `let`, bloques, `if`, `while`

**Estado actual:** el generador usa lambdas inmediatas de C++ para preservar la semantica de expresion de HULK.

**Teoria:** el lowering deberia convertir estructuras de control en labels, saltos y temporales.

**Evaluacion:** desviacion defendible como tecnica de emision C++, pero no como IR.

**Correccion:** en la IR, `if` y `while` deben verse como `LABEL`, `JUMP`, `JUMP_IF_FALSE`. El emisor C++ puede usar `goto` o reconstruir C++, pero la IR debe ser lineal.

---

### 16.4 Runtime dinamico por nombres vs layout aplanado

**Estado actual:** `hulk_rt::Object` usa `type_name` y tabla dinamica de campos. Los metodos se registran dinamicamente por nombre.

**Teoria:** BANNER/CIL aplana jerarquias; los atributos heredados tienen offsets fijos; los metodos se mapean a etiquetas/slots; `GETATTR` y `SETATTR` acceden por layout explicito.

**Evaluacion:** desviacion de bajo nivel. Funciona, pero es mas alto nivel que la teoria.

**Correccion incremental:**

1. Agregar `.TYPES` en IR.
2. Generar nombres bajados de campos: `Point_x`, `Animal_name`.
3. Asignar slots aunque el runtime todavia use nombres.
4. Luego migrar runtime a slots si hay tiempo.

---

### 16.5 No hay `.TYPES`, `.DATA`, `.CODE` visibles

**Estado actual:** el C++ generado contiene prototipos, funciones, registro de tipos y `main`, pero no existe una representacion intermedia textual con secciones.

**Teoria:** BANNER/CIL organiza el programa en tipos, datos y codigo.

**Evaluacion:** contradiccion de presentacion y arquitectura.

**Correccion:** el `IRPrinter` debe emitir:

```text
.TYPES
.DATA
.CODE
```

aunque el emisor C++ use internamente otras estructuras.

---

### 16.6 El backend tolera errores semanticos

**Estado actual:** el backend tolera dos diagnosticos semanticos para poder compilar tests C6:
- acceso publico a atributos;
- `is` no plausible.

**Teoria:** el backend debe asumir que el programa ya paso analisis semantico y type checking. En BANNER/CIL la VM no tiene por que proteger contra instrucciones inconsistentes: la consistencia la garantiza el compilador.

**Evaluacion:** contradiccion fuerte entre fases.

**Correccion:** alinear semantica/evaluador/tests para que el backend no necesite excepciones especiales. Mientras tanto, documentar que son tolerancias transitorias.

---

### 16.7 Atributos privados vs tests con acceso publico

**Estado actual:** el evaluador acepta `p.x` y `p.y`; el semantico lo rechaza diciendo que los atributos son privados y solo se acceden via `self`; el backend tolera ese error.

**Teoria HULK:** los atributos son privados y los metodos publicos/virtuales.

**Evaluacion:** contradiccion semantica fuerte.

**Opciones:**

1. Alinear con HULK: atributos privados. Cambiar tests/evaluador.
2. Alinear con tests actuales: atributos publicos en el subconjunto implementado. Cambiar semantico y documentar divergencia.
3. Solucion mixta no recomendada: privado salvo bandera de compatibilidad.

**Recomendacion:** si el evaluador oficial exige `p.x`, hagan atributos publicos en el core del proyecto y documenten la divergencia. Si no, vuelvan al modelo privado.

---

### 16.8 `is` entre tipos hermanos

**Estado actual:** el evaluador acepta `new Dog(...) is Cat` y devuelve `false`; el type checker lo rechaza como no plausible; el backend tolera el diagnostico.

**Teoria HULK:** `is` comprueba si el tipo dinamico conforma con un tipo estatico. Los ejemplos de HULK muestran chequeos dinamicos con posibles resultados `false`.

**Evaluacion:** el type checker parece demasiado restrictivo para `is`.

**Recomendacion:** permitir `is` siempre que el tipo objetivo exista y que el operando sea un valor de tipo compatible con objetos. La operacion debe devolver `true` o `false`. Reservar restricciones fuertes para `as`, porque `as` puede producir error runtime si el downcast no es valido.

---

### 16.9 `for/range` parcialmente modelado pero no ejecutable

**Estado actual:** existen nodos y simbolos relacionados con `for`/`range`, pero evaluador/runtime reportan no soportado; el backend tambien lo deja fuera.

**Teoria/documentacion:** `for` se desazucara a `while` sobre iterables; `range` produce un iterable.

**Evaluacion:** pendiente de implementacion. No es contradiccion si se declara fuera de alcance, pero si el requisito del proyecto exige todos los features hasta cierta seccion, puede convertirse en incumplimiento.

**Correccion:**

1. Implementar `Range` e `Iterable` en runtime/evaluador.
2. Definir lowering:
   ```hulk
   for (x in range(a, b)) body
   ```
   a:
   ```hulk
   let iterable = range(a, b) in
   while (iterable.next())
       let x = iterable.current() in body
   ```
3. Luego bajar ese azucar a IR.

---

### 16.10 `base()` con varias representaciones

**Estado actual:** existe `BaseCall`, pero el resolver tambien trata `FunctionCall("base")` como especial; no hay test real que cierre el contrato.

**Teoria:** `base` en un metodo refiere a la implementacion del padre o ancestro mas cercano.

**Evaluacion:** inconsistencia interna de AST/resolver/parser/backend.

**Correccion:** escoger una representacion canonica.

Recomendacion:

```text
Parser produce FunctionCall("base") porque self no es keyword y base puede tratarse como simbolo especial.
Resolver lo marca como SyntheticKind::Base.
IRGen lo baja a CALL estatico Parent.method(self, args).
```

o:

```text
Parser produce BaseCall.
Resolver valida contexto.
IRGen baja BaseCall a CALL estatico Parent.method(self, args).
```

Lo importante es que exista una sola convencion.

---

### 16.11 `self` como nodo AST vs `VariableReference("self")`

**Estado actual:** existe clase `SelfRef`, pero el parser real produce `VariableReference(name=self)`.

**Teoria HULK:** `self` no es necesariamente keyword; se comporta como simbolo especial dentro de metodos y puede ser ocultado por scopes internos.

**Evaluacion:** usar `VariableReference("self")` puede estar mas alineado con HULK que `SelfRef`, pero el AST y el resolver deben decidir una sola forma.

**Correccion:** si `self` es identificador especial, eliminen la dependencia de `SelfRef` en backend y documenten que el resolver produce un simbolo sintetico `Self`.

---

### 16.12 Documentacion menciona features fuera del core actual

**Estado actual:** protocolos, vectores, lambdas/functores y extensiones aparecen en documentos o nodos, pero no estan completos en backend.

**Teoria/orientacion:** el proyecto exige implementar el core hasta verificacion de tipos y al menos una extension sintactico-semantica adicional.

**Evaluacion:** riesgo de alcance. No es contradiccion si se separa claramente:
- core implementado;
- extensiones planificadas;
- extensiones fuera de alcance.

**Correccion:** el informe final debe tener una matriz de features.

---

### 16.13 `grammar.y` y parser generado no alineados

**Estado actual:** se reporta que `grammar.y` menciona tokens no expuestos de forma equivalente por lexer/adapter, pero la suite pasa porque se usa el parser generado existente.

**Evaluacion:** riesgo operacional.

**Correccion:** no regenerar parser dentro del hito de IR. Abrir una tarea independiente para alinear lexer, grammar, adapter y parser generado.

---

### 16.14 Valores dinamicos vs "todo es numero"

**Estado actual:** el backend usa `hulk_rt::Value` como variante dinamica.

**Teoria BANNER/CIL:** todos los valores son enteros/punteros de 32 bits, y su significado depende del uso.

**Evaluacion:** no es contradiccion si ustedes declaran que su primera IR es de nivel medio. Si afirman que implementan BANNER/CIL bajo, entonces si contradice.

**Correccion:** nombrar la IR como `HulkIR` o `HIR`, no como BANNER puro, salvo que adopten el modelo de valores bajos.

---

## 17. Recomendacion tecnica final

La ruta mas segura es:

```text
1. Mantener el runtime C++ actual.
2. Agregar HulkIR explicita.
3. Reemplazar CppCodegen directo por IRToCppEmitter.
4. Agregar --emit-ir.
5. Alinear semantica de atributos e is.
6. Solo despues considerar VM o layout por slots.
```

No migrar a ensamblador.  
No migrar a LLVM ahora.  
No tirar el backend C++20.

La arquitectura final defendible seria:

```text
Frontend:
  lexer -> parser -> AST

Semantica:
  binding -> inferencia -> type checking

Backend:
  AST tipado -> HulkIR -> C++20 -> g++ -> ejecutable

Runtime:
  hulk_runtime.hpp con valores, objetos, tipos, metodos, is/as y builtins
```

---

## 18. Checklist de implementacion

### IR basica

- [ ] Crear `src/ir/ir.h`.
- [ ] Crear `IRProgram`, `IRType`, `IRData`, `IRFunction`, `IRInstr`.
- [ ] Crear `IROp`.
- [ ] Crear `IRPrinter`.
- [ ] Crear tests de impresion de IR manual.

### IRGen

- [ ] `IRGen::generate(Program&)`.
- [ ] `lower_expr`.
- [ ] `lower_function_decl`.
- [ ] `lower_type_decl`.
- [ ] Tabla de temporales.
- [ ] Tabla de labels.
- [ ] Contexto de scopes.
- [ ] Mapeo de simbolos resueltos a locales IR.
- [ ] Soporte C4: literales, ops, builtins, bloques, let, if, while.
- [ ] Soporte C5: funciones, llamadas, recursividad, asignacion.
- [ ] Soporte C6: tipos, new, init, campos, metodos, self, vcall, is, as, base.

### IRToCppEmitter

- [ ] Emitir includes y runtime.
- [ ] Emitir forward declarations.
- [ ] Emitir funciones.
- [ ] Emitir registro de tipos desde `.TYPES`.
- [ ] Emitir `main`.
- [ ] Traducir cada `IROp`.
- [ ] Generar C++ compilable con `g++ -std=c++20 -Isrc`.

### CLI

- [ ] `--emit-ir`.
- [ ] `--emit-cpp`.
- [ ] default: compilar a ejecutable.
- [ ] opcional: `--legacy-cpp`.

### Tests

- [ ] Golden tests de IR.
- [ ] Tests de C4 via `AST -> IR -> C++`.
- [ ] Tests de C5 via `AST -> IR -> C++`.
- [ ] Tests de C6 via `AST -> IR -> C++`.
- [ ] Test real de `base()`.
- [ ] Test de `is` entre tipos no relacionados.
- [ ] Test de atributos segun la semantica elegida.
- [ ] Test de shadowing en `let`.
- [ ] Test de dispatch polimorfico.

---

## 19. Ejemplo completo de transformacion

### HULK

```hulk
type Animal(name) {
    name = name;
    speak() => print("...");
}

type Dog(name) inherits Animal(name) {
    speak() => print("Woof " @ self.name);
}

let a: Animal = new Dog("Rex") in a.speak();
```

### IR esperada

```text
.TYPES
type Animal parent Object {
  field Animal_name : Object slot 0
  method speak -> Animal_speak slot 0
}

type Dog parent Animal {
  field Animal_name : Object slot 0
  method speak -> Dog_speak slot 0
}

.DATA
s0 = "..."
s1 = "Woof "
s2 = "Rex"

.CODE
function Animal.__init__(self, name) {
  setfield self Animal_name name
  return self
}

function Animal_speak(self) {
  t0 = load_data s0
  t1 = builtin_print(t0)
  return t1
}

function Dog.__init__(self, name) {
  t0 = call Animal.__init__(self, name)
  return self
}

function Dog_speak(self) {
  t0 = load_data s1
  t1 = getfield self Animal_name
  t2 = concat t0 t1
  t3 = builtin_print(t2)
  return t3
}

function hulk_main() {
  t0 = new_object Dog
  t1 = load_data s2
  t2 = call Dog.__init__(t0, t1)
  a_0 = move t0
  t3 = vcall a_0 speak()
  return t3
}
```

Esto muestra todas las piezas academicas importantes:

- tipos y herencia aplanados;
- strings en `.DATA`;
- codigo lineal;
- `self` explicito;
- constructor explicito;
- acceso a campo explicito;
- dispatch virtual explicito.

---

## 20. Texto sugerido para la defensa

Pueden explicar la decision asi:

> Inicialmente implementamos un backend source-to-source a C++20 para cerrar ejecucion del core C4-C6 con un runtime propio. Luego introdujimos una representacion intermedia propia, HulkIR, para alinear el backend con la arquitectura del curso. El pipeline final no genera C++ directamente desde el AST: primero baja el AST semantico a una IR lineal con temporales, labels, funciones, tipos, campos y llamadas virtuales; despues un emisor traduce esa IR a C++20. Usamos `g++` como backend nativo final. Esta decision conserva portabilidad y evita la complejidad innecesaria de ensamblador directo, manteniendo una frontera clara entre frontend, IR, backend y runtime.

Y si preguntan por que no ensamblador:

> La generacion directa de ensamblador exige resolver registros, stack frames, ABI, heap, layout fisico y liberacion de memoria al mismo tiempo. Las conferencias recomiendan justamente introducir una IR para desacoplar esos problemas. En nuestro caso, C++20 funciona como target portable mientras la IR propia captura el lowering academico relevante.

---

## 21. Conclusion

La implementacion actual es recuperable y valiosa. No hay que descartarla.

Pero para alinearla con el plan y las conferencias, debe dejar de ser:

```text
AST -> C++20
```

y pasar a ser:

```text
AST -> HulkIR -> C++20
```

Esa modificacion resuelve la critica principal sin obligar a implementar ensamblador ni LLVM. Ademas, crea una base limpia para futuras mejoras: VM propia, optimizaciones, layout por slots, vtables reales, string pool, GC o backend LLVM.

El foco inmediato debe ser:

1. agregar IR;
2. imprimir IR con `--emit-ir`;
3. generar C++ desde IR;
4. alinear semantica de atributos e `is`;
5. agregar tests reales para `base()`;
6. documentar que C++20 es target portable, no sustituto conceptual de la IR.
