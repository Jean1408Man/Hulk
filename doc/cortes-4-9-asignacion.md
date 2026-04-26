# Asignación de cortes 4–9 — Compilador HULK
> Basado en el plan oficial y el estado actual del proyecto (rama `refactor/improve_ast`)

## Estado actual del proyecto

| Componente | Estado |
|---|---|
| Lexer (`src/lexer/`) | **Completo** |
| AST — 50+ nodos (`src/ast/`) | **Completo** |
| Infraestructura de diagnósticos (`src/common/`) | **Completo** |
| Parser (`src/parser/`) | En otra rama (considerar como disponible) |
| Evaluador (`src/eval/`) | Vacío |
| Binding (`src/binding/`) | Vacío |
| Inferencia (`src/inference/`) | Vacío |
| Type checking (`src/typecheck/`) | Vacío |
| Semántico (`src/semantic/`) | Vacío |
| Objetos/runtime (`src/objects/`) | Vacío |

El AST ya tiene **todos los nodos necesarios** para los cortes 4–9: literales, operaciones, `LetIn`, `IfStmt`, `WhileStmt`, `For`, `FunctionDecl`, `FunctionCall`, `TypeDecl`, `NewExpr`, `MemberAccess`, `MethodCall`, `IsExpr`, `AsExpr`, `DestructiveAssign`, `VectorLiteral`, `ProtocolDecl`, etc.

---

## Propuesta revisada de agrupación

> **Nota sobre la propuesta:** Los cortes 4+5 del plan original mezclan dos problemas distintos en el mismo bloque — evaluación de expresiones y resolución de nombres — que tienen naturalezas muy diferentes. La propuesta a continuación los mantiene juntos por seguir el plan, pero los delimita claramente internamente para que no se confundan.

---

## Persona 1 — Cortes 4 + 5: Intérprete base, scopes y funciones

**¿Qué es esto?** Hacer que el lenguaje *corra* sin tipos estáticos. Un evaluador que recorre el AST y produce valores en tiempo de ejecución.

**Directorio principal:** `src/eval/`, `src/objects/` (valores en runtime)

### Corte 4 — Subconjunto ejecutable

Implementar un evaluador de árbol que recorra los nodos AST ya definidos y produzca resultados.

**Qué implementar:**

- **Valores en runtime** — necesitás una representación de valor: `Number(double)`, `String(string)`, `Boolean(bool)`, `Nil`. Puede ser un `std::variant` o una jerarquía de clases en `src/objects/`.
- **Literales** — nodos `Number`, `String`, `Boolean` → devolver el valor directamente.
- **Aritmética** — nodo `ArithmeticBinOp`: `+`, `-`, `*`, `/`, `^`, `%`.
- **Lógica y comparación** — nodo `LogicBinOp`: `and`, `or`, `!`, `>`, `<`, `==`, `!=`, `>=`, `<=`.
- **Strings** — nodo `StringBinOp`: `@` (concat), `@@` (concat con espacio).
- **Builtins** — nodos `Print` y `BuiltinCall`: `print`, `sqrt`, `sin`, `cos`, `exp`, `log`, `rand`.
- **Bloques** — nodo `ExprBlock`: evaluar cada expresión en orden, devolver la última.
- **`let/in`** — nodo `LetIn` con múltiples `VariableBinding`: crear scope temporal, evaluar body.
- **`if/elif/else`** — nodo `IfStmt` + `ElifBranch`: evaluar condición, entrar en la rama correcta.
- **`while`** — nodo `WhileStmt`: loop mientras condición sea `true`.
- **`for`** — nodo `For`: si el parser lo desazucara a `while`, ya funciona; si no, evaluarlo directamente sobre iterables.

**Clases AST que se usan:** `Number`, `String`, `Boolean`, `ArithmeticBinOp`, `LogicBinOp`, `LogicUnaryOp`, `ArithmeticUnaryOp`, `StringBinOp`, `Print`, `BuiltinCall`, `ExprBlock`, `LetIn`, `VariableBinding`, `IfStmt`, `ElifBranch`, `WhileStmt`, `For`, `Group`.

**Criterio de cierre:**
```
// Esto debe correr y producir resultados correctos:
print(1 + 2 * 3);           // 7
print("hola" @ " " @ "mundo"); // hola mundo
let x = 10 in print(x * 2);    // 20
if (3 > 2) print("si") else print("no"); // si
```

---

### Corte 5 — Variables, binding y funciones

El evaluador del corte 4 ya existe. Ahora se le agrega **entorno léxico** y **funciones**.

**Qué implementar:**

