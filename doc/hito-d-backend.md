# Informe de implementación del Hito D: Backend

## Objetivo

Este informe propone cómo pasar del estado actual del compilador HULK a una implementación ordenada del **Hito D: Backend**. La idea central es no empezar el backend como una pieza aislada, sino montarlo sobre lo que ya está sólido en el proyecto: lexer, parser, AST, diagnósticos, análisis semántico, inferencia, type checking y evaluador.

La recomendación principal es implementar primero un backend **source-to-source hacia C++20**, con un runtime pequeño propio para HULK. Esta ruta permite generar ejecutables nativos con `g++`, reutilizar el modelo dinámico que ya validó el evaluador y mantener bajo control la complejidad de objetos, herencia, `self`, `base`, `is`, `as`, scopes y builtins.

## Estado actual del proyecto

### Componentes ya disponibles

El repositorio ya tiene una base bastante madura:

| Componente | Ruta | Estado |
|---|---|---|
| Lexer | `src/lexer/` | Implementado |
| Parser Bison | `src/parser/` | Implementado y usado por tests |
| AST | `src/ast/` | Implementado con nodos concretos y visitor |
| Diagnósticos | `src/common/`, `lib/es_errors.json` | Implementado |
| Evaluador | `src/eval/` | Implementa cortes 4, 5 y 6 para el subconjunto probado |
| Runtime dinámico | `src/objects/` | `HulkValue`, `HulkObject`, `TypeDef`, `MethodDef` |
| Binding | `src/binding/` | Resolución de símbolos y scopes estáticos |
| Semántica | `src/semantic/` | Tablas de tipos/funciones y chequeos globales |
| Inferencia | `src/inference/` | `HulkType`, `type_map`, refinamiento iterativo |
| Type checking | `src/typecheck/` | Conformidad, argumentos, retornos, casts, overrides |
| Backend | `src/backend/backend.cpp` | Vacío |

También existe un runner unificado en `tests/run_tests.sh` y targets en `Makefile` para `eval`, `semantic`, `parser-demo` y `run-tests`.

### Verificación realizada

Se ejecutó:

```bash
make run-tests
```

Resultado:

```text
Total  : 76
Passed : 76
Failed : 0
```

Esto confirma que el estado actual es una buena base para backend. El backend debe apoyarse en ese pipeline, no duplicarlo.

## Pipeline recomendado para Hito D

El flujo del compilador con backend debería quedar así:

```text
archivo .hulk
   |
   v
Lexer
   |
   v
Parser
   |
   v
AST: Hulk::Program
   |
   v
SemanticAnalyzer
   |-- SymbolResolver
   |-- TypeInferencer
   |-- TypeChecker
   |
   v
Backend
   |-- Lowering/Codegen
   |-- Runtime HULK
   |-- Emisión C++20
   |
   v
g++
   |
   v
ejecutable nativo
```

El punto importante: **el backend solo corre si el parseo y el análisis semántico no reportan errores**. Si hay errores, se imprimen con `DiagnosticEngine` igual que hoy y no se genera código.

## Decisión de target

### Alternativas

| Opción | Ventajas | Costos/Riesgos |
|---|---|---|
| Generar ensamblador directo | Control total, backend "puro" | Muy costoso para objetos, strings, heap, dispatch y builtins |
| Generar LLVM IR | Backend serio, optimizable | Añade dependencia grande y curva de integración |
| Generar C | Portable y simple de compilar | Runtime de objetos y strings más incómodo |
| Generar C++20 | Encaja con el repo, permite `std::variant`, `std::shared_ptr`, `std::string`, lambdas auxiliares | Es menos "bajo nivel", pero muy efectivo como primer backend |
| Bytecode propio | Controlado y testeable | Sería más un segundo intérprete que un backend nativo |

### Recomendación

Implementar primero **emisión C++20**.

Motivos:

- El proyecto ya compila con `g++ -std=c++20`.
- El evaluador ya validó una semántica dinámica basada en `HulkValue`, objetos con campos, tablas de métodos y scopes.
- El backend puede compartir conceptos con `src/eval/` sin copiar literalmente el evaluador.
- Se puede verificar comparando la salida del ejecutable generado contra los `.expected` actuales.
- Evita introducir LLVM antes de cerrar la semántica del lenguaje.

Esta decisión no impide pasar a LLVM después. Si se diseña una capa intermedia ligera, el emisor C++ puede ser el primer target y LLVM el segundo.

