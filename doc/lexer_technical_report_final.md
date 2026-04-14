# Informe técnico final del lexer

Este documento describe la implementación actual del lexer de HULK a partir del código en `src/lexer/` y `src/common/`. Su objetivo es dejar una referencia técnica final sobre:

- la arquitectura del scanner;
- el contrato léxico que expone al parser;
- la estructura de tokens y spans;
- la clasificación de palabras reservadas, operadores y delimitadores;
- la estrategia de reconocimiento;
- el manejo de errores y diagnósticos.

## 1. Vista general

El lexer está implementado manualmente, sin generadores externos, y se apoya en un conjunto pequeño de componentes bien separados:

- `token_kind.hpp`: enum con las categorías léxicas;
- `token.hpp`: estructura uniforme del token;
- `cursor.hpp`: navegación del input con control de posición;
- `keywords.hpp`: tabla de palabras reservadas;
- `lexer.hpp`: interfaz pública del scanner;
- `lexer.cpp`: lógica concreta de reconocimiento;
- `position.hpp` y `span.hpp`: ubicación de tokens y errores;
- `diagnostic.hpp` y `diagnosticEngine.hpp`: infraestructura de reporte.

La implementación usa una estrategia híbrida:

- reconocimiento por categorías para los casos simples;
- pequeños autómatas implícitos para números, strings y operadores compuestos.

## 2. Arquitectura léxica

### 2.1 `TokenKind`

Archivo: [src/lexer/token_kind.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/token_kind.hpp:1)

El contrato léxico actual clasifica los símbolos en:

- especiales;
- identificadores y literales;
- booleanos;
- builtins y constantes globales;
- keywords del lenguaje base;
- operadores;
- delimitadores y puntuación.

Código:

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
    DoubleConcat,
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

Observación importante:

- `@@` ya forma parte del contrato léxico mediante `TokenKind::DoubleConcat`.

### 2.2 `Token`

Archivo: [src/lexer/token.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/token.hpp:1)

Todos los tokens comparten el mismo payload:

- `kind`: categoría léxica;
- `lexeme`: texto exacto del token;
- `span`: rango dentro del source.

Código:

```cpp
struct Token {
    TokenKind kind = TokenKind::Error;
    std::string lexeme;
    hulk::common::Span span {};
};
```

### 2.3 `Position` y `Span`

Archivos:

- [src/common/position.hpp](/home/jean/School/Compilacion/Hulk/src/common/position.hpp:1)
- [src/common/span.hpp](/home/jean/School/Compilacion/Hulk/src/common/span.hpp:1)

Código:

```cpp
struct Position {
    std::size_t index = 0;
    std::size_t line = 1;
    std::size_t column = 1;
};
```

```cpp
struct Span {
    Position start {};
    Position end {};
};
```

Semántica observable:

- `line` y `column` son 1-based;
- `index` es absoluto;
- `start` se registra antes de consumir el lexema;
- `end` se registra justo después de consumirlo.

En términos prácticos, el `span` representa un intervalo semiabierto `[start, end)`.

### 2.4 `Cursor`

Archivo: [src/lexer/cursor.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/cursor.hpp:1)

`Cursor` encapsula el recorrido del texto de entrada y evita mezclar navegación con lógica léxica.

Operaciones principales:

- `eof()`;
- `peek(offset)`;
- `advance()`;
- `match(char)`;
- `match(std::string_view)`;
- `position()`.

Código:

```cpp
class Cursor {
public:
    explicit Cursor(std::string source)
        : source_(std::move(source)) {}

    [[nodiscard]] bool eof() const;
    [[nodiscard]] char peek(std::size_t offset = 0) const;
    char advance();
    bool match(char expected);
    bool match(std::string_view expected);
    [[nodiscard]] hulk::common::Position position() const;

private:
    std::string source_;
    hulk::common::Position pos_ {};
};
```

Punto técnico importante:

- `advance()` actualiza línea y columna automáticamente cuando consume `\n`.

## 3. Diagnósticos

### 3.1 `Diagnostic`

Archivo: [src/common/diagnostic.hpp](/home/jean/School/Compilacion/Hulk/src/common/diagnostic.hpp:1)

Código:

```cpp
enum class DiagnosticLevel {
    Lexical,
    Syntactic,
    Semantic,
};

enum class Severity {
    Error,
    Warning,
};

struct Diagnostic {
    DiagnosticLevel level = DiagnosticLevel::Lexical;
    Severity severity = Severity::Error;
    std::string message;
    Span span {};
};
```

### 3.2 `DiagnosticEngine`

Archivo: [src/common/diagnosticEngine.hpp](/home/jean/School/Compilacion/Hulk/src/common/diagnosticEngine.hpp:1)

El lexer ya no gestiona una lista privada de diagnósticos, sino que reporta sobre un `DiagnosticEngine` externo. Esto permite:

- centralizar errores de varias fases;
- reutilizar el mismo repositorio de mensajes;
- desacoplar el lexer del almacenamiento concreto de diagnósticos.

El método principal usado por el lexer es:

```cpp
engine_.report(error_id,
               hulk::common::DiagnosticLevel::Lexical,
               hulk::common::Severity::Error,
               span,
               lexeme);
```

## 4. Interfaz pública del lexer

Archivo: [src/lexer/lexer.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.hpp:1)

Código:

```cpp
class Lexer {
public:
    explicit Lexer(std::string source, hulk::common::DiagnosticEngine& engine);

    Token next_token();
    std::vector<Token> tokenize();

    [[nodiscard]] const std::vector<hulk::common::Diagnostic>& diagnostics() const {
        return engine_.diagnostics();
    }

private:
    Cursor cursor_;
    hulk::common::DiagnosticEngine& engine_;

    void skip_whitespace_and_comments();

    Token scan_identifier_or_keyword();
    Token scan_number();
    Token scan_string();
    Token scan_operator_or_delimiter();

    Token make_token(TokenKind kind, const std::string& lexeme,
                     hulk::common::Position start,
                     hulk::common::Position end);

    Token error_token(const std::string& error_id,
                      hulk::common::Position start,
                      hulk::common::Position end,
                      const std::string& lexeme);
};
```

### Contrato con el parser

La operación central del lexer es:

```cpp
Token next_token();
```

Ese `Token` es el lookahead natural para un parser LR/LALR.

También existe:

```cpp
std::vector<Token> tokenize();
```

que resulta útil para:

- depuración;
- pruebas;
- inspección completa de la secuencia léxica.

## 5. Palabras reservadas

Archivo: [src/lexer/keywords.hpp](/home/jean/School/Compilacion/Hulk/src/lexer/keywords.hpp:1)

### 5.1 Booleanos

- `true`
- `false`

### 5.2 Builtins y constantes reservadas léxicamente

- `print`
- `sqrt`
- `sin`
- `cos`
- `exp`
- `log`
- `rand`
- `PI`
- `E`

### 5.3 Keywords del lenguaje base

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

Código:

```cpp
inline const std::unordered_map<std::string_view, TokenKind> kKeywords = {
    {"true", TokenKind::True},
    {"false", TokenKind::False},
    {"print", TokenKind::Print},
    {"sqrt", TokenKind::Sqrt},
    {"sin", TokenKind::Sin},
    {"cos", TokenKind::Cos},
    {"exp", TokenKind::Exp},
    {"log", TokenKind::Log},
    {"rand", TokenKind::Rand},
    {"PI", TokenKind::Pi},
    {"E", TokenKind::E},
    {"let", TokenKind::Let},
    {"in", TokenKind::In},
    {"if", TokenKind::If},
    {"elif", TokenKind::Elif},
    {"else", TokenKind::Else},
    {"while", TokenKind::While},
    {"for", TokenKind::For},
    {"function", TokenKind::Function},
    {"type", TokenKind::Type},
    {"inherits", TokenKind::Inherits},
    {"new", TokenKind::New},
    {"is", TokenKind::Is},
    {"as", TokenKind::As},
};
```

## 6. Estrategia de reconocimiento

Archivo principal: [src/lexer/lexer.cpp](/home/jean/School/Compilacion/Hulk/src/lexer/lexer.cpp:1)

### 6.1 Flujo general

`next_token()` sigue este flujo:

1. consumir whitespace y comentarios;
2. si hay EOF, devolver `EndOfFile`;
3. si empieza por letra, escanear identificador o keyword;
4. si empieza por dígito, escanear número;
5. si empieza por `"`, escanear string;
6. en otro caso, escanear operador o delimitador.

Código esencial:

```cpp
Token Lexer::next_token() {
    skip_whitespace_and_comments();

    const hulk::common::Position start = cursor_.position();

    if (cursor_.eof()) {
        return make_token(TokenKind::EndOfFile, "", start, start);
    }

    const char c = cursor_.peek();

    if (is_alpha(c)) {
        return scan_identifier_or_keyword();
    }

    if (is_digit(c)) {
        return scan_number();
    }

    if (c == '"') {
        return scan_string();
    }

    return scan_operator_or_delimiter();
}
```

### 6.2 Trivia

`skip_whitespace_and_comments()` consume:

- espacios;
- tabs;
- carriage returns;
- saltos de línea;
- comentarios de línea con `//`.

Ni whitespace ni comentarios generan tokens.

### 6.3 Identificadores y keywords

Regla implementada:

- el primer carácter debe ser letra;
- los siguientes pueden ser letra, dígito o `_`.

Autómata implícito:

```text
S0 --[letter]--> S1
S1 --[letter|digit|_]--> S1
S1 --[otro]--> ACCEPT
```

