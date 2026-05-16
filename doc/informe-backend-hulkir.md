# Informe del backend HulkIR / C++20

Fecha: 2026-05-16

## 1. Resumen ejecutivo

El backend del proyecto HULK quedó migrado de un generador directo `AST -> C++20` a un pipeline con representación intermedia explícita:

```text
HULK -> Lexer/Parser -> AST -> SemanticAnalyzer -> HulkIR -> IRToCppEmitter -> C++20 -> g++ -> ejecutable
```

Esta migración cumple el objetivo principal del informe de IR: separar el frontend semántico del backend concreto. El C++20 ya no es la representación intermedia conceptual, sino un target portable generado desde HulkIR. La IR aparece como contrato interno entre el análisis semántico y la emisión final de código.

El backend actual puede considerarse una fase funcional final para el alcance ya validado del proyecto: C4-C6, type holes válidos actuales, ejecución de expresiones, variables, funciones, recursividad, objetos, métodos, herencia, `self`, `base`, `is` y `as`. No es una fase final para todo el lenguaje HULK completo, porque siguen fuera de alcance `for/range`, lambdas, protocolos, vectores, layout por slots real, VM propia y correcciones semánticas pendientes.

## 2. Estado actual del pipeline

La entrada principal del backend sigue siendo `BackendDriver::run`. El driver lee el archivo, ejecuta lexer/parser, valida semánticamente, genera HulkIR y luego decide si imprime IR, emite C++ o compila ejecutable. La secuencia concreta está en `src/backend/backend_driver.cpp:76-91`.

```text
SemanticAnalyzer
      |
      v
IRGen::generate
      |
      +--> IRPrinter        (--emit-ir)
      |
      +--> IRToCppEmitter   (--emit-cpp o ejecutable)
              |
              v
             g++
```

Citas principales:

- `BackendOptions` incluye `emit_ir`, `emit_cpp` y `keep_temp`: `src/backend/backend_driver.h:8-14`.
- `BackendResult` guarda rutas de IR, C++ y ejecutable: `src/backend/backend_driver.h:16-21`.
- El driver construye `IRGen` y llama `generate`: `src/backend/backend_driver.cpp:76-77`.
- Si `--emit-ir` está activo, usa `IRPrinter`: `src/backend/backend_driver.cpp:79-87`.
- Si no, usa `IRToCppEmitter`: `src/backend/backend_driver.cpp:90-91`.
- La compilación final se mantiene con `g++ -std=c++20 -Isrc`: `src/backend/backend_driver.cpp:133-137`.

## 3. CLI implementada

La CLI soporta los tres modos esperados:

```bash
./hulk_backend archivo.hulk --emit-ir -o salida.hir
./hulk_backend archivo.hulk --emit-cpp -o salida.cpp
./hulk_backend archivo.hulk -o ejecutable
```

El uso visible al usuario se actualizó en `src/backend/main.cpp:8-10`. El parser de argumentos reconoce `--emit-cpp`, `--emit-ir` y `--keep-temp` en `src/backend/main.cpp:23-41`.

También se agregó una validación para evitar pedir dos salidas incompatibles al mismo tiempo:

```text
--emit-ir y --emit-cpp no pueden usarse juntos
```

Esa validación está en `src/backend/main.cpp:44-48`.

## 4. Integración con el build

El `Makefile` ya no compila el viejo generador directo `codegen_cpp.*`. En su lugar compila:

- `src/ir/ir.cpp`
- `src/ir/ir_printer.cpp`
- `src/backend/ir_gen.cpp`
- `src/backend/ir_to_cpp.cpp`

La lista está en `Makefile:63-71`.

Además, se eliminaron `src/backend/codegen_cpp.cpp` y `src/backend/codegen_cpp.h`, por decisión de reemplazo directo. Esto evita tener dos backends compitiendo y deja una sola arquitectura defendible: `AST -> HulkIR -> C++20`.

## 5. Modelo de HulkIR

La IR está definida en `src/ir/ir.h`. El diseño es de nivel medio: todos los valores siguen representándose como `hulk_rt::Value`, pero las estructuras de alto nivel del AST se bajan a instrucciones lineales con temporales, labels, llamadas, objetos y operaciones explícitas.

