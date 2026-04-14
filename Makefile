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
	src/ast/binOps/arithmeticBinOp.cpp \
	src/ast/binOps/logicBinOp.cpp \
	src/ast/binOps/stringBinOp.cpp \
	src/ast/unaryOps/arithmeticUnaryOp.cpp \
	src/ast/unaryOps/logicUnaryOp.cpp \
	src/ast/domainFunctions/print.cpp \
	src/ast/domainFunctions/builtinCall.cpp

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
