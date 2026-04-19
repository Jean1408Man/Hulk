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
    #include <utility>
    #include <vector>

    #include "../ast/abs_nodes/decl.h"
    #include "../ast/abs_nodes/expr.h"
    #include "../ast/assignments/desctructiveAssign.h"
    #include "../ast/assignments/destructiveAssignMember.h"
    #include "../ast/binOps/arithmeticBinOp.h"
    #include "../ast/binOps/logicBinOp.h"
    #include "../ast/binOps/stringBinOp.h"
    #include "../ast/conditionals/ifStmt.h"
    #include "../ast/domainFunctions/builtinCall.h"
    #include "../ast/domainFunctions/print.h"
    #include "../ast/functions/functionCall.h"
    #include "../ast/functions/functionDecl.h"
    #include "../ast/functions/param.h"
    #include "../ast/literales/boolean.h"
    #include "../ast/literales/number.h"
    #include "../ast/literales/string.h"
    #include "../ast/loops/for.h"
    #include "../ast/loops/while.h"
    #include "../ast/others/exprBlock.h"
    #include "../ast/others/program.h"
    #include "../ast/types/asExpr.h"
    #include "../ast/types/isExpr.h"
    #include "../ast/types/memberAccess.h"
    #include "../ast/types/methodCall.h"
    #include "../ast/types/newExpr.h"
    #include "../ast/types/typeDecl.h"
    #include "../ast/types/typeMemberAttribute.h"
    #include "../ast/types/typeMemberMethod.h"
    #include "../ast/unaryOps/arithmeticUnaryOp.h"
    #include "../ast/unaryOps/logicUnaryOp.h"
    #include "../ast/variables/letIn.h"
    #include "../ast/variables/variableBinding.h"
    #include "../ast/variables/variableReference.h"
    #include "parser_driver.hpp"

    namespace hulk::parser {
        using ExprPtr = std::unique_ptr<Hulk::Expr>;
        using ExprList = std::vector<ExprPtr>;
        using DeclPtr = std::unique_ptr<Hulk::Decl>;
        using DeclList = std::vector<DeclPtr>;
        using ProgramPtr = std::unique_ptr<Hulk::Program>;
        using BindingPtr = std::unique_ptr<Hulk::VariableBinding>;
        using BindingList = std::vector<BindingPtr>;
        using ParamList = std::vector<Hulk::Param>;
        using ElifList = std::vector<Hulk::ElifBranch>;
        using TypeMemberList = std::vector<Hulk::TypeMember>;

        struct InheritsInfo {
            std::string parentName;
            ExprList parentArgs;
            bool hasParent = false;
        };

        struct LValueTarget {
            ExprPtr object;
            std::string name;
            bool isMember = false;
        };

        struct TopLevelItems {
            DeclList decls;
            ExprList exprs;
        };
    }
}

%code provides {
    namespace hulk::parser {
        Parser::symbol_type yylex(ParserDriver& driver);
    }
}