### 5.1 Operaciones

El enum `IROp` define operaciones para:

- constantes y movimientos;
- aritmética;
- lógica;
- comparaciones;
- concatenación;
- labels y saltos;
- llamadas globales;
- objetos y campos;
- dispatch virtual;
- llamada estática a método padre;
- `is` y `as`;
- builtins matemáticos y `print`.

Cita: `src/ir/ir.h:10-56`.

Operaciones relevantes:

- `ConstNil`, `ConstNumber`, `ConstBool`, `LoadData`, `Move`.
- `Add`, `Sub`, `Mul`, `Div`, `Mod`, `Pow`, `Neg`.
- `Label`, `Jump`, `JumpIfTrue`, `JumpIfFalse`.
- `Call`, `Return`.
- `NewObject`, `GetField`, `DefineField`, `SetField`.
- `VCall`, `SCallMethod`.
- `IsType`, `AsType`.

### 5.2 Tipos, campos y métodos

La IR incluye metadatos de tipos:

- `IRField`: dueño, nombre fuente, nombre bajado, tipo y slot informativo.
- `IRMethod`: dueño, nombre, función emitida, aridad y slot informativo.
- `IRType`: nombre, padre, init, ctor, campos y métodos.

Cita: `src/ir/ir.h:65-88`.

Importante: los slots hoy son informativos. El runtime todavía accede a campos y métodos por nombre. Esto es suficiente para el backend actual, pero deja una ruta futura hacia layouts/vtables reales.

### 5.3 Datos y funciones

Los strings se guardan como `IRData` y las funciones como `IRFunction`.

Citas:

- `IRData`: `src/ir/ir.h:90-93`.
- `IRInstr`: `src/ir/ir.h:95-108`.
- `IRFunction`: `src/ir/ir.h:110-117`.
- `IRProgram`: `src/ir/ir.h:119-124`.

Cada función IR tiene:

- nombre bajado;
- nombre fuente;
- tipo de función (`Entry`, `Global`, `Initializer`, `Method`);
- parámetros;
- locales;
- cuerpo lineal de instrucciones.

## 6. Formato textual de IR

El printer emite tres secciones:

```text
.TYPES
.DATA
.CODE
```

La implementación está en `IRPrinter::print`, `src/ir/ir_printer.cpp:8-45`.

Detalles:

- `.TYPES` imprime tipo, padre, campos, métodos y slots: `src/ir/ir_printer.cpp:11-23`.
- `.DATA` imprime strings con labels: `src/ir/ir_printer.cpp:25-28`.
- `.CODE` imprime funciones, locales e instrucciones: `src/ir/ir_printer.cpp:30-41`.
- Las instrucciones de objetos, dispatch, casts y builtins se imprimen explícitamente: `src/ir/ir_printer.cpp:109-140`.

Ejemplo de salida real para objetos:

```text
.TYPES
type Box parent Object {
  field Box_x : Object slot 0
  method get -> hulk_method_Box_get_2 slot 0
}

.CODE
function hulk_main() ; kind=entry
{
  hulk_tmp_object_2 = new_object Box
  hulk_tmp_method_call_5 = vcall hulk_local_b_4.get()
  return hulk_tmp_print_6
}
```

Ese caso está protegido por snapshot en `tests/expected/backend_ir/c6_ir_object.hir`.

## 7. Generación de IR: `IRGen`

`IRGen` es el componente que baja el AST semántico a HulkIR. Hereda de `ExprVisitor` y `DeclVisitor`, como el generador anterior, pero ya no produce C++: produce `IRProgram`.

Cita de interfaz: `src/backend/ir_gen.h:29-35`.

Recibe:

- `SemanticTables`;
- `resolution_map`;
- `type_map`.

Cita: `src/backend/ir_gen.h:31-33`.

Actualmente `type_map_` se recibe para mantener la interfaz semántica completa, pero no se usa todavía en la generación (`src/backend/ir_gen.cpp:50-51`). Esto es una continuación natural: usar el mapa de tipos para enriquecer campos, casts, checks y diagnósticos de backend.

