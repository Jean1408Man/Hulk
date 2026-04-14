// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

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

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.hpp"


// Unqualified %code blocks.
#line 50 "src/parser/grammar.y"

    #define yylex() yylex(driver)

#line 50 "src/parser/parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 4 "src/parser/grammar.y"
namespace hulk { namespace parser {
#line 143 "src/parser/parser.cpp"

  /// Build a parser object.
  Parser::Parser (hulk::parser::ParserDriver& driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.YY_MOVE_OR_COPY< BindingList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.YY_MOVE_OR_COPY< NodeList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.YY_MOVE_OR_COPY< NodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.move< BindingList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< NodeList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.move< NodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.copy< BindingList > (that.value);
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.copy< NodeList > (that.value);
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.copy< NodePtr > (that.value);
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_binding_list: // binding_list
        value.move< BindingList > (that.value);
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< NodeList > (that.value);
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        value.move< NodePtr > (that.value);
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        value.move< double > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_binding_list: // binding_list
        yylhs.value.emplace< BindingList > ();
        break;

      case symbol_kind::S_expr_list: // expr_list
        yylhs.value.emplace< NodeList > ();
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_let_expr: // let_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_logic_or: // logic_or
      case symbol_kind::S_logic_and: // logic_and
      case symbol_kind::S_equality: // equality
      case symbol_kind::S_concat: // concat
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_power: // power
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_postfix: // postfix
      case symbol_kind::S_primary: // primary
      case symbol_kind::S_block: // block
        yylhs.value.emplace< NodePtr > ();
        break;

      case symbol_kind::S_NUMBER_LITERAL: // NUMBER_LITERAL
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_ERROR_TOKEN: // ERROR_TOKEN
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: expr
#line 92 "src/parser/grammar.y"
      {
          driver.set_result(std::move(yystack_[0].value.as < NodePtr > ()));
      }
#line 719 "src/parser/parser.cpp"
    break;

  case 3: // expr: let_expr
#line 99 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 727 "src/parser/parser.cpp"
    break;

  case 4: // expr: assign_expr
#line 103 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 735 "src/parser/parser.cpp"
    break;

  case 5: // let_expr: LET binding_list IN expr
#line 110 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LetIn>(std::move(yystack_[2].value.as < BindingList > ()), std::move(yystack_[0].value.as < NodePtr > ()));
      }
#line 743 "src/parser/parser.cpp"
    break;

  case 6: // binding_list: IDENTIFIER ASSIGN expr
#line 117 "src/parser/grammar.y"
      {
          BindingList bindings;
          bindings.push_back(
              std::make_unique<Hulk::VariableBinding>(yystack_[2].value.as < std::string > (), std::move(yystack_[0].value.as < NodePtr > ()))
          );
          yylhs.value.as < BindingList > () = std::move(bindings);
      }
#line 755 "src/parser/parser.cpp"
    break;

  case 7: // binding_list: binding_list COMMA IDENTIFIER ASSIGN expr
#line 125 "src/parser/grammar.y"
      {
          yystack_[4].value.as < BindingList > ().push_back(
              std::make_unique<Hulk::VariableBinding>(yystack_[2].value.as < std::string > (), std::move(yystack_[0].value.as < NodePtr > ()))
          );
          yylhs.value.as < BindingList > () = std::move(yystack_[4].value.as < BindingList > ());
      }
#line 766 "src/parser/parser.cpp"
    break;

  case 8: // assign_expr: IDENTIFIER DESTRUCTIVE_ASSIGN expr
#line 135 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::DestructiveAssign>(yystack_[2].value.as < std::string > (), std::move(yystack_[0].value.as < NodePtr > ()));
      }
#line 774 "src/parser/parser.cpp"
    break;

  case 9: // assign_expr: logic_or
#line 139 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 782 "src/parser/parser.cpp"
    break;

  case 10: // logic_or: logic_or OR logic_and
#line 146 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::Or, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 792 "src/parser/parser.cpp"
    break;

  case 11: // logic_or: logic_and
#line 152 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 800 "src/parser/parser.cpp"
    break;

  case 12: // logic_and: logic_and AND equality
#line 159 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::And, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 810 "src/parser/parser.cpp"
    break;

  case 13: // logic_and: equality
#line 165 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 818 "src/parser/parser.cpp"
    break;

  case 14: // equality: equality EQUAL_EQUAL concat
#line 172 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::Equal, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 828 "src/parser/parser.cpp"
    break;

  case 15: // equality: equality NOT_EQUAL concat
#line 178 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::NotEqual, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 838 "src/parser/parser.cpp"
    break;

