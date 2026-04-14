# Contrato actual lexer ↔ parser

Este documento resume la interfaz real que hoy expone el lexer para un parser, a partir del código en `src/lexer/` y `src/common/`.

## Resumen ejecutivo

El proyecto ya tiene definido casi todo el contrato del lado del lexer:

- `TokenKind`: sí existe.
- `Token`: sí existe.
- `Span` y `Position`: sí existen.
- `Lexer::next_token()`: sí existe.
- palabras reservadas vs identificadores: sí está implementado.
- operadores dobles `:=`, `==`, `!=`, `<=`, `>=`, `=>`: sí están implementados con estrategia de longest-match.

Lo que no está completo respecto a la gramática/documentación:

- `@@` aparece en la gramática y en documentos, pero no existe como token separado en el lexer actual.
- el parser todavía no expone una interfaz formal equivalente; `src/parser/parser.cpp` está vacío.

## 1. `TokenKind`

La lista actual de tokens está en [src/lexer/token_kind.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/token_kind.hpp:1).

```cpp
enum class TokenKind {
    EndOfFile,
    Error,

    Identifier,
    Number,
    String,

    True,
    False,

    Print,
    Sqrt,
    Sin,
    Cos,
    Exp,
    Log,
    Rand,
    Pi,
    E,

    Let,
    In,
    If,
    Elif,
    Else,
    While,
    For,
    Function,
    Type,
    Inherits,
    New,
    Is,
    As,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Caret,
    Assign,
    DestructiveAssign,
    EqualEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
    Not,
    Concat,
    FatArrow,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semicolon,
    Colon,
    Dot,
};
```

## 2. `Token`

La estructura actual está en [src/lexer/token.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/token.hpp:8).

```cpp
struct Token {
    TokenKind kind = TokenKind::Error;
    std::string lexeme;
    hulk::common::Span span {};
};
```

### Payload real de cada token

El lexer hoy usa un payload uniforme para todos los tokens:

- `kind`: categoría léxica.
- `lexeme`: texto original exacto del token.
- `span`: rango en el archivo fuente.

No hay payload tipado por variante.

Ejemplos:

- un número llega como `TokenKind::Number` + `lexeme = "123.45"`;
- un identificador llega como `TokenKind::Identifier` + `lexeme = "miVariable"`;
- una keyword llega como su `TokenKind` específico + su `lexeme`;
- un operador llega con su símbolo en `lexeme`.

Para un parser LALR esto es suficiente como contrato base, aunque más adelante podrían querer:

- convertir `Number` a valor numérico en parser o AST;
- separar `raw_lexeme` de valor semántico;
- distinguir más explícitamente tokens reservados de builtins.

## 3. `Position` y `Span`

Las posiciones están definidas en [src/common/position.hpp](/home/jean/School/Compilacion/Hulk/src/common/position.hpp:6):

```cpp
struct Position {
    std::size_t index = 0;
    std::size_t line = 1;
    std::size_t column = 1;
};
```

El rango léxico está en [src/common/span.hpp](/home/jean/School/Compilacion/Hulk/src/common/span.hpp:6):

```cpp
struct Span {
    Position start {};
    Position end {};
};
```

### Semántica observada

- `line` y `column` son 1-based.
- `index` es absoluto sobre el source.
- `start` se toma antes de consumir el token.
- `end` se toma después de consumirlo.

O sea: el `span` parece representar un intervalo semiabierto `[start, end)`.

Esto se deduce de [src/lexer/lexer.cpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.cpp:30) y de cómo `Cursor::advance()` actualiza la posición en [src/lexer/cursor.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/cursor.hpp:27).

## 4. Interfaz del lexer

La interfaz pública está en [src/lexer/lexer.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.hpp:11):

```cpp
class Lexer {
public:
    explicit Lexer(std::string source);

    Token next_token();
    std::vector<Token> tokenize();

    [[nodiscard]] const std::vector<hulk::common::Diagnostic>& diagnostics() const;
};
```

### Contrato útil para el parser

La función importante para un parser LR/LALR es:

```cpp
Token next_token();
```

Ese `Token` es el lookahead natural del parser.

`tokenize()` también existe, pero es más útil para depuración o pruebas que para un parser incremental.

## 5. Palabras reservadas vs identificadores

La tabla de reservadas está en [src/lexer/keywords.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/keywords.hpp:9).

### Reservadas / tratadas como no-identificadores

Booleanos:

- `true`
- `false`

Builtins y constantes globales:

- `print`
- `sqrt`
- `sin`
- `cos`
- `exp`
- `log`
- `rand`
- `PI`
- `E`

Keywords del lenguaje base:

- `let`
- `in`
- `if`
- `elif`
- `else`
- `while`
- `for`
- `function`
- `type`
- `inherits`
- `new`
- `is`
- `as`

### Regla de reconocimiento

La lógica está en [src/lexer/lexer.cpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.cpp:91):

