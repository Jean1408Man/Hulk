CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -Isrc

LEXER_AST_SRCS := \
	src/lexer/lexer.cpp \
	src/ast/literales/number.cpp \
	src/ast/literales/string.cpp \
	src/ast/literales/boolean.cpp \
	src/ast/variables/variableReference.cpp \
	src/ast/variables/variableBinding.cpp \
	src/ast/variables/letIn.cpp \
	src/ast/assignments/desctructiveAssign.cpp \
	src/ast/assignments/destructiveAssignMember.cpp \
	src/ast/binOps/arithmeticBinOp.cpp \
	src/ast/binOps/logicBinOp.cpp \
	src/ast/binOps/stringBinOp.cpp \
	src/ast/conditionals/ifStmt.cpp \
	src/ast/unaryOps/arithmeticUnaryOp.cpp \
	src/ast/unaryOps/logicUnaryOp.cpp \
	src/ast/domainFunctions/print.cpp \
	src/ast/domainFunctions/builtinCall.cpp \
	src/ast/functions/functionCall.cpp \
	src/ast/functions/functionDecl.cpp \
	src/ast/loops/for.cpp \
	src/ast/loops/while.cpp \
	src/ast/others/exprBlock.cpp \
	src/ast/others/program.cpp \
	src/ast/types/asExpr.cpp \
	src/ast/types/isExpr.cpp \
	src/ast/types/memberAccess.cpp \
	src/ast/types/methodCall.cpp \
	src/ast/types/newExpr.cpp \
	src/ast/types/typeDecl.cpp \
	src/ast/types/typeMemberAttribute.cpp \
	src/ast/types/typeMemberMethod.cpp

PARSER_SRCS := \
	src/parser/parser.cpp \
	src/parser/parser_driver.cpp \
	src/parser/parser_lexer_adapter.cpp \
	src/parser/main.cpp

.PHONY: all parser-gen lexer parser-demo parser-tests clean

all: lexer parser-demo

parser-gen:
	bison -d -o src/parser/parser.cpp src/parser/grammar.y

lexer:
	$(CXX) $(CXXFLAGS) \
		src/lexer/main.cpp \
		src/lexer/lexer.cpp \
		-o hulk_lexer

parser-demo: parser-gen
	$(CXX) $(CXXFLAGS) \
		$(LEXER_AST_SRCS) \
		$(PARSER_SRCS) \
		-o hulk_parser_demo

parser-tests: parser-demo
	@for f in tests/parser/*.hulk; do \
		echo "===== $$f ====="; \
		./hulk_parser_demo $$f || true; \
		echo; \
	done

clean:
	rm -f hulk_lexer hulk_parser_demo