## Arquitectura propuesta

Crear una estructura nueva bajo `src/backend/`:

```text
src/backend/
  backend.cpp              # punto de integración actual o fachada
  backend.h
  backend_driver.cpp       # parsea, analiza, genera, invoca compilador si aplica
  backend_driver.h
  codegen_cpp.cpp          # visitor que emite C++20
  codegen_cpp.h
  codegen_context.cpp      # estado: nombres, scopes, tablas, type_map
  codegen_context.h
  name_mangler.cpp         # nombres válidos y únicos para C++
  name_mangler.h
  runtime/
    hulk_runtime.hpp       # runtime incluido por el C++ generado
```

Opcionalmente, si se quiere separar mejor:

```text
src/backend/ir/
  hulk_ir.h
  lowering.cpp
  lowering.h
```

Para una primera versión, se puede hacer AST -> C++ directamente con visitor. Si luego aparece demasiada complejidad, se introduce IR sin romper la CLI.

## Integración con el Makefile

Agregar targets:

```make
BACKEND_SRCS := \
    src/backend/backend.cpp \
    src/backend/backend_driver.cpp \
    src/backend/codegen_cpp.cpp \
    src/backend/codegen_context.cpp \
    src/backend/name_mangler.cpp

BACKEND_OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(BACKEND_SRCS))

backend: $(LEXER_AST_OBJS) $(PARSER_OBJS) $(SEMANTIC_OBJS) $(BACKEND_OBJS)
	@mkdir -p $(OBJDIR)/backend_main
	$(CXX) $(CXXFLAGS) -c src/backend/main.cpp -o $(OBJDIR)/backend_main/main.o
	$(CXX) $(CXXFLAGS) \
		$(LEXER_AST_OBJS) $(PARSER_OBJS) $(SEMANTIC_OBJS) $(BACKEND_OBJS) \
		$(OBJDIR)/backend_main/main.o \
		-o hulk_backend

backend-tests: backend
	bash tests/backend/run_backend_tests.sh
```

La CLI mínima:

```bash
./hulk_backend archivo.hulk -o programa
./hulk_backend archivo.hulk --emit-cpp -o programa.cpp
```

Comportamiento recomendado:

- `-o programa`: genera C++ temporal, compila con `g++` y produce ejecutable.
- `--emit-cpp -o programa.cpp`: solo emite C++ para depuración.
- `--keep-temp`: conserva el `.cpp` generado cuando también se compila.

## Runtime recomendado

La primera versión del backend debe usar un runtime dinámico, muy parecido al evaluador:

```cpp
namespace hulk_rt {
    struct Nil {};

    struct Object;
    using ObjectPtr = std::shared_ptr<Object>;

    using Value = std::variant<Nil, double, std::string, bool, ObjectPtr>;

    struct Object {
        std::string type_name;
        std::unordered_map<std::string, Value> fields;
    };

    struct Method {
        std::function<Value(ObjectPtr, const std::vector<Value>&)> fn;
        std::size_t arity;
    };

    struct TypeInfo {
        std::string name;
        std::string parent;
        std::unordered_map<std::string, Method> methods;
    };
}
```

Funciones mínimas del runtime:

- `to_string(Value)`.
- `truthy(Value)`.
- `as_number(Value)`, `as_bool(Value)`, `as_string(Value)`, `as_object(Value)`.
- `hulk_print(Value)`.
- `hulk_sqrt`, `hulk_sin`, `hulk_cos`, `hulk_exp`, `hulk_log`, `hulk_rand`.
- `is_instance(Value, std::string type_name)`.
- `checked_cast(Value, std::string type_name)`.
- `get_field(ObjectPtr, std::string field)`.
- `set_field(ObjectPtr, std::string field, Value value)`.
- `call_method(ObjectPtr self, std::string method_name, std::vector<Value> args)`.

La razón de mantener `Value` dinámico al inicio es pragmática: el type checker ya evita muchos errores, pero el evaluador actual todavía define detalles de runtime como `truthy`, formato de `print`, división por cero, cast inválido y acceso a miembros. El backend debe reproducir esos comportamientos antes de optimizar.

## Uso de la información semántica existente

El backend debe recibir:

```cpp
Hulk::Program& program;
const Hulk::SemanticTables& tables;
const std::unordered_map<Hulk::Expr*, Hulk::ResolutionResult>& resolution_map;
const std::unordered_map<Hulk::Expr*, Hulk::HulkType>& type_map;
```