- un identificador debe empezar por letra;
- luego puede contener letras, dígitos o `_`;
- si el lexema final está en `kKeywords`, se devuelve el `TokenKind` reservado;
- si no, se devuelve `TokenKind::Identifier`.

### Implicación importante

En esta implementación actual:

- `_nombre` no puede iniciar como identificador, porque el primer carácter debe cumplir `is_alpha(c)`.

Si quieren permitirlo, habría que cambiar el arranque del autómata.

## 6. Operadores dobles y longest-match

La lógica está en [src/lexer/lexer.cpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.cpp:177).

### Operadores dobles hoy soportados

- `:=` → `TokenKind::DestructiveAssign`
- `==` → `TokenKind::EqualEqual`
- `!=` → `TokenKind::NotEqual`
- `<=` → `TokenKind::LessEqual`
- `>=` → `TokenKind::GreaterEqual`
- `=>` → `TokenKind::FatArrow`

El lexer los reconoce antes que los operadores simples, o sea con política de longest-match.

Eso es exactamente lo que conviene para un parser LR/LALR, porque evita ambiguedades como:

- `:` seguido de `=`
- `=` seguido de `=`
- `<` seguido de `=`
- `>` seguido de `=`

## 7. Operadores simples y puntuación

Además del longest-match anterior, el lexer reconoce:

- `+`, `-`, `*`, `/`, `%`, `^`
- `=`, `<`, `>`
- `&`, `|`, `!`
- `@`
- `(`, `)`, `{`, `}`, `,`, `;`, `:`, `.`

## 8. Brecha importante: `@@`

La consigna menciona correctamente `@@` como operador doble a fijar en el contrato.

Además, la gramática y documentación del proyecto también lo esperan:

- [gramar.ebnf](/home/jean/School/Compilacion/Hulk/gramar.ebnf:156)
- [doc/analisis_gramatica_hulk_base.md](/home/jean/School/Compilacion/Hulk/doc/analisis_gramatica_hulk_base.md:405)
- [doc/lexer.md](/home/jean/School/Compilacion/Hulk/doc/lexer.md:31)

Pero en el lexer actual no existe:

- no hay `TokenKind` para `@@`;
- no hay `cursor_.match("@@")`;
- `@` se tokeniza siempre como `TokenKind::Concat`.

### Consecuencia

Hoy el lexer no satisface completamente el contrato que necesitaría la gramática base si el parser distingue `@` de `@@`.

## 9. Manejo de errores léxicos

Cuando el lexer encuentra un error:

- agrega un `Diagnostic` a `diagnostics_`;
- devuelve un `Token` con `kind = TokenKind::Error`.

Eso está implementado en [src/lexer/lexer.cpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.cpp:249) y usa [src/common/diagnostic.hpp](/home/jean/School/Compilacion/Hulk/src/common/diagnostic.hpp:6).

Los casos visibles son:

- carácter inválido;
- escape incompleto en string;
- string sin cerrar por fin de línea;
- string sin cerrar por EOF.

## 10. Estado actual del parser

En `src/parser/` solo aparece [src/parser/parser.cpp](/home/jean/School/Compilacion/Hulk/src/parser/parser.cpp:1), y actualmente está vacío.

Entonces, hoy el contrato lexer ↔ parser está definido de facto por:

- `TokenKind`
- `Token`
- `Span`
- `Position`
- `Lexer::next_token()`

Pero todavía no hay un contrato consumido por un parser implementado en el repo.

## 11. Contrato informal recomendado

Si quieren congelar una interfaz clara antes del `.y` / `.yy`, el contrato actual del repo podría expresarse así:

```cpp
enum class TokenKind;

struct Position {
    std::size_t index;
    std::size_t line;
    std::size_t column;
};

struct Span {
    Position start;
    Position end;
};

struct Token {
    TokenKind kind;
    std::string lexeme;
    Span span;
};

class Lexer {
public:
    explicit Lexer(std::string source);
    Token next_token();
};
```

## 12. Recomendaciones puntuales antes del parser LALR

Antes de implementar el parser, conviene cerrar estas decisiones:

1. Agregar `@@` al contrato léxico.
2. Decidir si `print`, `sqrt`, `sin`, etc. deben seguir siendo tokens reservados o si deberían pasar como `Identifier`.
3. Decidir si `_` puede iniciar identificadores.
4. Documentar explícitamente que `Span` es `[start, end)`.
5. Decidir cómo debe reaccionar el parser cuando reciba `TokenKind::Error`.

## 13. Conclusión

La interfaz lexer ↔ parser ya está bastante adelantada en este proyecto. El contrato base real hoy es:

- `TokenKind`
- `Token { kind, lexeme, span }`
- `Span { start, end }`
- `Position { index, line, column }`
- `Lexer::next_token()`

La brecha principal que encontré frente a la gramática es `@@`, que todavía no está implementado como token propio.
