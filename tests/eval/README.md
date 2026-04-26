# Tests del evaluador HULK

## Corte 4 — Subconjunto ejecutable (sin variables ni funciones)

| Archivo | Qué prueba |
|---|---|
| `c4_literals.hulk` | Literales: Number, Boolean, String |
| `c4_arithmetic.hulk` | Operaciones: +, -, *, /, %, ^, negación unaria |
| `c4_logic.hulk` | Lógica: and, or, not, comparaciones |
| `c4_strings.hulk` | Concatenación: @ y @@ |
| `c4_builtins.hulk` | Funciones builtin: sqrt, sin, cos, exp, log |
| `c4_block_let_if.hulk` | Bloques, let/in, if/elif/else como expresión |
| `c4_while.hulk` | While loop con :=  |

## Corte 5 — Variables, scopes y funciones

| Archivo | Qué prueba |
|---|---|
| `c5_variables.hulk` | Scopes léxicos, shadowing, asignación destructiva |
| `c5_functions.hulk` | Funciones globales, llamadas, composición |
| `c5_recursion.hulk` | Recursión: Fibonacci, factorial |

## Corte 6 — Objetos, clases y herencia

| Archivo | Qué prueba |
|---|---|
| `c6_objects_basic.hulk` | TypeDecl, new, self, atributos, métodos |
| `c6_inheritance.hulk` | inherits, override implícito, `is` en runtime |
| `c6_member_assign.hulk` | Asignación destructiva a miembros (self.x := ...) |

## Ejecutar

```bash
# Compilar evaluador
make eval

# Correr todos los tests de evaluación
make eval-tests

# Correr un test específico
./hulk_eval tests/eval/c5_recursion.hulk
```