Fuente:

```cpp
Hulk::SemanticAnalyzer sem(engine);
bool ok = sem.analyze(*program);
```

Uso concreto:

- `tables`: generar constructores, métodos, jerarquía, aridades, padres y atributos.
- `resolution_map`: saber a qué binding/param/símbolo corresponde cada referencia.
- `type_map`: elegir conversiones, validar operaciones especiales y, más adelante, desboxear valores primitivos.
- `DiagnosticEngine`: reportar nodos no soportados o errores internos de backend.

El backend no debe volver a resolver nombres buscando strings manualmente. Esa información ya existe.

## Modelo de nombres

Necesitamos un `NameMangler` para evitar colisiones con C++ y con nombres repetidos por scopes:

```text
function fib      -> hulk_fn_fib
type Point        -> hulk_type_Point
method Point.move -> hulk_method_Point_move
local x           -> hulk_local_17_x
param n           -> hulk_param_3_n
```

Para variables locales, el backend debe mapear símbolos semánticos a nombres C++:

```text
VariableBinding*     -> nombre C++ único
const Param*         -> nombre C++ único
SyntheticSymbol*self -> nombre C++ de self
SyntheticSymbol*for  -> nombre C++ de variable de for
```

Esto evita errores con shadowing:

```hulk
let x = 1 in let x = 2 in x
```

En C++ deben existir dos nombres distintos.

## Estrategia de emisión de expresiones

Como HULK trata casi todo como expresión, conviene emitir C++ usando lambdas inmediatas para los casos que requieren varias instrucciones:

```cpp
auto value = [&]() -> hulk_rt::Value {
    hulk_rt::Value x = hulk_rt::Value(10.0);
    return hulk_rt::add(x, hulk_rt::Value(1.0));
}();
```

El `CodegenCpp` puede tener dos modos:

- `emit_expr(Expr*) -> std::string`: devuelve una expresión C++.
- `emit_stmt_expr(Expr*) -> std::string`: devuelve una lambda C++ cuando el nodo necesita bloque.

Nodos simples pueden ser expresiones directas:

| Nodo | Emisión |
|---|---|
| `Number` | `hulk_rt::Value(1.0)` |
| `String` | `hulk_rt::Value(std::string("..."))` |
| `Boolean` | `hulk_rt::Value(true)` |
| `VariableReference` | nombre C++ resuelto |
| `ArithmeticBinOp` | `hulk_rt::add(left, right)` |
| `StringBinOp` | `hulk_rt::concat(left, right, with_space)` |
| `LogicUnaryOp` | `hulk_rt::logical_not(expr)` |
| `BuiltinCall` | helper correspondiente |

Nodos compuestos deberían emitirse como lambda:

| Nodo | Razón |
|---|---|
| `LetIn` | declara variables y devuelve body |
| `ExprBlock` | evalúa varias expresiones y devuelve la última |
| `IfStmt` | ramas con retorno de valor |
| `WhileStmt` | loop con último valor |
| `FunctionCall` | puede evaluar args en orden |
| `MethodCall` | necesita receiver, args, dispatch |
| `NewExpr` | construye objeto e inicializa atributos |
| `BaseCall` | requiere contexto de método actual |

## Declaraciones globales

### Funciones

Cada `FunctionDecl` se genera como:

```cpp
hulk_rt::Value hulk_fn_fib(const std::vector<hulk_rt::Value>& args) {
    hulk_rt::Value hulk_param_0_n = args[0];
    return ...;
}
```

Luego se registra en una tabla:

```cpp
hulk_rt::register_function("fib", hulk_fn_fib, 1);
```

Para llamadas directas se puede emitir:

```cpp
hulk_fn_fib({arg0})
```

La tabla de funciones sigue siendo útil para debug y para unificar con llamadas dinámicas si se agregan lambdas/functores más adelante.

### Tipos

Cada `TypeDecl` debe producir:

- Una función constructora.
- Funciones para cada método.
- Registro de `TypeInfo`.
- Información de padre.
- Lista de atributos inicializables.

Ejemplo conceptual:

```cpp
hulk_rt::ObjectPtr hulk_ctor_Point(const std::vector<hulk_rt::Value>& args) {
    auto self = std::make_shared<hulk_rt::Object>("Point");
    hulk_rt::Value x = args[0];
    hulk_rt::Value y = args[1];
    self->fields["x"] = x;
    self->fields["y"] = y;
    return self;
}

hulk_rt::Value hulk_method_Point_norm(
    hulk_rt::ObjectPtr self,
    const std::vector<hulk_rt::Value>& args
) {
    return ...;
}
```

Para herencia, la inicialización debe seguir la misma semántica del evaluador:

1. Crear scope de constructor del hijo.
2. Evaluar los argumentos del padre en ese scope.
3. Inicializar atributos del padre.
4. Inicializar atributos propios.
5. Mantener `self` disponible para inicializadores si el lenguaje lo permite en el punto correspondiente. Actualmente semántica restringe `self` en inicializadores de atributo.

## Despacho de métodos

Primera versión recomendada: despacho dinámico por nombre y tipo.

```cpp
hulk_rt::Value hulk_rt::call_method(
    ObjectPtr self,
    const std::string& method_name,
    const std::vector<Value>& args
);
```

`call_method` busca:

1. Método en el tipo concreto.
2. Si no está, método en el padre.
3. Continúa hasta `Object`.
4. Si no existe, error runtime.

Esto replica el evaluador y facilita `override`.

Más adelante se puede optimizar a vtables con índices:

```text
method_index("area") = 3
obj->vtable[3](obj, args)
```

Pero no es necesario para cerrar Hito D.

## Manejo de scopes

Para la primera versión, usar variables C++ locales generadas. No hace falta una tabla dinámica de scopes en runtime para el caso común, porque:

- Las funciones globales no capturan el scope del caller.
- Los métodos reciben `self` explícito.
- `let` y bloques pueden convertirse en lambdas C++.
- `:=` actualiza la variable C++ correspondiente.

Ejemplo HULK:

```hulk
let x = 1 in {
    x := x + 1;
    x;
}
```

Emisión conceptual:

```cpp
([&]() -> hulk_rt::Value {
    hulk_rt::Value hulk_local_1_x = hulk_rt::Value(1.0);
    return ([&]() -> hulk_rt::Value {
        hulk_local_1_x = hulk_rt::add(hulk_local_1_x, hulk_rt::Value(1.0));
        return hulk_local_1_x;
    })();
})()
```

Para que esto funcione bien, `CodegenContext` mantiene una pila de scopes:

```cpp
struct ScopeFrame {
    std::unordered_map<const void*, std::string> symbols;
};
```

Las llaves pueden ser `VariableBinding*`, `Param*` o `SyntheticSymbol*`.

## Nodo por nodo: plan de soporte

### Corte 4: expresiones básicas

| Nodo | Backend |
|---|---|
| `Number`, `String`, `Boolean` | Emitir `hulk_rt::Value` |
| `ArithmeticBinOp` | Helpers `add`, `sub`, `mul`, `div`, `mod`, `pow` |
| `ArithmeticUnaryOp` | Helper `neg` |
| `LogicBinOp` | Helpers para comparación; `and/or` con corto circuito |
| `LogicUnaryOp` | Helper `logical_not` |
| `StringBinOp` | `concat` y `space_concat` |
| `Print` | `hulk_rt::print(value)` y devuelve el valor |
| `BuiltinCall` | `sqrt`, `sin`, `cos`, `exp`, `log`, `rand` |
| `ExprBlock` | Lambda con valor de última expresión |
| `Group` | Emitir expresión interna |
| `IfStmt` | Lambda con `if/else` y `return` por rama |
| `WhileStmt` | Lambda con acumulador de último valor |

### Corte 5: variables y funciones

| Nodo | Backend |
|---|---|
| `VariableBinding` | Declaración local con nombre mangled |
| `VariableReference` | Nombre desde `resolution_map` |
| `LetIn` | Lambda con scope nuevo |
| `DestructiveAssign` | Asignación al símbolo resuelto |
| `FunctionDecl` | Función C++ global generada |
| `FunctionCall` | Llamada directa si resuelta, helper si builtin |
| `Lambda` | Reportar no soportado por ahora |

### Corte 6: objetos y herencia

