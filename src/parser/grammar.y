%skeleton "lalr1.cc"
%require "3.8"

%define api.namespace {hulk::parser}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor
%define parse.error detailed
%locations

%parse-param { hulk::parser::ParserDriver& driver }

%code requires {
    #include <memory>
    #include <string>
    #include <vector>

    #include "../ast/abs_nodes/expr.h"
    #include "../ast/literales/number.h"
    #include "../ast/literales/string.h"
    #include "../ast/literales/boolean.h"
    #include "../ast/variables/variableReference.h"
    #include "../ast/variables/variableBinding.h"
    #include "../ast/variables/letIn.h"
    #include "../ast/assignments/desctructiveAssign.h"
    #include "../ast/binOps/arithmeticBinOp.h"
    #include "../ast/binOps/logicBinOp.h"
    #include "../ast/binOps/stringBinOp.h"
    #include "../ast/unaryOps/arithmeticUnaryOp.h"
    #include "../ast/unaryOps/logicUnaryOp.h"
    #include "../ast/domainFunctions/print.h"
    #include "../ast/domainFunctions/builtinCall.h"
    #include "parser_driver.hpp"

    namespace hulk::parser {
        using ExprPtr = std::unique_ptr<Hulk::Expr>;
        using NodePtr = ExprPtr;
        using NodeList = std::vector<NodePtr>;
        using BindingPtr = std::unique_ptr<Hulk::VariableBinding>;
        using BindingList = std::vector<BindingPtr>;
    }
}

%code provides {
    namespace hulk::parser {
        Parser::symbol_type yylex(ParserDriver& driver);
    }
}

%code {
    #define yylex() yylex(driver)
}

%token <std::string> IDENTIFIER STRING_LITERAL ERROR_TOKEN
%token <double> NUMBER_LITERAL

%token TRUE FALSE
%token PRINT SQRT SIN COS EXP LOG RAND PI_CONST E_CONST
%token LET IN IF ELIF ELSE WHILE FOR
%token FUNCTION TYPE INHERITS NEW IS AS

%token PLUS MINUS STAR SLASH PERCENT CARET
%token ASSIGN DESTRUCTIVE_ASSIGN
%token EQUAL_EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%token AND OR NOT
%token CONCAT DOUBLECONCAT
%token FATARROW

%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON COLON DOT
%token END 0

%left OR
%left AND
%nonassoc EQUAL_EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%left CONCAT DOUBLECONCAT
%left PLUS MINUS
%left STAR SLASH
%right CARET
%right NOT
%right UMINUS

%type <NodePtr> program expr block primary
%type <NodePtr> logic_or logic_and equality concat additive multiplicative power unary postfix
%type <NodePtr> let_expr assign_expr
%type <BindingList> binding_list
%type <NodeList> expr_list

%%

program
    : expr
      {
          driver.set_result(std::move($1));
      }
    ;

expr
    : let_expr
      {
          $$ = std::move($1);
      }
    | assign_expr
      {
          $$ = std::move($1);
      }
    ;

let_expr
    : LET binding_list IN expr
      {
          $$ = std::make_unique<Hulk::LetIn>(std::move($2), std::move($4));
      }
    ;

binding_list
    : IDENTIFIER ASSIGN expr
      {
          BindingList bindings;
          bindings.push_back(
              std::make_unique<Hulk::VariableBinding>($1, std::move($3))
          );
          $$ = std::move(bindings);
      }
    | binding_list COMMA IDENTIFIER ASSIGN expr
      {
          $1.push_back(
              std::make_unique<Hulk::VariableBinding>($3, std::move($5))
          );
          $$ = std::move($1);
      }
    ;

assign_expr
    : IDENTIFIER DESTRUCTIVE_ASSIGN expr
      {
          $$ = std::make_unique<Hulk::DestructiveAssign>($1, std::move($3));
      }
    | logic_or
      {
          $$ = std::move($1);
      }
    ;

logic_or
    : logic_or OR logic_and
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Or, std::move($3)
          );
      }
    | logic_and
      {
          $$ = std::move($1);
      }
    ;

logic_and
    : logic_and AND equality
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::And, std::move($3)
          );
      }
    | equality
      {
          $$ = std::move($1);
      }
    ;

equality
    : equality EQUAL_EQUAL concat
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Equal, std::move($3)
          );
      }
    | equality NOT_EQUAL concat
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::NotEqual, std::move($3)
          );
      }
    | equality LESS concat
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Less, std::move($3)
          );
      }
    | equality LESS_EQUAL concat
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::LessEqual, std::move($3)
          );
      }
    | equality GREATER concat
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Greater, std::move($3)
          );
      }
    | equality GREATER_EQUAL concat
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::GreaterEqual, std::move($3)
          );
      }
    | concat
      {
          $$ = std::move($1);
      }
    ;

