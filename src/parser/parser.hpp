// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


/**
 ** \file src/parser/parser.hpp
 ** Define the hulk::parser::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_SRC_PARSER_PARSER_HPP_INCLUDED
# define YY_YY_SRC_PARSER_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 13 "src/parser/grammar.y"

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

#line 121 "src/parser/parser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 4 "src/parser/grammar.y"
namespace hulk { namespace parser {
#line 257 "src/parser/parser.hpp"




  /// A Bison parser.
  class Parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // binding_list
      char dummy1[sizeof (BindingList)];

      // binding
      char dummy2[sizeof (BindingPtr)];

      // decl
      // function_decl
      // type_decl
      char dummy3[sizeof (DeclPtr)];

      // elif_clauses
      char dummy4[sizeof (ElifList)];

      // parent_args_opt
      // args_opt
      // arg_list
      // block_body_opt
      // expr_list
      char dummy5[sizeof (ExprList)];

      // expr
      // let_expr
      // if_expr
      // while_expr
      // for_expr
      // assign_expr
      // logic_or
      // logic_and
      // equality
      // relation
      // type_test_expr
      // concat
      // additive
      // multiplicative
      // power
      // unary
      // postfix
      // primary
      // block
      char dummy6[sizeof (ExprPtr)];

      // param
      char dummy7[sizeof (Hulk::Param)];

      // type_member
      char dummy8[sizeof (Hulk::TypeMember)];

      // ctor_params_opt
      // params_opt
      // param_list
      char dummy9[sizeof (ParamList)];

      // program
      char dummy10[sizeof (ProgramPtr)];

      // type_member_list
      char dummy11[sizeof (TypeMemberList)];

      // NUMBER_LITERAL
      char dummy12[sizeof (double)];

      // inherits_opt
      char dummy13[sizeof (hulk::parser::InheritsInfo)];

      // lvalue
      char dummy14[sizeof (hulk::parser::LValueTarget)];

      // top_level_items
      // top_level_item
      char dummy15[sizeof (hulk::parser::TopLevelItems)];

      // IDENTIFIER
      // STRING_LITERAL
      // ERROR_TOKEN
      // return_ann_opt
      // type_ann_opt
      // type_expr
      char dummy16[sizeof (std::string)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        YYEMPTY = -2,
    END = 0,                       // END
    YYerror = 256,                 // error
    YYUNDEF = 257,                 // "invalid token"
    IDENTIFIER = 258,              // IDENTIFIER
    STRING_LITERAL = 259,          // STRING_LITERAL
    ERROR_TOKEN = 260,             // ERROR_TOKEN
    NUMBER_LITERAL = 261,          // NUMBER_LITERAL
    TRUE = 262,                    // TRUE
    FALSE = 263,                   // FALSE
    PRINT = 264,                   // PRINT
    SQRT = 265,                    // SQRT
    SIN = 266,                     // SIN
    COS = 267,                     // COS
    EXP = 268,                     // EXP
    LOG = 269,                     // LOG
    RAND = 270,                    // RAND
    PI_CONST = 271,                // PI_CONST
    E_CONST = 272,                 // E_CONST
    LET = 273,                     // LET
    IN = 274,                      // IN
    IF = 275,                      // IF
    ELIF = 276,                    // ELIF
    ELSE = 277,                    // ELSE
    WHILE = 278,                   // WHILE
    FOR = 279,                     // FOR
    FUNCTION = 280,                // FUNCTION
    TYPE = 281,                    // TYPE
    INHERITS = 282,                // INHERITS
    NEW = 283,                     // NEW
    IS = 284,                      // IS
    AS = 285,                      // AS
    PLUS = 286,                    // PLUS
    MINUS = 287,                   // MINUS
    STAR = 288,                    // STAR
    SLASH = 289,                   // SLASH
    PERCENT = 290,                 // PERCENT
    CARET = 291,                   // CARET
    ASSIGN = 292,                  // ASSIGN
    DESTRUCTIVE_ASSIGN = 293,      // DESTRUCTIVE_ASSIGN
    EQUAL_EQUAL = 294,             // EQUAL_EQUAL
    NOT_EQUAL = 295,               // NOT_EQUAL
    LESS = 296,                    // LESS
    LESS_EQUAL = 297,              // LESS_EQUAL
    GREATER = 298,                 // GREATER
    GREATER_EQUAL = 299,           // GREATER_EQUAL
    AND = 300,                     // AND
    OR = 301,                      // OR
    NOT = 302,                     // NOT
    CONCAT = 303,                  // CONCAT
    DOUBLECONCAT = 304,            // DOUBLECONCAT
    FATARROW = 305,                // FATARROW
    LPAREN = 306,                  // LPAREN
    RPAREN = 307,                  // RPAREN
    LBRACE = 308,                  // LBRACE
    RBRACE = 309,                  // RBRACE
    COMMA = 310,                   // COMMA
    SEMICOLON = 311,               // SEMICOLON
    COLON = 312,                   // COLON
    DOT = 313,                     // DOT
    UMINUS = 314                   // UMINUS
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 60, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // END
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_IDENTIFIER = 3,                        // IDENTIFIER
        S_STRING_LITERAL = 4,                    // STRING_LITERAL
        S_ERROR_TOKEN = 5,                       // ERROR_TOKEN
        S_NUMBER_LITERAL = 6,                    // NUMBER_LITERAL
        S_TRUE = 7,                              // TRUE
        S_FALSE = 8,                             // FALSE
        S_PRINT = 9,                             // PRINT
        S_SQRT = 10,                             // SQRT
        S_SIN = 11,                              // SIN
        S_COS = 12,                              // COS
        S_EXP = 13,                              // EXP
        S_LOG = 14,                              // LOG
        S_RAND = 15,                             // RAND
        S_PI_CONST = 16,                         // PI_CONST
        S_E_CONST = 17,                          // E_CONST
        S_LET = 18,                              // LET
        S_IN = 19,                               // IN
        S_IF = 20,                               // IF
        S_ELIF = 21,                             // ELIF
        S_ELSE = 22,                             // ELSE
        S_WHILE = 23,                            // WHILE
        S_FOR = 24,                              // FOR
        S_FUNCTION = 25,                         // FUNCTION
        S_TYPE = 26,                             // TYPE
        S_INHERITS = 27,                         // INHERITS
        S_NEW = 28,                              // NEW
        S_IS = 29,                               // IS
        S_AS = 30,                               // AS
        S_PLUS = 31,                             // PLUS
        S_MINUS = 32,                            // MINUS
        S_STAR = 33,                             // STAR
        S_SLASH = 34,                            // SLASH
        S_PERCENT = 35,                          // PERCENT
        S_CARET = 36,                            // CARET
        S_ASSIGN = 37,                           // ASSIGN
        S_DESTRUCTIVE_ASSIGN = 38,               // DESTRUCTIVE_ASSIGN
        S_EQUAL_EQUAL = 39,                      // EQUAL_EQUAL
        S_NOT_EQUAL = 40,                        // NOT_EQUAL
        S_LESS = 41,                             // LESS
        S_LESS_EQUAL = 42,                       // LESS_EQUAL
        S_GREATER = 43,                          // GREATER
        S_GREATER_EQUAL = 44,                    // GREATER_EQUAL
        S_AND = 45,                              // AND
        S_OR = 46,                               // OR
        S_NOT = 47,                              // NOT
        S_CONCAT = 48,                           // CONCAT
        S_DOUBLECONCAT = 49,                     // DOUBLECONCAT
        S_FATARROW = 50,                         // FATARROW
        S_LPAREN = 51,                           // LPAREN
        S_RPAREN = 52,                           // RPAREN
        S_LBRACE = 53,                           // LBRACE
        S_RBRACE = 54,                           // RBRACE
        S_COMMA = 55,                            // COMMA
        S_SEMICOLON = 56,                        // SEMICOLON
        S_COLON = 57,                            // COLON
        S_DOT = 58,                              // DOT
        S_UMINUS = 59,                           // UMINUS
        S_YYACCEPT = 60,                         // $accept
        S_program = 61,                          // program
        S_top_level_items = 62,                  // top_level_items
        S_top_level_item = 63,                   // top_level_item
        S_opt_semi = 64,                         // opt_semi
        S_decl = 65,                             // decl
        S_function_decl = 66,                    // function_decl
        S_type_decl = 67,                        // type_decl
        S_ctor_params_opt = 68,                  // ctor_params_opt
        S_inherits_opt = 69,                     // inherits_opt
        S_parent_args_opt = 70,                  // parent_args_opt
        S_type_member_list = 71,                 // type_member_list
        S_type_member = 72,                      // type_member
        S_params_opt = 73,                       // params_opt
        S_param_list = 74,                       // param_list
        S_param = 75,                            // param
        S_return_ann_opt = 76,                   // return_ann_opt
        S_type_ann_opt = 77,                     // type_ann_opt
        S_type_expr = 78,                        // type_expr
        S_expr = 79,                             // expr
        S_let_expr = 80,                         // let_expr
        S_binding_list = 81,                     // binding_list
        S_binding = 82,                          // binding
        S_if_expr = 83,                          // if_expr
        S_elif_clauses = 84,                     // elif_clauses
        S_while_expr = 85,                       // while_expr
        S_for_expr = 86,                         // for_expr
        S_assign_expr = 87,                      // assign_expr
        S_lvalue = 88,                           // lvalue
        S_logic_or = 89,                         // logic_or
        S_logic_and = 90,                        // logic_and
        S_equality = 91,                         // equality
        S_relation = 92,                         // relation
        S_type_test_expr = 93,                   // type_test_expr
        S_concat = 94,                           // concat
        S_additive = 95,                         // additive
        S_multiplicative = 96,                   // multiplicative
        S_power = 97,                            // power
        S_unary = 98,                            // unary
        S_postfix = 99,                          // postfix
        S_primary = 100,                         // primary
        S_args_opt = 101,                        // args_opt
        S_arg_list = 102,                        // arg_list
        S_block = 103,                           // block
        S_block_body_opt = 104,                  // block_body_opt
        S_expr_list = 105                        // expr_list
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.move< BindingList > (std::move (that.value));
        break;

      case symbol_kind::S_binding: // binding
        value.move< BindingPtr > (std::move (that.value));
        break;

      case symbol_kind::S_decl: // decl
      case symbol_kind::S_function_decl: // function_decl
      case symbol_kind::S_type_decl: // type_decl
        value.move< DeclPtr > (std::move (that.value));
        break;

      case symbol_kind::S_elif_clauses: // elif_clauses
        value.move< ElifList > (std::move (that.value));
        break;

      case symbol_kind::S_parent_args_opt: // parent_args_opt
      case symbol_kind::S_args_opt: // args_opt
      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_block_body_opt: // block_body_opt
      case symbol_kind::S_expr_list: // expr_list
        value.move< ExprList > (std::move (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_if_expr: // if_expr
      case symbol_kind::S_while_expr: // while_expr
      case symbol_kind::S_for_expr: // for_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_relation: // relation
      case symbol_kind::S_type_test_expr: // type_test_expr
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.move< ExprPtr > (std::move (that.value));
        break;

      case symbol_kind::S_param: // param
        value.move< Hulk::Param > (std::move (that.value));
        break;

      case symbol_kind::S_type_member: // type_member
        value.move< Hulk::TypeMember > (std::move (that.value));
        break;

      case symbol_kind::S_ctor_params_opt: // ctor_params_opt
      case symbol_kind::S_params_opt: // params_opt
      case symbol_kind::S_param_list: // param_list
        value.move< ParamList > (std::move (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< ProgramPtr > (std::move (that.value));
        break;

      case symbol_kind::S_type_member_list: // type_member_list
        value.move< TypeMemberList > (std::move (that.value));
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.move< double > (std::move (that.value));
        break;

      case symbol_kind::S_inherits_opt: // inherits_opt
        value.move< hulk::parser::InheritsInfo > (std::move (that.value));
        break;

      case symbol_kind::S_lvalue: // lvalue
        value.move< hulk::parser::LValueTarget > (std::move (that.value));
        break;

      case symbol_kind::S_top_level_items: // top_level_items
      case symbol_kind::S_top_level_item: // top_level_item
        value.move< hulk::parser::TopLevelItems > (std::move (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
      case symbol_kind::S_return_ann_opt: // return_ann_opt
      case symbol_kind::S_type_ann_opt: // type_ann_opt
      case symbol_kind::S_type_expr: // type_expr
        value.move< std::string > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BindingList&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BindingList& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BindingPtr&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BindingPtr& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DeclPtr&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DeclPtr& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ElifList&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ElifList& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ExprList&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ExprList& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ExprPtr&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ExprPtr& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Hulk::Param&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Hulk::Param& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Hulk::TypeMember&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Hulk::TypeMember& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ParamList&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ParamList& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ProgramPtr&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ProgramPtr& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, TypeMemberList&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const TypeMemberList& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, double&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const double& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, hulk::parser::InheritsInfo&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const hulk::parser::InheritsInfo& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, hulk::parser::LValueTarget&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const hulk::parser::LValueTarget& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, hulk::parser::TopLevelItems&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const hulk::parser::TopLevelItems& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_binding_list: // binding_list
        value.template destroy< BindingList > ();
        break;

      case symbol_kind::S_binding: // binding
        value.template destroy< BindingPtr > ();
        break;

      case symbol_kind::S_decl: // decl
      case symbol_kind::S_function_decl: // function_decl
      case symbol_kind::S_type_decl: // type_decl
        value.template destroy< DeclPtr > ();
        break;

      case symbol_kind::S_elif_clauses: // elif_clauses
        value.template destroy< ElifList > ();
        break;

      case symbol_kind::S_parent_args_opt: // parent_args_opt
      case symbol_kind::S_args_opt: // args_opt
      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_block_body_opt: // block_body_opt
      case symbol_kind::S_expr_list: // expr_list
        value.template destroy< ExprList > ();
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_if_expr: // if_expr
      case symbol_kind::S_while_expr: // while_expr
      case symbol_kind::S_for_expr: // for_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_relation: // relation
      case symbol_kind::S_type_test_expr: // type_test_expr
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.template destroy< ExprPtr > ();
        break;

      case symbol_kind::S_param: // param
        value.template destroy< Hulk::Param > ();
        break;

      case symbol_kind::S_type_member: // type_member
        value.template destroy< Hulk::TypeMember > ();
        break;

      case symbol_kind::S_ctor_params_opt: // ctor_params_opt
      case symbol_kind::S_params_opt: // params_opt
      case symbol_kind::S_param_list: // param_list
        value.template destroy< ParamList > ();
        break;

      case symbol_kind::S_program: // program
        value.template destroy< ProgramPtr > ();
        break;

      case symbol_kind::S_type_member_list: // type_member_list
        value.template destroy< TypeMemberList > ();
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.template destroy< double > ();
        break;

      case symbol_kind::S_inherits_opt: // inherits_opt
        value.template destroy< hulk::parser::InheritsInfo > ();
        break;

      case symbol_kind::S_lvalue: // lvalue
        value.template destroy< hulk::parser::LValueTarget > ();
        break;

      case symbol_kind::S_top_level_items: // top_level_items
      case symbol_kind::S_top_level_item: // top_level_item
        value.template destroy< hulk::parser::TopLevelItems > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
      case symbol_kind::S_return_ann_opt: // return_ann_opt
      case symbol_kind::S_type_ann_opt: // type_ann_opt
      case symbol_kind::S_type_expr: // type_expr
        value.template destroy< std::string > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return Parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, double v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const double& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    Parser (hulk::parser::ParserDriver& driver_yyarg);
    virtual ~Parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    Parser (const Parser&) = delete;
    /// Non copyable.
    Parser& operator= (const Parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_END (location_type l)
      {
        return symbol_type (token::END, std::move (l));
      }
#else
      static
      symbol_type
      make_END (const location_type& l)
      {
        return symbol_type (token::END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IDENTIFIER (std::string v, location_type l)
      {
        return symbol_type (token::IDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IDENTIFIER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::IDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING_LITERAL (std::string v, location_type l)
      {
        return symbol_type (token::STRING_LITERAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING_LITERAL (const std::string& v, const location_type& l)
      {
        return symbol_type (token::STRING_LITERAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ERROR_TOKEN (std::string v, location_type l)
      {
        return symbol_type (token::ERROR_TOKEN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ERROR_TOKEN (const std::string& v, const location_type& l)
      {
        return symbol_type (token::ERROR_TOKEN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NUMBER_LITERAL (double v, location_type l)
      {
        return symbol_type (token::NUMBER_LITERAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NUMBER_LITERAL (const double& v, const location_type& l)
      {
        return symbol_type (token::NUMBER_LITERAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TRUE (location_type l)
      {
        return symbol_type (token::TRUE, std::move (l));
      }
#else
      static
      symbol_type
      make_TRUE (const location_type& l)
      {
        return symbol_type (token::TRUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FALSE (location_type l)
      {
        return symbol_type (token::FALSE, std::move (l));
      }
#else
      static
      symbol_type
      make_FALSE (const location_type& l)
      {
        return symbol_type (token::FALSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PRINT (location_type l)
      {
        return symbol_type (token::PRINT, std::move (l));
      }
#else
      static
      symbol_type
      make_PRINT (const location_type& l)
      {
        return symbol_type (token::PRINT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SQRT (location_type l)
      {
        return symbol_type (token::SQRT, std::move (l));
      }
#else
      static
      symbol_type
      make_SQRT (const location_type& l)
      {
        return symbol_type (token::SQRT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIN (location_type l)
      {
        return symbol_type (token::SIN, std::move (l));
      }
#else
      static
      symbol_type
      make_SIN (const location_type& l)
      {
        return symbol_type (token::SIN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COS (location_type l)
      {
        return symbol_type (token::COS, std::move (l));
      }
#else
      static
      symbol_type
      make_COS (const location_type& l)
      {
        return symbol_type (token::COS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXP (location_type l)
      {
        return symbol_type (token::EXP, std::move (l));
      }
#else
      static
      symbol_type
      make_EXP (const location_type& l)
      {
        return symbol_type (token::EXP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOG (location_type l)
      {
        return symbol_type (token::LOG, std::move (l));
      }
#else
      static
      symbol_type
      make_LOG (const location_type& l)
      {
        return symbol_type (token::LOG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RAND (location_type l)
      {
        return symbol_type (token::RAND, std::move (l));
      }
#else
      static
      symbol_type
      make_RAND (const location_type& l)
      {
        return symbol_type (token::RAND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PI_CONST (location_type l)
      {
        return symbol_type (token::PI_CONST, std::move (l));
      }
#else
      static
      symbol_type
      make_PI_CONST (const location_type& l)
      {
        return symbol_type (token::PI_CONST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_E_CONST (location_type l)
      {
        return symbol_type (token::E_CONST, std::move (l));
      }
#else
      static
      symbol_type
      make_E_CONST (const location_type& l)
      {
        return symbol_type (token::E_CONST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LET (location_type l)
      {
        return symbol_type (token::LET, std::move (l));
      }
#else
      static
      symbol_type
      make_LET (const location_type& l)
      {
        return symbol_type (token::LET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IN (location_type l)
      {
        return symbol_type (token::IN, std::move (l));
      }
#else
      static
      symbol_type
      make_IN (const location_type& l)
      {
        return symbol_type (token::IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ELIF (location_type l)
      {
        return symbol_type (token::ELIF, std::move (l));
      }
#else
      static
      symbol_type
      make_ELIF (const location_type& l)
      {
        return symbol_type (token::ELIF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ELSE (location_type l)
      {
        return symbol_type (token::ELSE, std::move (l));
      }
#else
      static
      symbol_type
      make_ELSE (const location_type& l)
      {
        return symbol_type (token::ELSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WHILE (location_type l)
      {
        return symbol_type (token::WHILE, std::move (l));
      }
#else
      static
      symbol_type
      make_WHILE (const location_type& l)
      {
        return symbol_type (token::WHILE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FOR (location_type l)
      {
        return symbol_type (token::FOR, std::move (l));
      }
#else
      static
      symbol_type
      make_FOR (const location_type& l)
      {
        return symbol_type (token::FOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FUNCTION (location_type l)
      {
        return symbol_type (token::FUNCTION, std::move (l));
      }
#else
      static
      symbol_type
      make_FUNCTION (const location_type& l)
      {
        return symbol_type (token::FUNCTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TYPE (location_type l)
      {
        return symbol_type (token::TYPE, std::move (l));
      }
#else
      static
      symbol_type
      make_TYPE (const location_type& l)
      {
        return symbol_type (token::TYPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INHERITS (location_type l)
      {
        return symbol_type (token::INHERITS, std::move (l));
      }
#else
      static
      symbol_type
      make_INHERITS (const location_type& l)
      {
        return symbol_type (token::INHERITS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NEW (location_type l)
      {
        return symbol_type (token::NEW, std::move (l));
      }
#else
      static
      symbol_type
      make_NEW (const location_type& l)
      {
        return symbol_type (token::NEW, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IS (location_type l)
      {
        return symbol_type (token::IS, std::move (l));
      }
#else
      static
      symbol_type
      make_IS (const location_type& l)
      {
        return symbol_type (token::IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AS (location_type l)
      {
        return symbol_type (token::AS, std::move (l));
      }
#else
      static
      symbol_type
      make_AS (const location_type& l)
      {
        return symbol_type (token::AS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS (location_type l)
      {
        return symbol_type (token::PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS (const location_type& l)
      {
        return symbol_type (token::PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS (location_type l)
      {
        return symbol_type (token::MINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_MINUS (const location_type& l)
      {
        return symbol_type (token::MINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STAR (location_type l)
      {
        return symbol_type (token::STAR, std::move (l));
      }
#else
      static
      symbol_type
      make_STAR (const location_type& l)
      {
        return symbol_type (token::STAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SLASH (location_type l)
      {
        return symbol_type (token::SLASH, std::move (l));
      }
#else
      static
      symbol_type
      make_SLASH (const location_type& l)
      {
        return symbol_type (token::SLASH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PERCENT (location_type l)
      {
        return symbol_type (token::PERCENT, std::move (l));
      }
#else
      static
      symbol_type
      make_PERCENT (const location_type& l)
      {
        return symbol_type (token::PERCENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CARET (location_type l)
      {
        return symbol_type (token::CARET, std::move (l));
      }
#else
      static
      symbol_type
      make_CARET (const location_type& l)
      {
        return symbol_type (token::CARET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASSIGN (location_type l)
      {
        return symbol_type (token::ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_ASSIGN (const location_type& l)
      {
        return symbol_type (token::ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DESTRUCTIVE_ASSIGN (location_type l)
      {
        return symbol_type (token::DESTRUCTIVE_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_DESTRUCTIVE_ASSIGN (const location_type& l)
      {
        return symbol_type (token::DESTRUCTIVE_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQUAL_EQUAL (location_type l)
      {
        return symbol_type (token::EQUAL_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_EQUAL_EQUAL (const location_type& l)
      {
        return symbol_type (token::EQUAL_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NOT_EQUAL (location_type l)
      {
        return symbol_type (token::NOT_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_NOT_EQUAL (const location_type& l)
      {
        return symbol_type (token::NOT_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LESS (location_type l)
      {
        return symbol_type (token::LESS, std::move (l));
      }
#else
      static
      symbol_type
      make_LESS (const location_type& l)
      {
        return symbol_type (token::LESS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LESS_EQUAL (location_type l)
      {
        return symbol_type (token::LESS_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_LESS_EQUAL (const location_type& l)
      {
        return symbol_type (token::LESS_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GREATER (location_type l)
      {
        return symbol_type (token::GREATER, std::move (l));
      }
#else
      static
      symbol_type
      make_GREATER (const location_type& l)
      {
        return symbol_type (token::GREATER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GREATER_EQUAL (location_type l)
      {
        return symbol_type (token::GREATER_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_GREATER_EQUAL (const location_type& l)
      {
        return symbol_type (token::GREATER_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND (location_type l)
      {
        return symbol_type (token::AND, std::move (l));
      }
#else
      static
      symbol_type
      make_AND (const location_type& l)
      {
        return symbol_type (token::AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR (location_type l)
      {
        return symbol_type (token::OR, std::move (l));
      }
#else
      static
      symbol_type
      make_OR (const location_type& l)
      {
        return symbol_type (token::OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NOT (location_type l)
      {
        return symbol_type (token::NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_NOT (const location_type& l)
      {
        return symbol_type (token::NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONCAT (location_type l)
      {
        return symbol_type (token::CONCAT, std::move (l));
      }
#else
      static
      symbol_type
      make_CONCAT (const location_type& l)
      {
        return symbol_type (token::CONCAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOUBLECONCAT (location_type l)
      {
        return symbol_type (token::DOUBLECONCAT, std::move (l));
      }
#else
      static
      symbol_type
      make_DOUBLECONCAT (const location_type& l)
      {
        return symbol_type (token::DOUBLECONCAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FATARROW (location_type l)
      {
        return symbol_type (token::FATARROW, std::move (l));
      }
#else
      static
      symbol_type
      make_FATARROW (const location_type& l)
      {
        return symbol_type (token::FATARROW, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LPAREN (location_type l)
      {
        return symbol_type (token::LPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_LPAREN (const location_type& l)
      {
        return symbol_type (token::LPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RPAREN (location_type l)
      {
        return symbol_type (token::RPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_RPAREN (const location_type& l)
      {
        return symbol_type (token::RPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LBRACE (location_type l)
      {
        return symbol_type (token::LBRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_LBRACE (const location_type& l)
      {
        return symbol_type (token::LBRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RBRACE (location_type l)
      {
        return symbol_type (token::RBRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_RBRACE (const location_type& l)
      {
        return symbol_type (token::RBRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (location_type l)
      {
        return symbol_type (token::SEMICOLON, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const location_type& l)
      {
        return symbol_type (token::SEMICOLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLON (location_type l)
      {
        return symbol_type (token::COLON, std::move (l));
      }
#else
      static
      symbol_type
      make_COLON (const location_type& l)
      {
        return symbol_type (token::COLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOT (location_type l)
      {
        return symbol_type (token::DOT, std::move (l));
      }
#else
      static
      symbol_type
      make_DOT (const location_type& l)
      {
        return symbol_type (token::DOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UMINUS (location_type l)
      {
        return symbol_type (token::UMINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_UMINUS (const location_type& l)
      {
        return symbol_type (token::UMINUS, l);
      }
#endif


    class context
    {
    public:
      context (const Parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const Parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    Parser (const Parser&);
    /// Non copyable.
    Parser& operator= (const Parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const unsigned char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 297,     ///< Last index in yytable_.
      yynnts_ = 46,  ///< Number of nonterminal symbols.
      yyfinal_ = 76 ///< Termination state number.
    };


    // User arguments.
    hulk::parser::ParserDriver& driver;

  };

  inline
  Parser::symbol_kind_type
  Parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59
    };
    // Last valid token kind.
    const int code_max = 314;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.copy< BindingList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binding: // binding
        value.copy< BindingPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_decl: // decl
      case symbol_kind::S_function_decl: // function_decl
      case symbol_kind::S_type_decl: // type_decl
        value.copy< DeclPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_elif_clauses: // elif_clauses
        value.copy< ElifList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parent_args_opt: // parent_args_opt
      case symbol_kind::S_args_opt: // args_opt
      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_block_body_opt: // block_body_opt
      case symbol_kind::S_expr_list: // expr_list
        value.copy< ExprList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_if_expr: // if_expr
      case symbol_kind::S_while_expr: // while_expr
      case symbol_kind::S_for_expr: // for_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_relation: // relation
      case symbol_kind::S_type_test_expr: // type_test_expr
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.copy< ExprPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param: // param
        value.copy< Hulk::Param > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_member: // type_member
        value.copy< Hulk::TypeMember > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctor_params_opt: // ctor_params_opt
      case symbol_kind::S_params_opt: // params_opt
      case symbol_kind::S_param_list: // param_list
        value.copy< ParamList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.copy< ProgramPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_member_list: // type_member_list
        value.copy< TypeMemberList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.copy< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_inherits_opt: // inherits_opt
        value.copy< hulk::parser::InheritsInfo > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_lvalue: // lvalue
        value.copy< hulk::parser::LValueTarget > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_level_items: // top_level_items
      case symbol_kind::S_top_level_item: // top_level_item
        value.copy< hulk::parser::TopLevelItems > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
      case symbol_kind::S_return_ann_opt: // return_ann_opt
      case symbol_kind::S_type_ann_opt: // type_ann_opt
      case symbol_kind::S_type_expr: // type_expr
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  Parser::symbol_kind_type
  Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.move< BindingList > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_binding: // binding
        value.move< BindingPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_decl: // decl
      case symbol_kind::S_function_decl: // function_decl
      case symbol_kind::S_type_decl: // type_decl
        value.move< DeclPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_elif_clauses: // elif_clauses
        value.move< ElifList > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_parent_args_opt: // parent_args_opt
      case symbol_kind::S_args_opt: // args_opt
      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_block_body_opt: // block_body_opt
      case symbol_kind::S_expr_list: // expr_list
        value.move< ExprList > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_if_expr: // if_expr
      case symbol_kind::S_while_expr: // while_expr
      case symbol_kind::S_for_expr: // for_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_relation: // relation
      case symbol_kind::S_type_test_expr: // type_test_expr
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.move< ExprPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_param: // param
        value.move< Hulk::Param > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_member: // type_member
        value.move< Hulk::TypeMember > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ctor_params_opt: // ctor_params_opt
      case symbol_kind::S_params_opt: // params_opt
      case symbol_kind::S_param_list: // param_list
        value.move< ParamList > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_program: // program
        value.move< ProgramPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_member_list: // type_member_list
        value.move< TypeMemberList > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.move< double > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_inherits_opt: // inherits_opt
        value.move< hulk::parser::InheritsInfo > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_lvalue: // lvalue
        value.move< hulk::parser::LValueTarget > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_top_level_items: // top_level_items
      case symbol_kind::S_top_level_item: // top_level_item
        value.move< hulk::parser::TopLevelItems > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
      case symbol_kind::S_return_ann_opt: // return_ann_opt
      case symbol_kind::S_type_ann_opt: // type_ann_opt
      case symbol_kind::S_type_expr: // type_expr
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  Parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  Parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  Parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  Parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  Parser::symbol_kind_type
  Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  Parser::symbol_kind_type
  Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


#line 4 "src/parser/grammar.y"
} } // hulk::parser
#line 3010 "src/parser/parser.hpp"


// "%code provides" blocks.
#line 85 "src/parser/grammar.y"

    namespace hulk::parser {
        Parser::symbol_type yylex(ParserDriver& driver);
    }

#line 3020 "src/parser/parser.hpp"


#endif // !YY_YY_SRC_PARSER_PARSER_HPP_INCLUDED
