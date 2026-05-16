# Informe de implementación del Hito D: Backend C++20

## Resumen

Se implementó el **Hito D** como un backend source-to-source que toma un programa HULK ya parseado y analizado, genera código C++20, y opcionalmente lo compila a un ejecutable nativo usando `g++`.

El backend cubre el núcleo que hoy está validado por el evaluador:

- Corte 4: expresiones, literales, operaciones, strings, builtins, bloques, `let`, `if`, `while`.
- Corte 5: variables, shadowing, asignación destructiva, funciones globales y recursión.
- Corte 6: tipos, objetos, atributos, métodos, herencia, dispatch dinámico, `self`, `is` y `as`.

También se agregó una suite de pruebas específica para backend que compila y ejecuta los programas de `tests/eval/` y compara la salida contra los `.expected` existentes.

## Lo que se implementó

### CLI y flujo completo de backend

Se agregó el binario:

```bash
./hulk_backend archivo.hulk -o ejecutable
./hulk_backend archivo.hulk --emit-cpp -o salida.cpp
```

El flujo implementado es:

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

Archivos principales:

- `src/backend/main.cpp`
- `src/backend/backend.h`
- `src/backend/backend.cpp`
- `src/backend/backend_driver.h`
- `src/backend/backend_driver.cpp`

El driver:

- lee el archivo fuente;
- ejecuta lexer/parser;
- valida que el AST raíz sea `Hulk::Program`;
- ejecuta `SemanticAnalyzer`;
- genera C++ con `CppCodegen`;
- escribe el `.cpp`;
- compila a ejecutable si no se usa `--emit-cpp`.

### Runtime propio para C++ generado

Se agregó un runtime header-only:

```text
src/backend/runtime/hulk_runtime.hpp
```

Incluye:

- `hulk_rt::Value` como `std::variant<Nil, double, std::string, bool, ObjectPtr>`.
- `hulk_rt::Object` con `type_name` y tabla dinámica de campos.
- Registro dinámico de tipos y métodos.
- Helpers de conversión: `as_number`, `as_bool`, `as_object`, `to_string`, `truthy`.
- Operaciones aritméticas, lógicas y de comparación.
- Concatenación `@` y `@@`.
- Builtins: `sqrt`, `sin`, `cos`, `exp`, `log`, `rand`, `print`.
- Acceso y asignación de atributos.
- `is_instance`, `checked_cast`.
- `call_method` y `call_method_from` para dispatch dinámico y llamadas tipo `base`.

### Generación de C++20

Se agregó el generador:

```text
src/backend/codegen_cpp.h
src/backend/codegen_cpp.cpp
```

El generador produce un único `.cpp` que incluye:

- prototipos;
- funciones globales HULK;
- constructores e inicializadores de tipos;
- métodos de tipos;
- función `hulk_register_types`;
- `main`.

Todas las expresiones HULK se emiten como `hulk_rt::Value`.

Para preservar la semántica de expresión del lenguaje, los nodos compuestos se generan con lambdas inmediatas de C++:

- `let`;
- bloques;
- `if`;
- `while`;
- llamadas;
- construcción de objetos;
- asignaciones;
- llamadas a métodos.

### Nombres y scopes

Se agregaron:

```text
src/backend/name_mangler.h
src/backend/name_mangler.cpp
src/backend/codegen_context.h
src/backend/codegen_context.cpp
```

Esto permite:

- generar nombres C++ válidos y únicos;
- evitar colisiones con palabras reservadas;
- soportar shadowing;
- mapear `VariableBinding*`, `Param*` y símbolos sintéticos a variables C++.

Caso validado:

```hulk
let a = 7, a = a * 6 in print(a);
```

El segundo binding ve al primero, y el cuerpo ve al segundo.

### Objetos, métodos y herencia

Por cada tipo HULK se genera:

- un inicializador `hulk_init_Type`;
- un constructor `hulk_ctor_Type`;
- una función C++ por cada método;
- registro runtime de tipo, padre y métodos.

La inicialización sigue este orden:

1. crear objeto;
2. ligar parámetros del constructor;
3. inicializar padre si existe;
4. inicializar atributos propios;
5. registrar métodos para dispatch dinámico.

Los métodos reciben `self` explícito:

```cpp
hulk_rt::Value hulk_method_Type_method(
    hulk_rt::ObjectPtr self,
    const std::vector<hulk_rt::Value>& args
)
```

Dentro del método se expone:

```cpp
hulk_rt::Value hulk_self_value(self);
```

### Integración con Makefile

Se agregaron variables y targets:

```make
BACKEND_SRCS
BACKEND_OBJS
backend
backend-tests
```