  case 16: // equality: equality LESS concat
#line 184 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::Less, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 848 "src/parser/parser.cpp"
    break;

  case 17: // equality: equality LESS_EQUAL concat
#line 190 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::LessEqual, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 858 "src/parser/parser.cpp"
    break;

  case 18: // equality: equality GREATER concat
#line 196 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::Greater, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 868 "src/parser/parser.cpp"
    break;

  case 19: // equality: equality GREATER_EQUAL concat
#line 202 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::LogicOp::GreaterEqual, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 878 "src/parser/parser.cpp"
    break;

  case 20: // equality: concat
#line 208 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 886 "src/parser/parser.cpp"
    break;

  case 21: // concat: concat CONCAT additive
#line 215 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::StringBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::StringOp::Concat, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 896 "src/parser/parser.cpp"
    break;

  case 22: // concat: concat DOUBLECONCAT additive
#line 221 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::StringBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::StringOp::SpaceConcat, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 906 "src/parser/parser.cpp"
    break;

  case 23: // concat: additive
#line 227 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 914 "src/parser/parser.cpp"
    break;

  case 24: // additive: additive PLUS multiplicative
#line 234 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::ArithmeticOp::Plus, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 924 "src/parser/parser.cpp"
    break;

  case 25: // additive: additive MINUS multiplicative
#line 240 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::ArithmeticOp::Minus, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 934 "src/parser/parser.cpp"
    break;

  case 26: // additive: multiplicative
#line 246 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 942 "src/parser/parser.cpp"
    break;

  case 27: // multiplicative: multiplicative STAR power
#line 253 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::ArithmeticOp::Mult, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 952 "src/parser/parser.cpp"
    break;

  case 28: // multiplicative: multiplicative SLASH power
#line 259 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::ArithmeticOp::Div, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 962 "src/parser/parser.cpp"
    break;

  case 29: // multiplicative: power
#line 265 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 970 "src/parser/parser.cpp"
    break;

  case 30: // power: unary CARET power
#line 272 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::ArithmeticBinOp>(
              std::move(yystack_[2].value.as < NodePtr > ()), Hulk::ArithmeticOp::Pow, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 980 "src/parser/parser.cpp"
    break;

  case 31: // power: unary
#line 278 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 988 "src/parser/parser.cpp"
    break;

  case 32: // unary: MINUS unary
#line 285 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::ArithmeticUnaryOp>(
              Hulk::ArithUnaryType::Minus, std::move(yystack_[0].value.as < NodePtr > ())
          );
      }
#line 998 "src/parser/parser.cpp"
    break;

  case 33: // unary: NOT unary
#line 291 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::LogicUnaryOp>(std::move(yystack_[0].value.as < NodePtr > ()));
      }
#line 1006 "src/parser/parser.cpp"
    break;

  case 34: // unary: postfix
#line 295 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 1014 "src/parser/parser.cpp"
    break;

  case 35: // postfix: primary
#line 302 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 1022 "src/parser/parser.cpp"
    break;

  case 36: // primary: NUMBER_LITERAL
#line 309 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::Number>(yystack_[0].value.as < double > ());
      }
#line 1030 "src/parser/parser.cpp"
    break;

  case 37: // primary: STRING_LITERAL
#line 313 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::String>(yystack_[0].value.as < std::string > ());
      }
#line 1038 "src/parser/parser.cpp"
    break;

  case 38: // primary: TRUE
#line 317 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::Boolean>(true);
      }
#line 1046 "src/parser/parser.cpp"
    break;

  case 39: // primary: FALSE
#line 321 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::Boolean>(false);
      }
#line 1054 "src/parser/parser.cpp"
    break;

  case 40: // primary: IDENTIFIER
#line 325 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::VariableReference>(yystack_[0].value.as < std::string > ());
      }
#line 1062 "src/parser/parser.cpp"
    break;

  case 41: // primary: LPAREN expr RPAREN
#line 329 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[1].value.as < NodePtr > ());
      }
#line 1070 "src/parser/parser.cpp"
    break;

  case 42: // primary: block
#line 333 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::move(yystack_[0].value.as < NodePtr > ());
      }
#line 1078 "src/parser/parser.cpp"
    break;

  case 43: // primary: PRINT LPAREN expr RPAREN
#line 337 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::Print>(std::move(yystack_[1].value.as < NodePtr > ()));
      }
#line 1086 "src/parser/parser.cpp"
    break;

  case 44: // primary: RAND LPAREN RPAREN
#line 341 "src/parser/grammar.y"
      {
          NodeList args;
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Rand, std::move(args));
      }