- **Environment/Scope** — estructura de tabla de símbolos con soporte de encadenamiento de scopes (parent pointer). Crear uno nuevo por cada `let`, bloque de función, etc.
- **`VariableReference`** — buscar el nombre en el scope actual y subir en la cadena.
- **`let` múltiple y shadowing** — el nodo `LetIn` puede tener varios `VariableBinding`; cada binding ve los anteriores del mismo `let`.
- **Asignación destructiva `:=`** — nodo `DestructiveAssign`: buscar el nombre en el scope y actualizar su valor (no crear uno nuevo).
- **Funciones globales** — nodo `FunctionDecl`: registrar en una tabla global de funciones (nombre → parámetros + cuerpo AST).
- **Llamadas** — nodo `FunctionCall`: crear un scope nuevo con los parámetros ligados a los argumentos evaluados, evaluar el cuerpo.
- **Recursión** — si la función se registra antes de evaluarse, la recursión funciona naturalmente.
- **Errores de nombres** — variable no declarada, función no definida, aridad incorrecta → reportar con `DiagnosticEngine`.

**Clases AST que se usan:** `VariableReference`, `VariableBinding`, `LetIn`, `DestructiveAssign`, `FunctionDecl`, `FunctionCall`, `Lambda`, `Program`.

**Criterio de cierre:**
```
function fib(n) => if (n <= 1) n else fib(n-1) + fib(n-2);
print(fib(10)); // 55

let x = 5 in {
    x := x + 1;
    print(x); // 6
};
```

---

## Persona 2 — Cortes 6 + 7: Objetos, clases e infraestructura semántica

**¿Qué es esto?** Corte 6 extiende el intérprete para POO. Corte 7 **no ejecuta nada nuevo** — construye las estructuras de datos estáticas que necesitan la inferencia y el type checker.

### Corte 6 — Objetos y clases

Extender el evaluador para soportar tipos, instancias y herencia. Sigue siendo interpretación dinámica, sin tipos estáticos.

**Directorio principal:** `src/objects/`, `src/eval/`

**Qué implementar:**

- **Valor `Object` en runtime** — una instancia tiene: tipo al que pertenece, tabla de atributos (nombre → valor), referencia a la `TypeDecl` para encontrar métodos.
- **`TypeDecl`** — registrar cada tipo en una tabla global: nombre, parámetros del constructor, padre, atributos, métodos.
- **`NewExpr`** — instanciar: evaluar los argumentos del constructor, crear un `Object`, ejecutar la inicialización de atributos en un scope con `self` ligado.
- **`MemberAccess`** — `obj.attr`: buscar en la tabla de atributos de la instancia.
- **`MethodCall`** — `obj.method(args)`: buscar el método en el tipo (o en el padre si hay herencia), ejecutar con `self` ligado a `obj`.
- **`SelfRef`** — dentro de un método, `self` referencia la instancia actual.
- **Herencia (`inherits`)** — si un método no está en el tipo actual, buscarlo en el padre (cadena de herencia).
- **`override`** — el método del hijo tapa el del padre; se resuelve buscando primero en el tipo concreto.
- **`base()`** — nodo `BaseCall`: llamar al constructor o método del padre con los argumentos dados.
- **`DestructiveAssignMember`** — `self.attr := value` dentro de un método.
- **Errores estructurales** — tipo padre no declarado, ciclo de herencia, atributo inexistente, aridad del constructor incorrecta → reportar antes de ejecutar.

**Clases AST que se usan:** `TypeDecl`, `TypeMemberAttribute`, `TypeMemberMethod`, `NewExpr`, `MemberAccess`, `MethodCall`, `SelfRef`, `BaseCall`, `DestructiveAssignMember`, `IsExpr` (runtime: comparar tipo de la instancia).

**Criterio de cierre:**
```
type Animal(name) {
    name = name;
    speak() => print("...");
}
type Dog(name) inherits Animal(name) {
    speak() => print("Woof! I am " @ self.name);
}
let d = new Dog("Rex") in d.speak(); // Woof! I am Rex
```

---

### Corte 7 — Análisis semántico base (infraestructura estática)

Este corte **no agrega ejecución nueva**. Su producto es un AST anotado y dos tablas globales que los cortes 8 y 9 necesitan para operar.

**Directorio principal:** `src/semantic/`, `src/binding/`

**Qué implementar:**

- **Tabla global de tipos** — recorrer todas las `TypeDecl` del `Program` y registrar: nombre, parámetros del constructor, padre, lista de atributos (nombre + tipo anotado), lista de métodos (nombre + parámetros + tipo de retorno anotado).
- **Tabla global de funciones** — recorrer todas las `FunctionDecl` del `Program` y registrar: nombre, parámetros (nombre + tipo anotado), tipo de retorno anotado.
- **Resolución de símbolos (binding)** — recorrer el AST y anotar cada `VariableReference`, `FunctionCall`, `MemberAccess`, `MethodCall` con un puntero a su declaración correspondiente. Usar scopes encadenados igual que en el evaluador pero sin ejecutar.
- **Chequeos previos a tipos:**
  - Tipo padre declarado (no referencia a tipo inexistente).
  - Sin ciclos en la jerarquía de herencia.
  - Sin métodos duplicados dentro del mismo tipo.
  - Aridad correcta en `FunctionCall` y `NewExpr`.
  - `VariableReference` a variable declarada.
  - `override` solo si el método existe en el padre.