%code {
    #define yylex() yylex(driver)

    static std::string unquote_string_literal(const std::string& lexeme) {
        if (lexeme.size() >= 2 && lexeme.front() == '"' && lexeme.back() == '"') {
            return lexeme.substr(1, lexeme.size() - 2);
        }
        return lexeme;
    }
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
%nonassoc EQUAL_EQUAL NOT_EQUAL
%nonassoc LESS LESS_EQUAL GREATER GREATER_EQUAL
%nonassoc IS AS
%left CONCAT DOUBLECONCAT
%left PLUS MINUS
%left STAR SLASH PERCENT
%right CARET
%right NOT
%right UMINUS

%type <ProgramPtr> program
%type <ExprPtr> expr let_expr if_expr while_expr for_expr assign_expr
%type <ExprPtr> logic_or logic_and equality relation type_test_expr concat additive multiplicative power unary postfix primary block
%type <DeclPtr> decl function_decl type_decl
%type <BindingList> binding_list
%type <BindingPtr> binding
%type <ExprList> expr_list args_opt arg_list block_body_opt parent_args_opt
%type <ParamList> params_opt param_list ctor_params_opt
%type <Hulk::Param> param
%type <std::string> type_expr type_ann_opt return_ann_opt
%type <ElifList> elif_clauses
%type <hulk::parser::InheritsInfo> inherits_opt
%type <Hulk::TypeMember> type_member
%type <TypeMemberList> type_member_list
%type <hulk::parser::LValueTarget> lvalue
%type <hulk::parser::TopLevelItems> top_level_items top_level_item

%%

program
    : top_level_items
      {
          if ($1.exprs.empty()) {
              driver.report_syntax_error("el programa debe contener al menos una expresion global");
              $$ = std::make_unique<Hulk::Program>(
                  std::move($1.decls),
                  std::make_unique<Hulk::ExprBlock>(ExprList {})
              );
          } else if ($1.exprs.size() == 1) {
              $$ = std::make_unique<Hulk::Program>(
                  std::move($1.decls),
                  std::move($1.exprs.front())
              );
          } else {
              $$ = std::make_unique<Hulk::Program>(
                  std::move($1.decls),
                  std::make_unique<Hulk::ExprBlock>(std::move($1.exprs))
              );
          }
          driver.set_result(std::move($$));
      }
    ;

top_level_items
    : top_level_item opt_semi
      {
          $$ = std::move($1);
      }
    | top_level_items top_level_item opt_semi
      {
          for (auto& decl : $2.decls) {
              $1.decls.push_back(std::move(decl));
          }
          for (auto& expr : $2.exprs) {
              $1.exprs.push_back(std::move(expr));
          }
          $$ = std::move($1);
      }
    ;

top_level_item
    : decl
      {
          $$ = hulk::parser::TopLevelItems {};
          $$.decls.push_back(std::move($1));
      }
    | expr
      {
          $$ = hulk::parser::TopLevelItems {};
          $$.exprs.push_back(std::move($1));
      }
    ;

opt_semi
    : SEMICOLON
    |
    ;

decl
    : function_decl
      {
          $$ = std::move($1);
      }
    | type_decl
      {
          $$ = std::move($1);
      }
    ;

function_decl
    : FUNCTION IDENTIFIER LPAREN params_opt RPAREN return_ann_opt FATARROW expr SEMICOLON
      {
          if ($6.empty()) {
              $$ = std::make_unique<Hulk::FunctionDecl>($2, std::move($4), std::move($8));
          } else {
              $$ = std::make_unique<Hulk::FunctionDecl>($2, std::move($4), $6, std::move($8));
          }
      }
    | FUNCTION IDENTIFIER LPAREN params_opt RPAREN return_ann_opt block
      {
          if ($6.empty()) {
              $$ = std::make_unique<Hulk::FunctionDecl>($2, std::move($4), std::move($7));
          } else {
              $$ = std::make_unique<Hulk::FunctionDecl>($2, std::move($4), $6, std::move($7));
          }
      }
    ;

type_decl
    : TYPE IDENTIFIER ctor_params_opt inherits_opt LBRACE type_member_list RBRACE
      {
          if ($3.empty() && !$4.hasParent) {
              $$ = std::make_unique<Hulk::TypeDecl>($2, std::move($6));
          } else if (!$3.empty() && !$4.hasParent) {
              $$ = std::make_unique<Hulk::TypeDecl>($2, std::move($3), std::move($6));
          } else if ($3.empty() && $4.hasParent && $4.parentArgs.empty()) {
              $$ = std::make_unique<Hulk::TypeDecl>($2, $4.parentName, std::move($6));
          } else {
              $$ = std::make_unique<Hulk::TypeDecl>(
                  $2,
                  std::move($3),
                  $4.parentName,
                  std::move($4.parentArgs),
                  std::move($6)
              );
          }
      }
    ;

ctor_params_opt
    : LPAREN params_opt RPAREN
      {
          $$ = std::move($2);
      }
    |
      {
          $$ = ParamList {};
      }
    ;

inherits_opt
    : INHERITS IDENTIFIER parent_args_opt
      {
          $$ = hulk::parser::InheritsInfo { $2, std::move($3), true };
      }
    |
      {
          $$ = hulk::parser::InheritsInfo {};
      }
    ;

parent_args_opt
    : LPAREN args_opt RPAREN
      {
          $$ = std::move($2);
      }
    |
      {
          $$ = ExprList {};
      }
    ;

type_member_list
    :
      {
          $$ = TypeMemberList {};
      }
    | type_member_list type_member
      {
          $1.push_back(std::move($2));
          $$ = std::move($1);
      }
    ;

type_member
    : IDENTIFIER type_ann_opt ASSIGN expr SEMICOLON
      {
          if ($2.empty()) {
              $$ = Hulk::TypeMember(
                  Hulk::TypeMember::Kind::Attribute,
                  std::make_unique<Hulk::TypeMemberAttribute>($1, std::move($4))
              );
          } else {
              $$ = Hulk::TypeMember(
                  Hulk::TypeMember::Kind::Attribute,
                  std::make_unique<Hulk::TypeMemberAttribute>($1, $2, std::move($4))
              );
          }
      }
    | IDENTIFIER LPAREN params_opt RPAREN return_ann_opt FATARROW expr SEMICOLON
      {
          if ($5.empty()) {
              $$ = Hulk::TypeMember(
                  Hulk::TypeMember::Kind::Method,
                  std::make_unique<Hulk::TypeMemberMethod>($1, std::move($3), std::move($7))
              );
          } else {
              $$ = Hulk::TypeMember(
                  Hulk::TypeMember::Kind::Method,
                  std::make_unique<Hulk::TypeMemberMethod>($1, std::move($3), $5, std::move($7))
              );
          }
      }
    | IDENTIFIER LPAREN params_opt RPAREN return_ann_opt block
      {
          if ($5.empty()) {
              $$ = Hulk::TypeMember(
                  Hulk::TypeMember::Kind::Method,
                  std::make_unique<Hulk::TypeMemberMethod>($1, std::move($3), std::move($6))
              );
          } else {
              $$ = Hulk::TypeMember(
                  Hulk::TypeMember::Kind::Method,
                  std::make_unique<Hulk::TypeMemberMethod>($1, std::move($3), $5, std::move($6))
              );
          }
      }
    ;

params_opt
    : param_list
      {
          $$ = std::move($1);
      }
    |
      {
          $$ = ParamList {};
      }
    ;

param_list
    : param
      {
          ParamList params;
          params.push_back(std::move($1));
          $$ = std::move(params);
      }
    | param_list COMMA param
      {
          $1.push_back(std::move($3));
          $$ = std::move($1);
      }
    ;

param
    : IDENTIFIER type_ann_opt
      {
          if ($2.empty()) {
              $$ = Hulk::Param($1);
          } else {
              $$ = Hulk::Param($1, $2);
          }
      }
    ;

return_ann_opt
    : COLON type_expr
      {
          $$ = std::move($2);
      }
    |
      {
          $$ = "";
      }
    ;

type_ann_opt
    : COLON type_expr
      {
          $$ = std::move($2);
      }
    |
      {
          $$ = "";
      }
    ;

type_expr
    : IDENTIFIER
      {
          $$ = std::move($1);
      }
    ;

expr
    : let_expr
      {
          $$ = std::move($1);
      }
    | if_expr
      {
          $$ = std::move($1);
      }
    | while_expr
      {
          $$ = std::move($1);
      }
    | for_expr
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
    : binding
      {
          BindingList bindings;
          bindings.push_back(std::move($1));
          $$ = std::move(bindings);
      }
    | binding_list COMMA binding
      {
          $1.push_back(std::move($3));
          $$ = std::move($1);
      }
    ;

binding
    : IDENTIFIER type_ann_opt ASSIGN expr
      {
          if ($2.empty()) {
              $$ = std::make_unique<Hulk::VariableBinding>($1, std::move($4));
          } else {
              $$ = std::make_unique<Hulk::VariableBinding>($1, $2, std::move($4));
          }
      }
    ;

if_expr
    : IF LPAREN expr RPAREN expr elif_clauses ELSE expr
      {
          $$ = std::make_unique<Hulk::IfStmt>(std::move($3), std::move($5), std::move($6), std::move($8));
      }
    ;

elif_clauses
    :
      {
          $$ = ElifList {};
      }
    | elif_clauses ELIF LPAREN expr RPAREN expr
      {
          $1.emplace_back(std::move($4), std::move($6));
          $$ = std::move($1);
      }
    ;

while_expr
    : WHILE LPAREN expr RPAREN expr
      {
          $$ = std::make_unique<Hulk::WhileStmt>(std::move($3), std::move($5));
      }
    ;

for_expr
    : FOR LPAREN IDENTIFIER IN expr RPAREN expr
      {
          $$ = std::make_unique<Hulk::For>($3, std::move($5), std::move($7));
      }
    ;

assign_expr
    : lvalue DESTRUCTIVE_ASSIGN expr
      {
          if ($1.isMember) {
              $$ = std::make_unique<Hulk::DestructiveAssignMember>(std::move($1.object), $1.name, std::move($3));
          } else {
              $$ = std::make_unique<Hulk::DestructiveAssign>($1.name, std::move($3));
          }
      }
    | logic_or
      {
          $$ = std::move($1);
      }
    ;

lvalue
    : IDENTIFIER
      {
          $$ = hulk::parser::LValueTarget { nullptr, $1, false };
      }
    | postfix DOT IDENTIFIER
      {
          $$ = hulk::parser::LValueTarget { std::move($1), $3, true };
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
    : equality EQUAL_EQUAL relation
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Equal, std::move($3)
          );
      }
    | equality NOT_EQUAL relation
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::NotEqual, std::move($3)
          );
      }
    | relation
      {
          $$ = std::move($1);
      }
    ;