concat
    : concat CONCAT additive
      {
          $$ = std::make_unique<Hulk::StringBinOp>(
              std::move($1), Hulk::StringOp::Concat, std::move($3)
          );
      }
    | concat DOUBLECONCAT additive
      {
          $$ = std::make_unique<Hulk::StringBinOp>(
              std::move($1), Hulk::StringOp::SpaceConcat, std::move($3)
          );
      }
    | additive
      {
          $$ = std::move($1);
      }
    ;

additive
    : additive PLUS multiplicative
      {
          $$ = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move($1), Hulk::ArithmeticOp::Plus, std::move($3)
          );
      }
    | additive MINUS multiplicative
      {
          $$ = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move($1), Hulk::ArithmeticOp::Minus, std::move($3)
          );
      }
    | multiplicative
      {
          $$ = std::move($1);
      }
    ;

multiplicative
    : multiplicative STAR power
      {
          $$ = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move($1), Hulk::ArithmeticOp::Mult, std::move($3)
          );
      }
    | multiplicative SLASH power
      {
          $$ = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move($1), Hulk::ArithmeticOp::Div, std::move($3)
          );
      }
    | power
      {
          $$ = std::move($1);
      }
    ;

power
    : unary CARET power
      {
          $$ = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move($1), Hulk::ArithmeticOp::Pow, std::move($3)
          );
      }
    | unary
      {
          $$ = std::move($1);
      }
    ;

unary
    : MINUS unary %prec UMINUS
      {
          $$ = std::make_unique<Hulk::ArithmeticUnaryOp>(
              Hulk::ArithUnaryType::Minus, std::move($2)
          );
      }
    | NOT unary
      {
          $$ = std::make_unique<Hulk::LogicUnaryOp>(std::move($2));
      }
    | postfix
      {
          $$ = std::move($1);
      }
    ;

postfix
    : primary
      {
          $$ = std::move($1);
      }
    ;

primary
    : NUMBER_LITERAL
      {
          $$ = std::make_unique<Hulk::Number>($1);
      }
    | STRING_LITERAL
      {
          $$ = std::make_unique<Hulk::String>($1);
      }
    | TRUE
      {
          $$ = std::make_unique<Hulk::Boolean>(true);
      }
    | FALSE
      {
          $$ = std::make_unique<Hulk::Boolean>(false);
      }
    | IDENTIFIER
      {
          $$ = std::make_unique<Hulk::VariableReference>($1);
      }
    | LPAREN expr RPAREN
      {
          $$ = std::move($2);
      }
    | block
      {
          $$ = std::move($1);
      }
    | PRINT LPAREN expr RPAREN
      {
          $$ = std::make_unique<Hulk::Print>(std::move($3));
      }
    | RAND LPAREN RPAREN
      {
          NodeList args;
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Rand, std::move(args));
      }
    | EXP LPAREN expr RPAREN
      {
          NodeList args;
          args.push_back(std::move($3));
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Exp, std::move(args));
      }
    | LOG LPAREN expr COMMA expr RPAREN
      {
          NodeList args;
          args.push_back(std::move($3));
          args.push_back(std::move($5));
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Log, std::move(args));
      }
    | PI_CONST
      {
          $$ = std::make_unique<Hulk::Number>(3.14159265358979323846);
      }
    | E_CONST
      {
          $$ = std::make_unique<Hulk::Number>(2.71828182845904523536);
      }
    ;

block
    : LBRACE expr_list RBRACE
      {
          // AST note: ExprBlock currently expects Expr-derived nodes,
          // but many existing literal/expression nodes derive from ASTnode directly.
          // Until that hierarchy is reconciled, the parser preserves block value
          // semantics by returning the last expression of the block.
          $$ = std::move($2.back());
      }
    ;

expr_list
    : expr
      {
          NodeList nodes;
          nodes.push_back(std::move($1));
          $$ = std::move(nodes);
      }
    | expr_list SEMICOLON expr
      {
          $1.push_back(std::move($3));
          $$ = std::move($1);
      }
    ;

%%

void hulk::parser::Parser::error(const location_type& loc,
                                 const std::string& msg) {
    hulk::common::Span span {
        .start = {
            .index = 0,
            .line = static_cast<std::size_t>(loc.begin.line),
            .column = static_cast<std::size_t>(loc.begin.column),
        },
        .end = {
            .index = 0,
            .line = static_cast<std::size_t>(loc.end.line),
            .column = static_cast<std::size_t>(loc.end.column),
        },
    };

    driver.report_syntax_error(msg, span);
}