### 7.1 Orden de generación

`IRGen::generate` reinicia el estado, recoge declaraciones, emite metadatos de tipos, luego funciones globales, funciones de tipos y finalmente `hulk_main`.

Cita: `src/backend/ir_gen.cpp:50-76`.

El orden elegido evita referencias hacia funciones aún no nombradas, porque primero se generan nombres estables de funciones, inicializadores y métodos.

### 7.2 Recolección de nombres

`collect_declarations` asigna nombres únicos para:

- funciones globales;
- tipos;
- inicializadores;
- constructores conceptuales;
- métodos.

Cita: `src/backend/ir_gen.cpp:79-98`.

Aunque el constructor como función separada ya no es necesario en el C++ emitido, se conserva `ctor_name` en la IR para mantener el modelo de tipos completo y extensible.

### 7.3 Metadatos de tipos

`emit_type_metadata` crea `IRType` con:

- `name`;
- `parent`;
- `init_name`;
- `ctor_name`;
- campos;
- métodos;
- slots informativos.

Cita: `src/backend/ir_gen.cpp:100-135`.

Los nombres bajados de campos se calculan como `Tipo_atributo`, por ejemplo `Box_x`: `src/backend/ir_gen.cpp:119`.

### 7.4 Funciones globales

Cada `FunctionDecl` baja a `IRFunctionKind::Global`. Sus parámetros se registran como parámetros IR y el cuerpo se baja como expresión. La función termina con `Return`.

Cita: `src/backend/ir_gen.cpp:281-292`.

### 7.5 Inicializadores de tipos

Cada tipo genera una función init:

```text
function hulk_init_Type(self, args...)
```

La implementación:

- agrega `self`;
- registra parámetros del constructor;
- si hay padre, llama al init del padre;
- baja inicializadores de atributos;
- emite `DefineField`;
- retorna `self`.

Cita: `src/backend/ir_gen.cpp:304-365`.

La llamada al padre está en `src/backend/ir_gen.cpp:320-345`. La inicialización de campos propios está en `src/backend/ir_gen.cpp:348-360`.

### 7.6 Métodos

Cada método se baja a una función IR `IRFunctionKind::Method`. El primer parámetro lógico es `hulk_self_value`.

Cita: `src/backend/ir_gen.cpp:368-393`.

Esto permite que `self` sea explícito en IR, y que el emisor C++ pueda producir funciones con firma:

```cpp
hulk_rt::Value method(hulk_rt::ObjectPtr self, const std::vector<hulk_rt::Value>& args)
```

### 7.7 Variables, scopes y shadowing

`IRGen` usa `CodegenContext` para asociar símbolos semánticos resueltos con nombres IR, no strings crudos. Esto preserva shadowing.

La resolución de referencias mira `resolution_map` y distingue:

- `Variable`;
- `Param`;
- `Synthetic` como `self`;
- otros casos no soportados.

Cita: `src/backend/ir_gen.cpp:440-463`.

El lowering de `let` empuja un scope, evalúa cada initializer en orden, registra el binding y luego baja el body. Esto soporta correctamente:

```hulk
let a = 7, a = a * 6 in a
```

Cita: `src/backend/ir_gen.cpp:604-615`.

### 7.8 Asignación destructiva

La asignación destructiva baja a:

1. bajar el valor;
2. copiarlo a un temporal resultado;
3. mover ese temporal al destino;
4. devolver el temporal resultado.

Cita: `src/backend/ir_gen.cpp:617-631`.

Esto es importante porque `x := expr` es una expresión y debe devolver el valor asignado en ese instante, no una referencia futura al local. Se agregó una regresión:

```hulk
let x = 0 in print((x := 2) + (x := 3));
```

Archivos:

- `tests/backend/regression/assign_value_capture.hulk`.
- `tests/expected/backend/assign_value_capture.expected`.

### 7.9 Control de flujo

El `if` baja a labels, saltos condicionales y un temporal resultado.

Cita: `src/backend/ir_gen.cpp:647-674`.

El `while` baja a:

- resultado inicial `nil`;
- label de inicio;
- condición;
- salto al final;
- body;
- actualización del resultado;
- salto al inicio;
- label final.

Cita: `src/backend/ir_gen.cpp:676-690`.

Esto cumple el objetivo académico de no depender de lambdas C++ inmediatas para expresar control de flujo en la IR.

### 7.10 Lógica con corto circuito

`&` y `|` bajan con saltos y labels, preservando corto circuito.

Cita: `src/backend/ir_gen.cpp:508-535`.

Las comparaciones (`==`, `!=`, `<`, `>`, `<=`, `>=`) bajan a instrucciones binarias normales: `src/backend/ir_gen.cpp:538-552`.

### 7.11 Funciones, builtins y constantes

Las llamadas globales usan `resolution_map` para encontrar la función declarada y luego emiten `Call`.

Cita: `src/backend/ir_gen.cpp:696-725`.

`print` baja a `BuiltinPrint`: `src/backend/ir_gen.cpp:731-740`.

Los builtins matemáticos (`sqrt`, `sin`, `cos`, `exp`, `log`, `rand`) bajan a operaciones IR específicas: `src/backend/ir_gen.cpp:742-760`.

Las constantes `PI` y `E` se bajan a números en `src/backend/ir_gen.cpp:580-595`.

### 7.12 Objetos, campos y métodos

`new Type(args)` baja a:

1. `NewObject Type`;
2. evaluación de argumentos;
3. `Call` al init del tipo;
4. resultado útil: el objeto creado.

Cita: `src/backend/ir_gen.cpp:791-817`.

Acceso a campo:

- `MemberAccess` baja a `GetField`: `src/backend/ir_gen.cpp:819-829`.

Asignación a campo:

- `DestructiveAssignMember` baja a `SetField`: `src/backend/ir_gen.cpp:633-645`.

Llamada virtual:

- `MethodCall` baja a `VCall`: `src/backend/ir_gen.cpp:831-842`.

### 7.13 `base()`

`base()` no baja a dispatch virtual. Baja a `SCallMethod`, con tipo inicial igual al padre del tipo actual.

Cita: `src/backend/ir_gen.cpp:418-438`.

Esto evita el problema clásico:

```text
vcall self.metodo()
```

que volvería a entrar en el override del hijo. La IR representa explícitamente que se desea llamar estáticamente a la implementación heredada.

### 7.14 `is` y `as`

`is` baja a `IsType`: `src/backend/ir_gen.cpp:844-854`.

`as` baja a `AsType`: `src/backend/ir_gen.cpp:856-866`.

La semántica runtime se delega a:

- `hulk_rt::is_instance`: `src/backend/runtime/hulk_runtime.hpp:207-224`.
- `hulk_rt::checked_cast`: `src/backend/runtime/hulk_runtime.hpp:226-231`.

### 7.15 Features no soportados

El backend mantiene fuera de alcance:

- `for/range`;
- lambdas;
- protocolos;
- vectores.

Citas:

- `for`: `src/backend/ir_gen.cpp:692-694`.
- `range`: `src/backend/ir_gen.cpp:750-751`.
- `lambda`: `src/backend/ir_gen.cpp:727-729`.
- `protocol`: `src/backend/ir_gen.cpp:872-874`.

El error común se produce con `CodegenError`, definido en `src/backend/codegen_error.h:9-12`.

## 8. Emisión C++ desde IR

`IRToCppEmitter` recibe `IRProgram` y no mira el AST. Esta es una propiedad importante: el backend C++ queda desacoplado del lenguaje fuente.

Punto de entrada: `src/backend/ir_to_cpp.cpp:11-21`.

### 8.1 Estructura del C++ generado

El C++ emitido contiene:

1. include del runtime;
2. datos/string pool;
3. prototipos;
4. funciones IR;
5. registro de tipos;
6. `main`.

Cita: `src/backend/ir_to_cpp.cpp:11-21`.

### 8.2 Datos

Los strings IR se emiten como `const std::string` en namespace anónimo.

Cita: `src/backend/ir_to_cpp.cpp:24-35`.

### 8.3 Funciones