#line 1095 "src/parser/parser.cpp"
    break;

  case 45: // primary: EXP LPAREN expr RPAREN
#line 346 "src/parser/grammar.y"
      {
          NodeList args;
          args.push_back(std::move(yystack_[1].value.as < NodePtr > ()));
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Exp, std::move(args));
      }
#line 1105 "src/parser/parser.cpp"
    break;

  case 46: // primary: LOG LPAREN expr COMMA expr RPAREN
#line 352 "src/parser/grammar.y"
      {
          NodeList args;
          args.push_back(std::move(yystack_[3].value.as < NodePtr > ()));
          args.push_back(std::move(yystack_[1].value.as < NodePtr > ()));
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::BuiltinCall>(Hulk::BuiltinFunc::Log, std::move(args));
      }
#line 1116 "src/parser/parser.cpp"
    break;

  case 47: // primary: PI_CONST
#line 359 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::Number>(3.14159265358979323846);
      }
#line 1124 "src/parser/parser.cpp"
    break;

  case 48: // primary: E_CONST
#line 363 "src/parser/grammar.y"
      {
          yylhs.value.as < NodePtr > () = std::make_unique<Hulk::Number>(2.71828182845904523536);
      }
#line 1132 "src/parser/parser.cpp"
    break;

  case 49: // block: LBRACE expr_list RBRACE
#line 370 "src/parser/grammar.y"
      {
          // AST note: ExprBlock currently expects Expr-derived nodes,
          // but many existing literal/expression nodes derive from ASTnode directly.
          // Until that hierarchy is reconciled, the parser preserves block value
          // semantics by returning the last expression of the block.
          yylhs.value.as < NodePtr > () = std::move(yystack_[1].value.as < NodeList > ().back());
      }
#line 1144 "src/parser/parser.cpp"
    break;

  case 50: // expr_list: expr
#line 381 "src/parser/grammar.y"
      {
          NodeList nodes;
          nodes.push_back(std::move(yystack_[0].value.as < NodePtr > ()));
          yylhs.value.as < NodeList > () = std::move(nodes);
      }
#line 1154 "src/parser/parser.cpp"
    break;

  case 51: // expr_list: expr_list SEMICOLON expr
#line 387 "src/parser/grammar.y"
      {
          yystack_[2].value.as < NodeList > ().push_back(std::move(yystack_[0].value.as < NodePtr > ()));
          yylhs.value.as < NodeList > () = std::move(yystack_[2].value.as < NodeList > ());
      }
#line 1163 "src/parser/parser.cpp"
    break;