| Nodo | Backend |
|---|---|
| `TypeDecl` | Constructor, métodos y registro de tipo |
| `TypeMemberAttribute` | Inicialización dentro del constructor |
| `TypeMemberMethod` | Función C++ con `self` explícito |
| `NewExpr` | Llamada a constructor generado |
| `MemberAccess` | `hulk_rt::get_field(obj, name)` |
| `DestructiveAssignMember` | `hulk_rt::set_field(obj, name, value)` |
| `MethodCall` | `hulk_rt::call_method(obj, method, args)` |
| `SelfRef` | Símbolo sintético `self` |
| `BaseCall` | Llamada al método padre del método actual |
| `IsExpr` | `hulk_rt::is_instance(value, type)` |
| `AsExpr` | `hulk_rt::checked_cast(value, type)` |

### Casos con decisión pendiente

| Caso | Estado actual | Propuesta |
|---|---|---|
| `for` | Existe AST y binding sintético; evaluador lo reporta no soportado | Implementarlo en backend si se cierra `range`/iterables |
| `range` | Está en `BuiltinFunc::Range` y en tablas semánticas, pero lexer/parser no lo tratan como builtin dedicado | Soportarlo como `FunctionCall` builtin resuelto por semántica |
| Lambdas | AST existe; evaluador e inferencia lo dejan fuera del core | Reportar `BACKEND_UNSUPPORTED` hasta que el front lo soporte completo |
| Protocolos | AST existe; `SymbolResolver::visit(ProtocolDecl)` es no-op | No incluir en Hito D salvo requisito explícito |
| `auto` y `_` | Inferencia los entiende, pero `grammar.y` menciona tokens que no aparecen en lexer/parser adapter | Alinear grammar/lexer antes de regenerar parser |

## Orden de implementación recomendado

### Fase 0: contrato del backend

Objetivo: tener CLI y flujo completo sin generar todavía todo el lenguaje.

Tareas:

- Crear `src/backend/main.cpp`.
- Crear `BackendDriver`.
- Reutilizar lectura de archivo de `src/eval/main.cpp` y `src/semantic/main_semantic.cpp`.
- Parsear a `Program`.
- Ejecutar `SemanticAnalyzer`.
- Si hay errores, imprimir y terminar.
- Agregar target `backend`.
- Generar un `.cpp` mínimo con `int main() { return 0; }`.

Cierre:

```bash
make backend
./hulk_backend tests/eval/c4_literals.hulk --emit-cpp -o /tmp/hulk_out.cpp
```

### Fase 1: runtime mínimo y literales

Objetivo: compilar programas con literales y `print`.

Tareas:

- Crear `hulk_runtime.hpp`.
- Implementar `Value`, `Nil`, `to_string`, `truthy`, `print`.
- Emitir `Number`, `String`, `Boolean`, `Print`, `ExprBlock`.
- Generar `main` que evalúa la expresión global.

Tests iniciales:

- `tests/eval/c4_literals.hulk`
- `tests/eval/c4_strings.hulk` parcialmente si ya está concat

### Fase 2: operaciones y builtins

Objetivo: cubrir expresiones puras.

Tareas:

- Helpers aritméticos con chequeos runtime.
- División y módulo por cero con mensaje compatible.
- Helpers lógicos y comparaciones.
- Concatenación `@` y `@@`.
- Builtins matemáticos.
- `if`, `while`, bloques.

Tests:

- Todos los `c4_*.hulk`, excepto cualquier caso que dependa de `for/range` si se mantiene fuera.

### Fase 3: variables, let y funciones

Objetivo: cubrir corte 5 compilado.

Tareas:

- `CodegenContext` con scopes.
- `NameMangler`.
- Soporte de `VariableBinding`, `VariableReference`, `LetIn`.
- `DestructiveAssign`.
- Emisión de `FunctionDecl`.
- Llamadas recursivas.
- Aridad validada antes por semántica; mantener asserts defensivos en runtime.

Tests:

- `c5_variables.hulk`
- `c5_let_shadowing.hulk`
- `c5_functions.hulk`
- `c5_recursion.hulk`

### Fase 4: objetos, métodos y herencia

Objetivo: cubrir corte 6 compilado.

Tareas:

- Registro de tipos.
- Constructores generados.
- Inicialización padre -> hijo.
- Campos dinámicos.
- Métodos con `self`.
- Dispatch por herencia.
- `base()`.
- `is` y `as`.
- Asignación destructiva a miembros.

Tests:

- `c6_objects_basic.hulk`
- `c6_inheritance.hulk`
- `c6_member_assign.hulk`
- `c6_base_call.hulk`
- `c6_base_method.hulk`

