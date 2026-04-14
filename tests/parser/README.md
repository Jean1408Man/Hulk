# Tests del parser actual

Estos tests están contrastados contra `doc/hulk-docs.pdf`, pero representan solo el subconjunto
del lenguaje que el parser actual implementa.

## Sintaxis confirmada en la documentación

El PDF confirma, entre otras, estas construcciones:

- literales numéricos, strings y booleanos;
- `print(expr)`;
- builtins `sqrt`, `sin`, `cos`, `exp`, `log`, `rand`;
- operadores `+`, `-`, `*`, `/`, `%`, `^`;
- comparaciones `<`, `>`, `<=`, `>=`, `==`, `!=`;
- booleanos `&`, `|`, `!`;
- concatenación `@`;
- bloques `{ expr; expr; ... }`;
- `let ... in ...`;
- asignación destructiva `:=`;
- `if / elif / else`;
- `while`;
- `for`;
- funciones y tipos.

## Subconjunto que sí soporta hoy el parser

Por ahora el parser implementa y prueba:

- literales;
- `print(expr)`;
- `exp`, `log`, `rand`, `PI`, `E`;
- `+`, `-`, `*`, `/`, `^`;
- `@` y `@@`;
- `==`, `!=`, `<`, `<=`, `>`, `>=`;
- `&`, `|`, `!`;
- `let`;
- `:=`;
- bloques de expresiones.

## Sintaxis documentada pero aún no soportada por este parser

- `%`;
- `sqrt`, `sin`, `cos` como builtins unarios;
- `if / elif / else`;
- `while`;
- `for`;
- funciones;
- tipos;
- acceso a miembros, llamadas a métodos, `new`, `is`, `as`.

## Nota importante

Algunos ejemplos del PDF terminan en `;` a nivel global. El parser actual todavía está
trabajando sobre un subconjunto mínimo y no acepta todavía esa forma final en todos los
casos, así que varios tests usan la versión sin `;` para poder validar el núcleo del frontend
sin mezclar extensiones aún no cerradas.