Observación:

- `_nombre` no es válido como identificador inicial en esta implementación, porque el arranque exige `is_alpha(c)`.

### 6.4 Números

Soporta:

- enteros;
- decimales simples.

Regla:

- consume una secuencia de dígitos;
- si encuentra `.` seguido por otro dígito, entra en la parte decimal;
- si `.` no va seguido de dígito, el punto no se absorbe como parte del número.

Autómata implícito:

```text
S0 --[digit]--> S1
S1 --[digit]--> S1
S1 --[.]--> S2
S2 --[digit]--> S3
S3 --[digit]--> S3
```

### 6.5 Strings

Soporta:

- comilla de apertura;
- contenido arbitrario;
- secuencias de escape simples;
- cierre correcto con `"`.

Errores reportados:

- `INCOMPLETE_ESCAPE`
- `UNTERMINATED_STRING_EOL`
- `UNTERMINATED_STRING_EOF`

### 6.6 Operadores y delimitadores

El lexer usa política de longest-match para operadores compuestos. En el orden actual del código, intenta primero:

- `:=`
- `==`
- `!=`
- `<=`
- `>=`
- `=>`
- `@@`

Luego clasifica operadores simples y signos con `switch`.

Operadores simples actuales:

- `+`
- `-`
- `*`
- `/`
- `%`
- `^`
- `=`
- `<`
- `>`
- `&`
- `|`
- `!`
- `@`

Puntuación y delimitadores:

- `(`
- `)`
- `{`
- `}`
- `,`
- `;`
- `:`
- `.`

### 6.7 Reconocimiento de `@@`

La incorporación de `@@` quedó implementada así:

```cpp
if (cursor_.match("@@")) {
    return make_token(TokenKind::DoubleConcat, "@@", start, cursor_.position());
}
```

Esto alinea el lexer con:

- la gramática en [gramar.ebnf](/home/jean/School/Compilacion/Hulk/gramar.ebnf:157);
- el AST, que ya contempla `StringOp::SpaceConcat` en [src/ast/binOps/stringBinOp.h](/home/jean/School/Compilacion/Hulk/src/ast/binOps/stringBinOp.h:8).

## 7. Manejo de errores

Cuando ocurre un error léxico, el lexer:

1. construye un `Span`;
2. reporta el error al `DiagnosticEngine`;
3. devuelve un `Token` con `kind = TokenKind::Error`.

Código:

```cpp
Token Lexer::error_token(const std::string& error_id,
                         hulk::common::Position start,
                         hulk::common::Position end,
                         const std::string& lexeme) {
    const hulk::common::Span span { .start = start, .end = end };

    engine_.report(error_id,
                   hulk::common::DiagnosticLevel::Lexical,
                   hulk::common::Severity::Error,
                   span,
                   lexeme);

    return Token {
        .kind   = TokenKind::Error,
        .lexeme = lexeme,
        .span   = span,
    };
}
```

IDs de error visibles en la implementación:

- `INVALID_CHAR`
- `INCOMPLETE_ESCAPE`
- `UNTERMINATED_STRING_EOL`
- `UNTERMINATED_STRING_EOF`

## 8. Contrato léxico final verificado

Con base en el código actual del workspace, el contrato léxico final queda compuesto por:

- `TokenKind`;
- `Token`;
- `Position`;
- `Span`;
- `Lexer::next_token()`;
- `Lexer::tokenize()`;
- integración con `DiagnosticEngine`.

Los operadores dobles efectivamente soportados son:

- `:=`
- `==`
- `!=`
- `<=`
- `>=`
- `=>`
- `@@`

## 9. Evaluación técnica del diseño

### Fortalezas

- implementación compacta y clara;
- separación correcta entre cursor, tokens y diagnósticos;
- spans completos para tokens y errores;
- contrato adecuado para un parser LR/LALR;
- longest-match correcto para operadores dobles;
- integración más limpia con `DiagnosticEngine`.

### Limitaciones actuales

- los identificadores no pueden comenzar con `_`;
- builtins como `print`, `sqrt`, `sin`, etc. están reservados léxicamente;
- el payload de `Token` es uniforme y no tipado por categoría;
- el nombre `DoubleConcat` funciona técnicamente, aunque podría renombrarse si más adelante quieren alinear mejor lexer y AST con algo como `SpaceConcat`.

## 10. Conclusión

La versión actual del lexer de HULK ya define un contrato léxico sólido y consistente con la gramática base y con el AST. En particular, la incorporación de `@@` cierra una brecha importante entre frontend léxico, gramática y representación sintáctica.

Con `TokenKind`, `Token`, `Span`, `next_token()` y el sistema de diagnósticos ya estabilizados, el lexer está en una buena posición para servir como base del parser y del resto del frontend del compilador.