El emisor genera funciones C++ uniformes:

- métodos: `Value fn(ObjectPtr self, const vector<Value>& args)`;
- funciones globales/init/main: `Value fn(const vector<Value>& args)`.

Citas:

- prototipos: `src/backend/ir_to_cpp.cpp:37-50`.
- firma y aridad: `src/backend/ir_to_cpp.cpp:52-95`.

### 8.4 Instrucciones

Las operaciones aritméticas y de comparación se traducen a helpers del runtime.

Citas:

- aritmética: `src/backend/ir_to_cpp.cpp:120-146`.
- comparaciones: `src/backend/ir_to_cpp.cpp:158-180`.
- concat: `src/backend/ir_to_cpp.cpp:182-189`.

Los labels y saltos IR se emiten como `goto` local en C++:

- `Label`: `src/backend/ir_to_cpp.cpp:190-193`.
- `Jump`: `src/backend/ir_to_cpp.cpp:194-196`.
- `JumpIfTrue`: `src/backend/ir_to_cpp.cpp:197-200`.
- `JumpIfFalse`: `src/backend/ir_to_cpp.cpp:201-204`.

Objetos y dispatch:

- `NewObject`: `src/backend/ir_to_cpp.cpp:212-216`.
- `GetField`: `src/backend/ir_to_cpp.cpp:217-220`.
- `DefineField`: `src/backend/ir_to_cpp.cpp:221-225`.
- `SetField`: `src/backend/ir_to_cpp.cpp:226-229`.
- `VCall`: `src/backend/ir_to_cpp.cpp:230-234`.
- `SCallMethod`: `src/backend/ir_to_cpp.cpp:235-239`.

Casts:

- `IsType`: `src/backend/ir_to_cpp.cpp:240-243`.
- `AsType`: `src/backend/ir_to_cpp.cpp:244-247`.

Builtins:

- `print`, `sqrt`, `sin`, `cos`, `exp`, `log`, `rand`: `src/backend/ir_to_cpp.cpp:248-269`.

### 8.5 Registro de tipos y main

El registro runtime de tipos se genera desde `.TYPES`:

- tipos y padres: `src/backend/ir_to_cpp.cpp:274-280`.
- métodos y aridad: `src/backend/ir_to_cpp.cpp:281-288`.

El `main` registra tipos, invoca `hulk_main` y captura errores runtime:

- `src/backend/ir_to_cpp.cpp:293-305`.

## 9. Runtime actual

El runtime sigue siendo header-only en `src/backend/runtime/hulk_runtime.hpp`.

### 9.1 Valores

`hulk_rt::Value` es:

```cpp
variant<Nil, double, string, bool, ObjectPtr>
```

Cita: `src/backend/runtime/hulk_runtime.hpp:31-42`.

### 9.2 Objetos y métodos

Los objetos guardan:

- `type_name`;
- tabla dinámica de campos por string.

Cita: `src/backend/runtime/hulk_runtime.hpp:44-49`.

Los métodos se registran como funciones runtime con aridad:

- `MethodFn`: `src/backend/runtime/hulk_runtime.hpp:51`.
- `Method`: `src/backend/runtime/hulk_runtime.hpp:53-56`.
- `TypeInfo`: `src/backend/runtime/hulk_runtime.hpp:58-61`.
- `register_type`: `src/backend/runtime/hulk_runtime.hpp:68-70`.
- `register_method`: `src/backend/runtime/hulk_runtime.hpp:72-77`.

### 9.3 Operaciones básicas

El runtime implementa:

- truthiness: `src/backend/runtime/hulk_runtime.hpp:123-127`.
- print: `src/backend/runtime/hulk_runtime.hpp:129-132`.
- aritmética: `src/backend/runtime/hulk_runtime.hpp:134-155`.
- comparaciones: `src/backend/runtime/hulk_runtime.hpp:157-173`.
- concat: `src/backend/runtime/hulk_runtime.hpp:175-177`.
- builtins matemáticos: `src/backend/runtime/hulk_runtime.hpp:179-186`.

### 9.4 Objetos, casts y dispatch