relation
    : relation LESS type_test_expr
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Less, std::move($3)
          );
      }
    | relation LESS_EQUAL type_test_expr
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::LessEqual, std::move($3)
          );
      }
    | relation GREATER type_test_expr
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::Greater, std::move($3)
          );
      }
    | relation GREATER_EQUAL type_test_expr
      {
          $$ = std::make_unique<Hulk::LogicBinOp>(
              std::move($1), Hulk::LogicOp::GreaterEqual, std::move($3)
          );
      }
    | type_test_expr
      {
          $$ = std::move($1);
      }
    ;

type_test_expr
    : concat
      {
          $$ = std::move($1);
      }
    | concat IS type_expr
      {
          $$ = std::make_unique<Hulk::IsExpr>(std::move($1), $3);
      }
    | concat AS type_expr
      {
          $$ = std::make_unique<Hulk::AsExpr>(std::move($1), $3);
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
    | multiplicative PERCENT power
      {
          $$ = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move($1), Hulk::ArithmeticOp::Mod, std::move($3)
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
    | postfix LPAREN args_opt RPAREN
      {
          if (const auto* callee = dynamic_cast<const Hulk::VariableReference*>($1.get())) {
              $$ = std::make_unique<Hulk::FunctionCall>(callee->GetName(), std::move($3));
          } else if (auto* access = dynamic_cast<Hulk::MemberAccess*>($1.get())) {
              auto object = access->TakeObject();
              auto memberName = access->TakeMemberName();
              $$ = std::make_unique<Hulk::MethodCall>(std::move(object), memberName, std::move($3));
          } else {
              driver.report_syntax_error("solo se pueden invocar identificadores o accesos a metodo");
              $$ = std::move($1);
          }
      }
    | postfix DOT IDENTIFIER
      {
          $$ = std::make_unique<Hulk::MemberAccess>(std::move($1), $3);
      }
    ;

primary
    : NUMBER_LITERAL
      {
          $$ = std::make_unique<Hulk::Number>($1);
      }
    | STRING_LITERAL
      {
          $$ = std::make_unique<Hulk::String>(unquote_string_literal($1));
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
    | NEW IDENTIFIER LPAREN args_opt RPAREN
      {
          $$ = std::make_unique<Hulk::NewExpr>($2, std::move($4));
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
    | SQRT LPAREN expr RPAREN
      {
          ExprList args;
          args.push_back(std::move($3));
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Sqrt, std::move(args));
      }
    | SIN LPAREN expr RPAREN
      {
          ExprList args;
          args.push_back(std::move($3));
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Sin, std::move(args));
      }
    | COS LPAREN expr RPAREN
      {
          ExprList args;
          args.push_back(std::move($3));
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Cos, std::move(args));
      }
    | RAND LPAREN RPAREN
      {
          ExprList args;
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Rand, std::move(args));
      }
    | EXP LPAREN expr RPAREN
      {
          ExprList args;
          args.push_back(std::move($3));
          $$ = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Exp, std::move(args));
      }
    | LOG LPAREN expr COMMA expr RPAREN
      {
          ExprList args;
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

args_opt
    : arg_list
      {
          $$ = std::move($1);
      }
    |
      {
          $$ = ExprList {};
      }
    ;

arg_list
    : expr
      {
          ExprList args;
          args.push_back(std::move($1));
          $$ = std::move(args);
      }
    | arg_list COMMA expr
      {
          $1.push_back(std::move($3));
          $$ = std::move($1);
      }
    ;

block
    : LBRACE block_body_opt RBRACE
      {
          $$ = std::make_unique<Hulk::ExprBlock>(std::move($2));
      }
    ;

block_body_opt
    :
      {
          $$ = ExprList {};
      }
    | expr_list opt_semi
      {
          $$ = std::move($1);
      }
    ;

expr_list
    : expr
      {
          ExprList nodes;
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