- **Anotar el AST** — cada nodo que referencia un nombre debe quedar enlazado a su entidad (puede ser un puntero al nodo `FunctionDecl`, `TypeDecl`, `VariableBinding`, etc.).

**Clases AST que se usan:** Todos los nodos de declaración (`FunctionDecl`, `TypeDecl`, `VariableBinding`, `Param`) y todos los nodos de referencia (`VariableReference`, `FunctionCall`, `MethodCall`, `MemberAccess`, `NewExpr`).

**Criterio de cierre:** Cada nodo que usa un nombre apunta a la entidad que lo declara. Un programa con tipo no declarado o variable indefinida genera un error semántico con span correcto antes de llegar a la inferencia.

---

## Persona 3 — Cortes 8 + 9: Inferencia de tipos y type checking

**¿Qué es esto?** El núcleo estático del compilador. Trabaja sobre el AST ya resuelto por el corte 7. El corte 8 **anota cada expresión con su tipo**; el corte 9 **verifica que los tipos sean compatibles**.

**Directorio principal:** `src/inference/`, `src/typecheck/`

### Corte 8 — Inferencia de tipos

Un pase sobre el AST que determina el tipo de cada expresión, incluso sin anotaciones explícitas del programador.

**Qué implementar:**

- **Representación de tipos** — una estructura `HulkType` que puede ser: `Number`, `String`, `Boolean`, `Object(nombre)`, `Unknown`, `Error`, `Void`. Necesitás también la jerarquía: quién hereda de quién (usar la tabla de tipos del corte 7).
- **Tipos de literales** — `Number` → `Number`, `String` → `String`, `Boolean` → `Boolean`.
- **Aritmética** — `ArithmeticBinOp` con operandos `Number` → `Number`.
- **Lógica y comparación** — `LogicBinOp` → `Boolean`.
- **String ops** — `StringBinOp` → `String`.
- **`ExprBlock`** — tipo = tipo de la última expresión.
- **`LetIn`** — inferir el tipo de cada binding, luego inferir el tipo del body en el scope extendido.
- **`if/elif/else`** — tipo = LCA (ancestro común más bajo) de todos los tipos de rama. Si hay rama `else`, es el LCA de todas; si no hay `else`, es `Void` o el tipo de la rama (depende de la semántica del lenguaje).
- **`while`** — tipo = `Void` o tipo del body (según convención del curso).
- **`FunctionCall`** — si el retorno está anotado, usar esa anotación; si no, inferir del cuerpo de la función (puede requerir un segundo pase o inferencia iterativa).
- **`MethodCall`** — igual que `FunctionCall` pero sobre el método del tipo correspondiente.
- **`NewExpr`** — tipo = el tipo declarado (`TypeDecl` correspondiente).
- **`MemberAccess`** — tipo = tipo anotado del atributo en la `TypeDecl`.
- **`IsExpr`** → `Boolean`. **`AsExpr`** → el tipo al que se castea.
- **Propagación de errores** — si un operando es `Error` o `Unknown`, propagar el error sin generar cascadas.
- **Anotar el AST** — guardar el tipo inferido en cada nodo `Expr` (los nodos ya tienen `Span`; agregar un campo `type` o un mapa externo `Expr* → HulkType`).

**LCA (ancestro común más bajo):** Para calcularlo necesitás la jerarquía de tipos del corte 7. Subís los tipos en la jerarquía hasta encontrar el primer ancestro común.

**Criterio de cierre:** El AST queda completamente anotado con tipos. Un programa sin errores de tipos produce un AST donde cada nodo `Expr` tiene su tipo asignado. La inferencia falla solo en casos genuinamente inválidos o ambiguos.

---

### Corte 9 — Type checking

Dado el AST anotado (corte 8), verificar que todas las operaciones sean válidas según la jerarquía de tipos.

**Qué implementar:**