- `get_field`: `src/backend/runtime/hulk_runtime.hpp:188-195`.
- `set_field`: `src/backend/runtime/hulk_runtime.hpp:197-205`.
- `is_instance`: `src/backend/runtime/hulk_runtime.hpp:207-224`.
- `checked_cast`: `src/backend/runtime/hulk_runtime.hpp:226-231`.
- búsqueda de método: `src/backend/runtime/hulk_runtime.hpp:233-244`.
- llamada desde tipo base: `src/backend/runtime/hulk_runtime.hpp:246-259`.
- llamada virtual normal: `src/backend/runtime/hulk_runtime.hpp:261-266`.

## 10. Pruebas agregadas y evidencia

El runner de backend se extendió para validar no solo ejecución, sino también la nueva capa IR.

### 10.1 Pruebas existentes preservadas

El runner sigue ejecutando:

- C4: `tests/eval/c4_*.hulk`.
- C5: `tests/eval/c5_*.hulk`.
- C6: `tests/eval/c6_*.hulk`.
- type holes válidos: `tests/extension/valid_*.hulk`.
- type holes inválidos seleccionados.

Citas:

- C4-C6: `tests/backend/run_backend_tests.sh:197-213`.
- type holes válidos: `tests/backend/run_backend_tests.sh:215-219`.
- type holes inválidos: `tests/backend/run_backend_tests.sh:227-238`.

### 10.2 Pruebas de `--emit-ir`

Se agregó smoke test de `.TYPES`, `.DATA` y `.CODE` en:

- `tests/backend/run_backend_tests.sh:102-127`.
- invocaciones concretas: `tests/backend/run_backend_tests.sh:240-244`.

### 10.3 Prueba de `--emit-cpp`

Se agregó una prueba que:

1. invoca `--emit-cpp`;
2. compila el `.cpp` generado con `g++`;
3. ejecuta el binario;
4. compara contra expected.

Cita: `tests/backend/run_backend_tests.sh:129-164`.

### 10.4 Snapshots IR

Se agregaron snapshots de IR para casos pequeños:

- `tests/backend/ir_cases/c4_ir_control.hulk`.
- `tests/backend/ir_cases/c5_ir_call.hulk`.
- `tests/backend/ir_cases/c6_ir_object.hulk`.

Expected:

- `tests/expected/backend_ir/c4_ir_control.hir`.
- `tests/expected/backend_ir/c5_ir_call.hir`.
- `tests/expected/backend_ir/c6_ir_object.hir`.

El runner compara snapshots en `tests/backend/run_backend_tests.sh:166-190` y los ejecuta en `tests/backend/run_backend_tests.sh:246-250`.

### 10.5 Regresión de asignación destructiva

Se agregó:

- `tests/backend/regression/assign_value_capture.hulk`.
- `tests/expected/backend/assign_value_capture.expected`.

El objetivo es verificar que `x := expr` devuelva el valor asignado y no una referencia mutable al local.

El runner la ejecuta en `tests/backend/run_backend_tests.sh:221-225`.

### 10.6 Resultados verificados

Comandos ejecutados después de la migración:

```bash
make backend-tests
```

Resultado:

```text
Total  : 39
Passed : 39
Failed : 0
```

También se ejecutó:

```bash
make run-tests
```

Resultado:

```text
Total  : 76
Passed : 76
Failed : 0
```

Pruebas manuales:

```bash
./hulk_backend tests/eval/c5_recursion.hulk --emit-ir -o /tmp/c5_recursion.hir
./hulk_backend tests/eval/c6_objects_basic.hulk --emit-cpp -o /tmp/hulk.cpp
./hulk_backend tests/eval/c5_recursion.hulk -o /tmp/hulk_rec && /tmp/hulk_rec
```

La ejecución de `c5_recursion` produjo:

```text
0
1
55
120
3628800
```

## 11. Qué se implementó exactamente

### Implementado

- Pipeline `AST -> HulkIR -> C++20`.
- Modelo `Hulk::IR`.
- Printer textual con `.TYPES`, `.DATA`, `.CODE`.
- CLI `--emit-ir`.
- Emisión C++ desde IR.
- Eliminación del backend directo anterior.
- Soporte funcional C4:
  - literales;
  - aritmética;
  - lógica;
  - strings;
  - builtins;
  - bloques;
  - `let`;
  - `if`;
  - `while`.