#line 1167 "src/parser/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "END", "error", "invalid token", "IDENTIFIER", "STRING_LITERAL",
  "ERROR_TOKEN", "NUMBER_LITERAL", "TRUE", "FALSE", "PRINT", "SQRT", "SIN",
  "COS", "EXP", "LOG", "RAND", "PI_CONST", "E_CONST", "LET", "IN", "IF",
  "ELIF", "ELSE", "WHILE", "FOR", "FUNCTION", "TYPE", "INHERITS", "NEW",
  "IS", "AS", "PLUS", "MINUS", "STAR", "SLASH", "PERCENT", "CARET",
  "ASSIGN", "DESTRUCTIVE_ASSIGN", "EQUAL_EQUAL", "NOT_EQUAL", "LESS",
  "LESS_EQUAL", "GREATER", "GREATER_EQUAL", "AND", "OR", "NOT", "CONCAT",
  "DOUBLECONCAT", "FATARROW", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
  "COMMA", "SEMICOLON", "COLON", "DOT", "UMINUS", "$accept", "program",
  "expr", "let_expr", "binding_list", "assign_expr", "logic_or",
  "logic_and", "equality", "concat", "additive", "multiplicative", "power",
  "unary", "postfix", "primary", "block", "expr_list", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // Parser::context.
  Parser::context::context (const Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -40;

  const signed char Parser::yytable_ninf_ = -1;

  const signed char
  Parser::yypact_[] =
  {
      18,    -1,   -40,   -40,   -40,   -40,   -28,   -12,   -10,     4,
     -40,   -40,    42,    69,    69,    18,    18,    59,   -40,   -40,
     -40,    14,    16,   -36,   -33,    12,    13,   -40,    26,   -40,
     -40,   -40,    18,    18,    18,    18,    11,    27,   -17,   -40,
     -40,   -40,    15,   -40,   -14,   -40,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,   -40,    29,    35,    24,   -40,    18,    18,    63,   -40,
     -40,    18,    16,   -36,   -33,   -33,   -33,   -33,   -33,   -33,
      12,    12,    13,    13,   -40,   -40,   -40,   -40,   -40,    18,
     -40,   -40,    31,   -40,    36,    18,   -40,   -40
  };

  const signed char
  Parser::yydefact_[] =
  {
       0,    40,    37,    36,    38,    39,     0,     0,     0,     0,
      47,    48,     0,     0,     0,     0,     0,     0,     2,     3,
       4,     9,    11,    13,    20,    23,    26,    29,    31,    34,
      35,    42,     0,     0,     0,     0,     0,     0,     0,    40,
      32,    33,     0,    50,     0,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     8,     0,     0,     0,    44,     0,     0,     0,    41,
      49,     0,    10,    12,    14,    15,    16,    17,    18,    19,
      21,    22,    24,    25,    27,    28,    30,    43,    45,     0,
       6,     5,     0,    51,     0,     0,    46,     7
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -40,   -40,   -15,   -40,   -40,   -40,   -40,    43,    23,   -39,
      -6,    -3,   -30,    44,   -40,   -40,   -40,   -40
  };

  const signed char
  Parser::yydefgoto_[] =
  {
       0,    17,    18,    19,    38,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    44
  };

  const signed char
  Parser::yytable_[] =
  {
      42,    43,    67,    48,    49,    50,    51,    52,    53,    74,
      75,    76,    77,    78,    79,    54,    55,    61,    62,    63,
      64,     1,     2,    33,     3,     4,     5,     6,    84,    85,
      86,     7,     8,     9,    10,    11,    12,    32,    68,    34,
      70,    35,    71,    56,    57,    37,    58,    59,    80,    81,
      13,    90,    91,    82,    83,    36,    93,    40,    41,    45,
      46,    47,    60,    65,    66,    14,    92,    69,    95,    15,
      73,    16,    39,     2,    94,     3,     4,     5,     6,    89,
      97,    87,     7,     8,     9,    10,    11,    88,    96,    72,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
      15,     0,    16
  };

  const signed char
  Parser::yycheck_[] =
  {
      15,    16,    19,    39,    40,    41,    42,    43,    44,    48,
      49,    50,    51,    52,    53,    48,    49,    32,    33,    34,
      35,     3,     4,    51,     6,     7,     8,     9,    58,    59,
      60,    13,    14,    15,    16,    17,    18,    38,    55,    51,
      54,    51,    56,    31,    32,     3,    33,    34,    54,    55,
      32,    66,    67,    56,    57,    51,    71,    13,    14,     0,
      46,    45,    36,    52,    37,    47,     3,    52,    37,    51,
      47,    53,     3,     4,    89,     6,     7,     8,     9,    55,
      95,    52,    13,    14,    15,    16,    17,    52,    52,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      51,    -1,    53
  };

  const signed char
  Parser::yystos_[] =
  {
       0,     3,     4,     6,     7,     8,     9,    13,    14,    15,
      16,    17,    18,    32,    47,    51,    53,    61,    62,    63,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    38,    51,    51,    51,    51,     3,    64,     3,
      73,    73,    62,    62,    77,     0,    46,    45,    39,    40,
      41,    42,    43,    44,    48,    49,    31,    32,    33,    34,
      36,    62,    62,    62,    62,    52,    37,    19,    55,    52,
      54,    56,    67,    68,    69,    69,    69,    69,    69,    69,
      70,    70,    71,    71,    72,    72,    72,    52,    52,    55,
      62,    62,     3,    62,    62,    37,    52,    62
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    60,    61,    62,    62,    63,    64,    64,    65,    65,
      66,    66,    67,    67,    68,    68,    68,    68,    68,    68,
      68,    69,    69,    69,    70,    70,    70,    71,    71,    71,
      72,    72,    73,    73,    73,    74,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    76,
      77,    77
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     4,     3,     5,     3,     1,
       3,     1,     3,     1,     3,     3,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     3,     4,     6,     1,     1,     3,
       1,     3
  };




#if YYDEBUG
  const short
  Parser::yyrline_[] =
  {
       0,    91,    91,    98,   102,   109,   116,   124,   134,   138,
     145,   151,   158,   164,   171,   177,   183,   189,   195,   201,
     207,   214,   220,   226,   233,   239,   245,   252,   258,   264,
     271,   277,   284,   290,   294,   301,   308,   312,   316,   320,
     324,   328,   332,   336,   340,   345,   351,   358,   362,   369,
     380,   386
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 4 "src/parser/grammar.y"
} } // hulk::parser
#line 1660 "src/parser/parser.cpp"

#line 393 "src/parser/grammar.y"


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