- **Conformidad de tipos** — `A` conforma a `B` si `A == B` o `A` es descendiente de `B` en la jerarquía. Implementar `conforms_to(A, B)` usando la tabla de tipos del corte 7.
- **Chequeo de argumentos en llamadas** — para cada `FunctionCall` y `MethodCall`: el tipo del argumento `i` debe conformar al tipo del parámetro `i`.
- **Chequeo de retornos** — el tipo del cuerpo de cada `FunctionDecl` debe conformar al tipo de retorno anotado (si lo hay).
- **Chequeo de atributos** — el tipo del inicializador de cada `TypeMemberAttribute` debe conformar al tipo anotado del atributo.
- **Chequeo de `override`** — el tipo de retorno del método hijo debe conformar al del padre; los parámetros deben coincidir en cantidad y tipos.
- **Chequeo de `is`** — el tipo del operando debe estar en alguna relación con el tipo objetivo (no tiene sentido hacer `is` con un tipo completamente disjunto).
- **Chequeo de `as`** — similar a `is`; el cast debe ser plausible en la jerarquía.
- **Chequeo de `if` sin `else`** — si el lenguaje requiere `else`, reportar error.
- **Chequeo de aritmética** — los dos operandos de `ArithmeticBinOp` deben ser `Number`.
- **Chequeo de lógica** — los operandos de `and`/`or` deben ser `Boolean`.
- **Mensajes de error útiles** — incluir el span del nodo, el tipo encontrado y el tipo esperado.

**Clases AST que se usan:** Todos los nodos de expresión anotados por el corte 8, más `FunctionDecl`, `TypeDecl`, `TypeMemberAttribute`, `TypeMemberMethod`.

**Criterio de cierre:** Programas correctos pasan sin errores. Programas con errores de tipos producen mensajes claros con ubicación. **Este corte representa el mínimo académico principal del proyecto.**

```
// Debe aceptarse:
function suma(x: Number, y: Number): Number => x + y;
print(suma(1, 2)); // 3

// Debe rechazarse con error útil:
function suma(x: Number, y: Number): Number => x @ y; // Error: StringBinOp requiere String
```

---

## Resumen de responsabilidades

| Persona | Cortes | Entradas | Salida | Pregunta central |
|---|---|---|---|---|
| 1 | 4 + 5 | AST (del parser) | Programa ejecutándose, funciones y scopes | ¿El programa *corre*? |
| 2 | 6 + 7 | Evaluador de P1 + AST | Objetos en runtime + AST con símbolos resueltos | ¿Las clases *funcionan* y los nombres *están ligados*? |
| 3 | 8 + 9 | AST resuelto de P2 | AST anotado con tipos + reporte de errores de tipos | ¿El programa es *correcto de tipos*? |

## Dependencias entre personas

```
Parser (rama externa)
       │
       ▼
  [Persona 1] eval básico + scopes + funciones
       │
       ▼
  [Persona 2] objetos en runtime + tablas semánticas + binding
       │
       ▼
  [Persona 3] inferencia + type checking
```

**Regla operativa:** Persona 3 no puede empezar el corte 8 sin que Persona 2 entregue el AST con símbolos resueltos. Persona 2 no puede empezar el corte 7 sin que Persona 1 haya definido la representación de entornos/scopes (para reusarla en el análisis estático).

---

## Análisis crítico del plan original y propuesta alternativa

### ¿Los cortes 4+5 tienen sentido juntos?

**Sí, con una advertencia.** Son naturalezas distintas:
- Corte 4 = evaluar expresiones sin nombres (puro cálculo).
- Corte 5 = resolver nombres y ejecutar funciones.

El riesgo es que la persona los mezcle desde el principio y diseñe mal el Environment. **Recomendación:** dentro del corte 4, implementar un Environment vacío/stub desde ya, para que el corte 5 solo lo llene — no lo rediseñe.

### ¿Los cortes 6+7 tienen sentido juntos?

**Parcialmente.** El corte 6 es ejecución (dinámica); el corte 7 es análisis estático. Son fases distintas del pipeline. Sin embargo, agruparlos en una sola persona tiene sentido porque:
- La tabla de tipos del corte 7 es básicamente la versión estática de lo que el corte 6 construyó dinámicamente.
- La misma persona que entiende cómo funcionan los objetos en runtime puede construir su representación estática sin confundirse.

**Riesgo:** que el corte 7 se postergue hasta el final porque "el 6 ya funciona". Definir una fecha interna de entrega para el corte 7 separada del 6.

### ¿Los cortes 8+9 tienen sentido juntos?

**Sí, completamente.** Son la misma fase semántica en dos pasadas:
- Corte 8 = pase ascendente (bottom-up): inferir tipos desde hojas hacia la raíz.
- Corte 9 = pase descendente (top-down): verificar conformidad desde declaraciones hacia usos.

En la práctica muchas implementaciones los fusionan en un solo recorrido. Que una sola persona los haga asegura consistencia en la representación de tipos.

### Propuesta alternativa si hubiera 4 personas

Si en algún momento el equipo crece o se redistribuye:

| Persona | Cortes | Foco |
|---|---|---|
| A | 4 | Evaluador de expresiones puro |
| B | 5 + 6 | Scopes, funciones y objetos en runtime |
| C | 7 | Binding y tablas semánticas |
| D | 8 + 9 | Inferencia y type checking |

Esto separa la ejecución dinámica (A+B) de la estática (C+D) de forma más limpia.