- Soporte funcional C5:
  - funciones;
  - llamadas;
  - recursividad;
  - parámetros;
  - shadowing;
  - asignación destructiva.
- Soporte funcional C6:
  - tipos;
  - objetos;
  - inicialización;
  - atributos;
  - asignación a atributos;
  - métodos;
  - dispatch virtual;
  - herencia;
  - `self`;
  - `base`;
  - `is`;
  - `as`.
- Tests de ejecución, emisión C++, emisión IR, snapshots IR y regresiones.

### No implementado

- VM propia.
- LLVM.
- Ensamblador.
- Layout real por slots.
- VTables reales.
- GC.
- `for/range`.
- Lambdas.
- Protocolos.
- Vectores.
- Optimización de IR.

## 12. Cosas que debemos tener en cuenta o arreglar

### 12.1 El backend todavía tolera dos errores semánticos

El driver conserva dos tolerancias transitorias:

- acceso a atributos privados desde fuera de `self`;
- `is` considerado "no plausible" por el type checker.

Cita: `src/backend/backend_driver.cpp:60-70`.

Esto permitió mantener paridad con los tests actuales, pero conceptualmente el backend debería recibir programas ya aceptados por semántica y type checking. Esta es una incongruencia entre fases.

Recomendación:

- decidir si el core del proyecto permitirá atributos públicos;
- relajar el type checker para `is` dinámico entre tipos existentes;
- eliminar estas tolerancias del backend cuando semántica y tests estén alineados.

### 12.2 Atributos: privados en teoría, públicos en tests

La IR y el runtime soportan `getfield obj x` para cualquier receptor si el programa llega al backend. Esto preserva los tests actuales, pero HULK teórico trata atributos como privados y recomienda acceso mediante métodos.

Riesgo:

- El informe final debe declarar esta divergencia.

Opciones:

1. Hacer atributos públicos en el subconjunto implementado.
2. Mantener atributos privados y cambiar tests/evaluador.
3. Añadir una bandera de compatibilidad, menos recomendable.

### 12.3 `is` debería ser más dinámico

El runtime ya puede devolver `false` para un `is` no satisfecho: `src/backend/runtime/hulk_runtime.hpp:207-224`.

El problema está antes: el type checker puede rechazar algunos casos como no plausibles. El backend lo tolera temporalmente.

Recomendación:

- permitir `is` si el tipo objetivo existe y el operando puede ser objeto;
- reservar errores fuertes para `as`, porque `as` sí puede fallar en runtime.

### 12.4 `type_map_` aún no participa en IRGen

`IRGen` recibe `type_map_`, pero lo marca como no usado en `src/backend/ir_gen.cpp:50-51`.

Esto no rompe ejecución, pero limita la riqueza de la IR.

Usos futuros:

- anotar tipos inferidos en `IRField`;
- optimizar builtins;
- validar casts;
- emitir mejores diagnósticos de backend;
- preparar una IR más baja o typed IR.

### 12.5 `.TYPES` tiene slots informativos, no layout real

La IR imprime slots en campos y métodos, pero el runtime sigue usando nombres en mapas dinámicos.

Citas:

- slots en IR: `src/ir/ir.h:65-79`.
- campos runtime por string: `src/backend/runtime/hulk_runtime.hpp:44-49`.
- métodos runtime por string: `src/backend/runtime/hulk_runtime.hpp:58-77`.

Esto es defendible como IR de nivel medio, pero no como backend de bajo nivel tipo BANNER/CIL.

Continuación posible:

- aplanar herencia;
- asignar slots estables globales;
- migrar `get_field`/`set_field` a offsets;
- migrar dispatch a vtable por slot.

### 12.6 El nombre bajado de campo no se usa todavía para runtime

La IR de tipos guarda `lowered_name`, por ejemplo `Box_x`, pero las instrucciones `GetField`/`SetField` usan el nombre fuente del atributo, por ejemplo `x`.

