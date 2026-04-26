SHELL    := bash
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -Isrc
# Flags sin warnings para código generado por Bison (parser.cpp / parser.hpp)
CXXFLAGS_BISON := -std=c++20 -w -Isrc

OBJDIR := build

# ─────────────────────────────────────────────────────────────────────────────
# Fuentes compartidas: lexer + todos los nodos del AST + accept() centralizado
# ─────────────────────────────────────────────────────────────────────────────
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
	src/ast/others/baseCall.cpp \
	src/ast/others/exprBlock.cpp \
	src/ast/others/group.cpp \
	src/ast/others/program.cpp \
	src/ast/others/selfRef.cpp \
	src/ast/vectors/vectorLiteral.cpp \
	src/ast/vectors/vectorIndex.cpp \
	src/ast/vectors/vectorGenerator.cpp \
	src/ast/types/asExpr.cpp \
	src/ast/types/isExpr.cpp \
	src/ast/types/memberAccess.cpp \
	src/ast/types/methodCall.cpp \
	src/ast/types/newExpr.cpp \
	src/ast/types/typeDecl.cpp \
	src/ast/types/typeMemberAttribute.cpp \
	src/ast/types/typeMemberMethod.cpp \
	src/ast/accept_impl.cpp

EVAL_SRCS := \
	src/objects/hulk_value.cpp \
	src/eval/evaluator.cpp

# Objetos pre-compilados (se reusan entre targets para no recompilar Bison)
LEXER_AST_OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(LEXER_AST_SRCS))
PARSER_OBJS    := $(OBJDIR)/parser/parser.o \
                  $(OBJDIR)/parser/parser_driver.o \
                  $(OBJDIR)/parser/parser_lexer_adapter.o
EVAL_OBJS      := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(EVAL_SRCS))

.PHONY: all parser-gen lexer parser-demo parser-tests eval eval-tests err-tests clean

all: lexer parser-demo eval

# ─────────────────────────────────────────────────────────────────────────────
# Directorios de objetos
# ─────────────────────────────────────────────────────────────────────────────
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# parser.cpp es código Bison generado: compilar sin -Wall/-Wextra para no colgarse
$(OBJDIR)/parser/parser.o: src/parser/parser.cpp
	@mkdir -p $(OBJDIR)/parser
	$(CXX) $(CXXFLAGS_BISON) -c $< -o $@

$(OBJDIR)/parser/parser_driver.o: src/parser/parser_driver.cpp
	@mkdir -p $(OBJDIR)/parser
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/parser/parser_lexer_adapter.o: src/parser/parser_lexer_adapter.cpp
	@mkdir -p $(OBJDIR)/parser
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ─────────────────────────────────────────────────────────────────────────────
# Targets de binarios
# ─────────────────────────────────────────────────────────────────────────────
parser-gen:
	bison -d -o src/parser/parser.cpp src/parser/grammar.y

lexer:
	$(CXX) $(CXXFLAGS) \
		src/lexer/main.cpp \
		src/lexer/lexer.cpp \
		-o hulk_lexer

parser-demo: $(LEXER_AST_OBJS) $(PARSER_OBJS)
	@mkdir -p $(OBJDIR)/parser_main
	$(CXX) $(CXXFLAGS) -c src/parser/main.cpp -o $(OBJDIR)/parser_main/main.o
	$(CXX) $(CXXFLAGS) \
		$(LEXER_AST_OBJS) $(PARSER_OBJS) \
		$(OBJDIR)/parser_main/main.o \
		-o hulk_parser_demo

# ─────────────────────────────────────────────────────────────────────────────
# Evaluador (cortes 4, 5 y 6)
# ─────────────────────────────────────────────────────────────────────────────
eval: $(LEXER_AST_OBJS) $(PARSER_OBJS) $(EVAL_OBJS)
	@mkdir -p $(OBJDIR)/eval_main
	$(CXX) $(CXXFLAGS) -c src/eval/main.cpp -o $(OBJDIR)/eval_main/main.o
	$(CXX) $(CXXFLAGS) \
		$(LEXER_AST_OBJS) $(PARSER_OBJS) $(EVAL_OBJS) \
		$(OBJDIR)/eval_main/main.o \
		-o hulk_eval

# ─────────────────────────────────────────────────────────────────────────────
# Tests
# ─────────────────────────────────────────────────────────────────────────────
parser-tests: parser-demo
	@for f in tests/parser/*.hulk; do \
		echo "===== $$f ====="; \
		./hulk_parser_demo $$f || true; \
		echo; \
	done

eval-tests: eval
	@echo "=== Corte 4: expresiones básicas ==="; \
	for f in tests/eval/c4_*.hulk; do \
		echo "----- $$f -----"; \
		./hulk_eval $$f 2>/dev/null || true; \
		echo; \
	done; \
	echo "=== Corte 5: variables y funciones ==="; \
	for f in tests/eval/c5_*.hulk; do \
		echo "----- $$f -----"; \
		./hulk_eval $$f 2>/dev/null || true; \
		echo; \
	done; \
	echo "=== Corte 6: objetos y herencia ==="; \
	for f in tests/eval/c6_*.hulk; do \
		echo "----- $$f -----"; \
		./hulk_eval $$f 2>/dev/null || true; \
		echo; \
	done

err-tests: eval
	@echo "=== Tests de error (deben fallar con mensaje diagnóstico) ==="; \
	for f in tests/eval/err_*.hulk; do \
		echo "----- $$f -----"; \
		./hulk_eval $$f 1>/dev/null; \
		echo; \
	done

clean:
	rm -rf $(OBJDIR) hulk_lexer hulk_parser_demo hulk_eval