### Fase 5: negativos y diagnósticos

Objetivo: garantizar que programas inválidos no generan ejecutables incorrectos.

Tareas:

- Si `SemanticAnalyzer` reporta errores, no emitir C++.
- Agregar error `BACKEND_UNSUPPORTED` o usar `report_raw`.
- Tests de error que esperan fallo antes de codegen.
- Verificar mensajes de runtime para errores que hoy pertenecen al evaluador.

Tests:

- Reutilizar `tests/eval/err_*.hulk`.
- Reutilizar `tests/semantic/err_*.hulk`.
- Reutilizar `tests/typecheck/*.hulk`.

### Fase 6: `for` y `range`

Objetivo: cerrar la brecha entre parser/semántica y runtime.

Actualmente hay pruebas semánticas para `for`, pero el evaluador reporta `for` como no soportado. El backend puede resolverlo si implementa un runtime mínimo de iterables:

```cpp
struct Range {
    double current;
    double end;
};
```

O, manteniendo `Value`, agregar:

```cpp
struct Iterable {
    std::vector<Value> values;
};
```

Recomendación inicial:

- Implementar `range(a, b)` como vector de números `[a, a+1, ..., b-1]`.
- Emitir `for (x in iterable)` como loop C++ sobre ese vector.
- Registrar la variable del for usando `SyntheticSymbol`.

Esto debe hacerse después de que el core C4-C6 compile correctamente.

## Tests de backend

Crear:

```text
tests/backend/
  run_backend_tests.sh
```

Estrategia:

1. Construir `hulk_backend`.
2. Para cada test válido de `tests/eval/c4_*.hulk`, `c5_*.hulk`, `c6_*.hulk`:
   - generar ejecutable temporal;
   - ejecutarlo;
   - comparar stdout/stderr con `tests/expected/eval/*.expected`.
3. Para tests inválidos:
   - comprobar que `hulk_backend` falla;
   - comparar salida con expected semántico cuando aplique;
   - no exigir C++ generado.

Ejemplo conceptual:

```bash
./hulk_backend tests/eval/c5_recursion.hulk -o /tmp/hulk_c5_recursion
/tmp/hulk_c5_recursion > /tmp/actual.txt 2>&1
diff -u tests/expected/eval/c5_recursion.expected /tmp/actual.txt
```

Agregar target:

```bash
make backend-tests
```

Criterio de cierre del Hito D:

```text
make run-tests        -> 76/76 como ahora
make backend-tests    -> todos los casos soportados pasan
```

## Compatibilidad de salida

El backend debe reproducir la salida visible del evaluador:

- `print` imprime un salto de línea.
- Números enteros se imprimen sin `.000000`.
- Objetos se imprimen como `<Type object>`.
- Booleanos como `true`/`false`.
- `nil` como `nil`.

Esto está definido actualmente por `HulkValue::to_string()`. Conviene copiar esa semántica al runtime del backend.

## Manejo de errores

Hay dos categorías:

### Errores antes de backend

Incluyen:

- Sintaxis inválida.
- Variable no declarada.
- Tipo no declarado.
- Aridad incorrecta.
- Ciclos de herencia.
- Errores de tipo.

Estos ya deben salir por:

```cpp
engine.print_all();
```

El backend no debe ejecutarse.

### Errores runtime en código generado

Incluyen:

- División por cero si no fue descartada estáticamente.
- Cast inválido.
- Acceso a campo inexistente por un caso dinámico.
- Método inexistente por un caso dinámico.

Recomendación:

- El runtime generado puede imprimir mensajes compatibles con `DiagnosticEngine`, aunque no tenga spans exactos.
- Cuando el AST tenga `span`, el backend puede incrustar línea/columna en llamadas runtime:

```cpp
hulk_rt::checked_div(left, right, hulk_rt::Span{line1, col1, line2, col2});
```

No es obligatorio para la primera versión, pero ayuda mucho a mantener calidad.

## Riesgos técnicos

### 1. Duplicar lógica semántica

Riesgo: que el backend vuelva a resolver nombres por string.

Mitigación: usar `resolution_map` y `SemanticTables`.

### 2. Shadowing incorrecto

Riesgo: dos variables HULK con el mismo nombre terminan siendo la misma variable C++.

Mitigación: `NameMangler` basado en punteros semánticos, no solo en texto.

### 3. Orden de evaluación