Citas:

- `lowered_name`: `src/backend/ir_gen.cpp:119`.
- `GetField` usa `node.GetMemberName()`: `src/backend/ir_gen.cpp:819-827`.
- `DefineField` usa `attr->GetName()`: `src/backend/ir_gen.cpp:348-359`.

Esto mantiene compatibilidad con el runtime actual, pero si se migra a slots/layout real habrá que unificar el modelo.

### 12.7 Documentos viejos pueden estar desactualizados

Al eliminar `codegen_cpp.*`, cualquier documento que todavía diga que el backend central es `CppCodegen` quedó obsoleto.

Archivos detectados con menciones antiguas:

- `doc/hito-d-backend.md`.
- `doc/informe-implementacion-hito-d-backend.md`.

Recomendación:

- actualizar esos documentos para decir `IRGen + IRToCppEmitter`.

### 12.8 Snapshots IR son útiles pero sensibles a nombres

Los snapshots dependen de nombres generados como `hulk_tmp_number_0`. Si se cambia el orden de lowering, pueden fallar aunque la semántica siga bien.

Recomendación:

- conservar snapshots pequeños;
- no snapshotear programas enormes;
- combinar snapshots con pruebas estructurales de `.TYPES/.DATA/.CODE`.

### 12.9 `for/range` sigue fuera de alcance

El AST y semántica tienen nociones de `for`/`range`, pero backend lo rechaza explícitamente.

Citas:

- `for`: `src/backend/ir_gen.cpp:692-694`.
- `range`: `src/backend/ir_gen.cpp:750-751`.

Si el alcance final del proyecto exige iterables, esta es la continuación más visible.

### 12.10 No hay optimización de IR

La IR no optimiza:

- constantes;
- temporales muertos;
- labels redundantes;
- copies innecesarias.

Esto es aceptable para el hito funcional. Si se quiere presentar una fase de optimización, podría agregarse un pass `IRPass` entre `IRGen` e `IRToCppEmitter`.

### 12.11 No hay ejecución directa de IR

La IR se imprime y se traduce a C++20, pero no existe intérprete ni VM de HulkIR.

Esto no impide defender el backend, porque el objetivo acordado fue C++20 como target portable. Pero si se desea mayor alineación con una arquitectura de compilador clásico, una VM sería una continuación posible.

## 13. ¿Es esta una fase final?

Respuesta corta:

```text
Sí, para el backend funcional del alcance C4-C6 actual.
No, para el lenguaje HULK completo ni para un backend bajo nivel definitivo.
```

### Puede considerarse final para el hito backend funcional porque:

- compila programas HULK soportados a ejecutables nativos mediante C++20 y `g++`;
- conserva paridad con el evaluador en C4-C6;
- pasa `make backend-tests`;
- pasa `make run-tests`;
- tiene IR explícita;
- tiene salida `--emit-ir`;
- tiene salida `--emit-cpp`;
- eliminó el camino directo `AST -> C++`;
- tiene tests de ejecución, IR y C++ emitido.

### Debe tener continuación si el objetivo es:

- implementar todo HULK;
- cerrar inconsistencias semánticas;
- añadir `for/range`;
- añadir lambdas/protocolos/vectores;
- migrar a layout real por slots;
- implementar VM, LLVM o ensamblador;
- usar `type_map_` para una IR typed;
- agregar optimizaciones.

## 14. Conclusión

El backend quedó en una arquitectura mucho más defendible:

```text
Frontend: lexer/parser/AST
Semántica: resolver + inferencia + type checking
Backend: IRGen -> HulkIR -> IRToCppEmitter -> C++20
Runtime: hulk_rt::Value + objetos + dispatch + builtins
```

La implementación actual no es solo un transpilador directo a C++; ahora existe una IR propia, imprimible, testeada y usada realmente para generar código. Para el alcance C4-C6 y las extensiones de type holes ya probadas, esta fase es funcionalmente final. Lo que queda por delante no es reparar el backend para que funcione, sino decidir si se quiere convertirlo en un backend más completo, más bajo nivel o más alineado con toda la especificación formal de HULK.