También se actualizó `clean` para borrar:

```text
hulk_backend
```

### Tests de backend

Se agregó:

```text
tests/backend/run_backend_tests.sh
```

La suite:

- construye `hulk_backend`;
- compila los tests `c4_*.hulk`, `c5_*.hulk`, `c6_*.hulk`;
- ejecuta los binarios generados;
- compara salida con `tests/expected/eval/*.expected`.

Resultado verificado:

```text
make backend-tests -> 16/16 passed
make run-tests     -> 76/76 passed
```

## Lo que no se pudo implementar o quedó fuera de alcance

### `for` y `range`

No se implementaron en el backend.

Motivo:

- El AST y el resolver tienen soporte parcial para `for`.
- `SemanticTables` registra `range`.
- Pero el evaluador actual reporta `for` y `range()` como no soportados.

Decisión tomada:

- El backend también los deja fuera por ahora.
- Si aparece `for` o `range`, se reporta:

```text
Backend no soporta todavia: for/range
```

### Lambdas

No se implementaron lambdas de primera clase.

Motivo:

- Existe nodo `Lambda`.
- El evaluador las reporta como no soportadas.
- La inferencia también las trata como fuera del core.

Decisión tomada:

- El backend reporta:

```text
Backend no soporta todavia: lambda
```

### Protocolos y vectores

No se implementaron protocolos ni vectores.

Motivo:

- Hay archivos/nodos parciales o documentación previa que los menciona.
- No forman parte del subconjunto ejecutable actual C4-C6.
- `SymbolResolver::visit(ProtocolDecl)` es no-op.

Decisión tomada:

- Protocolos quedan fuera de Hito D inicial.
- Vectores no se tocan.

### `base()` real en tests de evaluación

El backend incluye una ruta para `base()`:

- como `FunctionCall("base")` resuelto a método padre;
- o como `BaseCall`, si algún parser futuro lo produce.

Pero no se validó contra tests reales de evaluación con `base()`.

Motivo:

- Los tests actuales `c6_base_call.hulk` y `c6_base_method.hulk` son placeholders.
- Ambos solo imprimen:

```text
base() pendiente de soporte en parser
```

Por tanto, la infraestructura está preparada, pero falta un test real de parser/eval que ejerza `base()`.

## Incongruencias detectadas en fases anteriores

### 1. Evaluador acepta acceso público a atributos, semántica lo rechaza

Ejemplo:

```hulk
let p = new Point(3, 4) in {
    print(p.x);
    print(p.y);
};
```

Estado actual:

- El evaluador lo acepta y los tests `c6_objects_basic.hulk` esperan que funcione.
- El analizador semántico reporta:

```text
Los atributos son privados. Solo se pueden acceder mediante 'self'.
```

Impacto:

- Si el backend abortara ante todo error semántico, no podría compilar `c6_objects_basic.hulk`.

Solución aplicada solo en backend:

- Se tolera ese diagnóstico exacto en `BackendDriver`.
- El backend genera acceso runtime con `hulk_rt::get_field`.

Recomendación:

- Decidir una semántica definitiva:
  - atributos públicos, como asume el evaluador;
  - o atributos privados, como asume el semántico.
- Luego alinear evaluador, semántica, typecheck y tests.

### 2. `is` entre tipos hermanos: evaluador lo acepta, typecheck lo rechaza

Ejemplo de `c6_inheritance.hulk`:

```hulk
let a = new Dog("Buddy") in {
    print(a is Cat);
};
```

Estado actual:

- El evaluador lo acepta y devuelve `false`.
- El type checker reporta:

```text
Operación 'is' no plausible: tipo 'Dog' y 'Cat' no están relacionados.
```

Impacto:

- Si el backend abortara ante ese error, no podría compilar `c6_inheritance.hulk`.

Solución aplicada solo en backend:

- Se toleran diagnósticos que contienen:

```text
'is' no plausible:
```

- El runtime evalúa `is` dinámicamente y devuelve `true` o `false`.

Recomendación:

- Para `is`, conviene permitir chequeos entre tipos no relacionados y devolver `false`.
- Reservar errores de plausibilidad para `as`, donde un cast inválido sí puede fallar.

### 3. `for/range` está en semántica, pero no en runtime/evaluador

Estado actual:

- Hay nodo `For`.
- Hay símbolo sintético para variable de `for`.
- `SemanticTables` registra builtin `range`.
- `TypeInferencer` puede producir tipo `Iterable`.
- El evaluador reporta:

```text
SEM_UNSUPPORTED: for
SEM_UNSUPPORTED: range()
```

Impacto:

- El lenguaje está parcialmente modelado, pero no ejecutable.
- El backend no puede implementar esto limpiamente sin inventar un runtime de iterables que el evaluador no tiene.

Recomendación:

- Implementar `Iterable`/`Range` en `src/objects` y evaluador primero.
- Luego extender backend con el mismo comportamiento.

### 4. `base()` tiene varias representaciones posibles

Estado actual:

- Existe nodo `BaseCall`.
- El resolver también trata `FunctionCall("base")` como llamada especial.
- Los AST dumps actuales de typecheck muestran `FunctionCall(name=base)`.
- Los tests de eval con `base()` real están comentados o son placeholders.

Impacto:

- Backend tuvo que soportar ambas posibilidades.
- No existe test ejecutable que cierre el contrato real de `base()`.

Recomendación:

- Definir una sola representación canónica:
  - o parser genera `BaseCall`;
  - o se mantiene `FunctionCall("base")` como forma oficial.
- Agregar tests reales de parser, semántica, evaluador y backend para `base()`.

### 5. `self` existe como nodo AST, pero el parser usa `VariableReference("self")`

Estado actual:

- Existe clase `SelfRef`.
- El resolver soporta `SelfRef`.
- Pero los AST dumps de programas reales muestran `VariableReference(name=self)`.

Impacto:

- Backend no puede depender solo de `SelfRef`.
- Debe reconocer `self` mediante `resolution_map` como `SyntheticSymbol`.

Solución aplicada:

- Si una referencia se resuelve como `SyntheticKind::Self`, el backend emite `hulk_self_value`.

Recomendación:

- Decidir si `self` debe ser keyword/nodo propio o identificador especial.
- Alinear lexer, parser, AST dumps y resolver.

### 6. Documentación previa menciona features fuera del core real

En documentos anteriores aparecen protocolos, vectores, lambdas/functores y extensiones.

Estado actual:

- No forman parte del subconjunto ejecutable probado por `tests/eval`.
- Algunas estructuras existen, pero sin pipeline completo.

Impacto:

- Puede dar la impresión de que el backend debe cubrir más lenguaje del que realmente está cerrado.

Recomendación:

- Separar documentación de:
  - lenguaje core implementado;
  - extensiones diseñadas;
  - extensiones pendientes.

### 7. `grammar.y` y parser generado no parecen estar perfectamente alineados

Ya se había detectado en el plan:

- `grammar.y` menciona `AUTO` y `UNDERSCORE_TYPE`.
- El lexer y adapter actuales no exponen esos tokens de forma equivalente.
- La suite pasa porque se usa el parser generado existente.

Impacto:

- Regenerar parser con `make parser-gen` puede abrir problemas no relacionados con backend.

Recomendación:

- No regenerar parser dentro del Hito D.
- Crear una tarea separada para alinear grammar, lexer, adapter y parser generado.

## Decisiones técnicas importantes

### Backend tolera solo dos errores semánticos conocidos

El backend depende de `SemanticAnalyzer`, pero hay dos diagnósticos que se toleran para poder compilar los tests C6 del evaluador:

1. Acceso público a atributos:

```text
Los atributos son privados. Solo se pueden acceder mediante 'self'.
```

2. `is` no plausible:

```text
'is' no plausible:
```

Cualquier otro error semántico o de tipos sigue abortando la generación.

Esto no cambia `hulk_semantic`; solo afecta al flujo de `hulk_backend`.

### Backend emite valores dinámicos

Aunque existe `type_map`, el backend no desboxea `Number`, `String` o `Boolean`.

Motivo:

- El objetivo era cerrar funcionalidad C4-C6.
- El modelo dinámico replica mejor el evaluador actual.
- Optimizar con tipos estáticos se puede hacer después.

## Estado final

Comandos validados:

```bash
make backend
make backend-tests
make run-tests
```

Resultados:

```text
make backend-tests -> 16/16 passed
make run-tests     -> 76/76 passed
```

Ejemplos verificados:

```bash
./hulk_backend tests/eval/c5_recursion.hulk -o /tmp/hulk_rec
/tmp/hulk_rec

./hulk_backend tests/eval/c6_objects_basic.hulk --emit-cpp -o /tmp/hulk.cpp
```

## Pendientes recomendados

1. Alinear semántica de atributos: públicos vs privados.
2. Alinear semántica de `is` para tipos no relacionados.
3. Implementar `for/range` primero en evaluador/runtime y luego en backend.
4. Agregar tests reales de `base()`.
5. Decidir representación canónica de `self` y `base`.
6. Separar documentación de core implementado y extensiones futuras.
7. Revisar `grammar.y` antes de usar `parser-gen`.