Riesgo: generar C++ que evalúe argumentos o subexpresiones en orden distinto.

Mitigación: para llamadas y nodos con efectos, emitir lambdas que guardan temporales en orden explícito.

### 4. `self` y `base`

Riesgo: `base()` llama al método incorrecto cuando hay varios niveles de herencia.

Mitigación: `CodegenContext` debe guardar `current_type` y `current_method` durante generación de métodos.

### 5. Parser generado vs `grammar.y`

Riesgo: el target `parser-gen` puede descubrir desalineaciones. En el árbol actual, `grammar.y` menciona `AUTO` y `UNDERSCORE_TYPE`, pero esos tokens no aparecen en el lexer ni en el adapter. La suite pasa porque usa el parser generado ya presente.

Mitigación: antes de cerrar Hito D, decidir si `auto` y `_` son parte del lenguaje soportado y alinear `grammar.y`, `token_kind.hpp`, `keywords.hpp` y `parser_lexer_adapter.cpp`.

### 6. `for`/`range`

Riesgo: semántica e inferencia conocen parte de `for`/`range`, pero el evaluador no lo ejecuta.

Mitigación: tratarlo como feature separada de Fase 6. No bloquear C4-C6 por esto.

### 7. Protocolos/lambdas/vectores

Riesgo: aparecen en reportes de AST antiguos o nodos parciales, pero no están integrados como lenguaje core.

Mitigación: declararlos explícitamente fuera de alcance inicial del backend y emitir `BACKEND_UNSUPPORTED` si aparecen.

## Entregables concretos

Al cerrar Hito D deberíamos tener:

```text
src/backend/
  main.cpp
  backend.cpp
  backend.h
  backend_driver.cpp
  backend_driver.h
  codegen_cpp.cpp
  codegen_cpp.h
  codegen_context.cpp
  codegen_context.h
  name_mangler.cpp
  name_mangler.h
  runtime/hulk_runtime.hpp

tests/backend/
  run_backend_tests.sh

Makefile
  target backend
  target backend-tests
```

Y estos comandos deberían funcionar:

```bash
make backend
./hulk_backend tests/eval/c4_arithmetic.hulk -o /tmp/c4_arithmetic
/tmp/c4_arithmetic

./hulk_backend tests/eval/c5_recursion.hulk --emit-cpp -o /tmp/c5_recursion.cpp
g++ -std=c++20 /tmp/c5_recursion.cpp -o /tmp/c5_recursion
/tmp/c5_recursion

make backend-tests
```

## Orden de trabajo por personas

Si el equipo se reparte el backend, una división limpia sería:

| Persona | Responsabilidad | Archivos principales |
|---|---|---|
| A | Driver, CLI, Makefile, tests backend | `backend_driver.*`, `main.cpp`, `Makefile`, `tests/backend/` |
| B | Runtime dinámico y builtins | `runtime/hulk_runtime.hpp` |
| C | Codegen de expresiones C4-C5 | `codegen_cpp.*`, `codegen_context.*`, `name_mangler.*` |
| D | Objetos, métodos, herencia, `base`, `is/as` | `codegen_cpp.*`, runtime de tipos |

Dependencia recomendada:

```text
A desbloquea build y CLI
B desbloquea ejecución
C desbloquea C4-C5
D desbloquea C6
```

## Secuencia mínima viable

Si se quiere avanzar de la forma más directa posible:

1. Crear `hulk_backend` que parsea y analiza.
2. Emitir C++ con runtime embebido.
3. Pasar `c4_literals`.
4. Pasar todos los `c4`.
5. Pasar todos los `c5`.
6. Pasar todos los `c6`.
7. Agregar `backend-tests`.
8. Implementar o declarar fuera de alcance `for/range`, lambdas y protocolos.

## Conclusión

El proyecto ya tiene suficiente infraestructura para empezar el Hito D sin rediseñar el compilador. La ruta más segura es:

```text
AST semánticamente validado
    -> codegen C++20
    -> runtime dinámico HULK
    -> g++
    -> ejecutable
```

Este enfoque aprovecha el evaluador como especificación de comportamiento, usa las tablas semánticas e inferencia existentes como fuente de verdad, y permite construir tests de backend comparando contra los `.expected` actuales. Una vez que el backend C++ pase C4-C6, se puede decidir si vale la pena optimizar con IR propia, vtables reales o LLVM.
