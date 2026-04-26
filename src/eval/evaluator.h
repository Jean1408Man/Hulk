#ifndef HULK_EVALUATOR_H
#define HULK_EVALUATOR_H

#include "visitor.h"
#include "environment.h"
#include "../objects/hulk_value.h"
#include "../objects/hulk_object.h"
#include "../ast/others/program.h"
#include "../common/diagnosticEngine.hpp"
#include "../common/diagnosticRepository.hpp"
#include "../common/span.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <vector>

namespace Hulk {

    // -----------------------------------------------------------------------
    // EvalError — excepción interna que corta la evaluación.
    // El mensaje ya fue registrado en el DiagnosticEngine antes de lanzar.
    // -----------------------------------------------------------------------
    struct EvalError : std::runtime_error {
        explicit EvalError() : std::runtime_error("eval error") {}
    };

    // -----------------------------------------------------------------------
    // Evaluator — implementa ExprVisitor y DeclVisitor.
    //
    // Uso:
    //   Evaluator ev;
    //   ev.run(*program);          // evalúa el programa completo
    //
    // El resultado de cada visit() se deposita en result_.
    // Para evaluar una subexpresión se llama eval(expr*) que retorna HulkValue.
    // -----------------------------------------------------------------------
    class Evaluator : public ExprVisitor, public DeclVisitor {
    public:
        explicit Evaluator(hulk::common::DiagnosticEngine& engine);

        // Punto de entrada: evalúa un Program completo
        void run(Program& program);

    private:
        // ------------------------------------------------------------------
        // Estado del evaluador
        // ------------------------------------------------------------------
        hulk::common::DiagnosticEngine& engine_;                // diagnósticos compartidos
        HulkValue result_;                                      // registro de retorno
        std::shared_ptr<Environment> env_;                      // scope actual
        std::unordered_map<std::string, FunctionDecl*> funcs_; // tabla global de funciones
        std::unordered_map<std::string, TypeDef> types_;        // tabla global de tipos
        HulkValue self_;                                        // instancia actual en métodos

        // ------------------------------------------------------------------
        // Helper para reportar un error y lanzar EvalError
        // ------------------------------------------------------------------
        template<typename... Args>
        [[noreturn]] void report_error(const hulk::common::Span& span,
                                       const std::string& error_id,
                                       Args&&... args) {
            engine_.report(error_id,
                           hulk::common::DiagnosticLevel::Semantic,
                           hulk::common::Severity::Error,
                           span,
                           std::forward<Args>(args)...);
            throw EvalError{};
        }

        [[noreturn]] void report_error_raw(const hulk::common::Span& span,
                                           const std::string& msg) {
            engine_.report_raw(hulk::common::DiagnosticLevel::Semantic,
                               hulk::common::Severity::Error,
                               span,
                               msg);
            throw EvalError{};
        }

        // ------------------------------------------------------------------
        // Helpers internos
        // ------------------------------------------------------------------
        HulkValue eval(Expr* node);
        void push_scope();
        void pop_scope();
        std::shared_ptr<Environment> make_child_env();

        // Busca un método en el tipo y sus ancestros (para herencia)
        const MethodDef* find_method(const std::string& type_name,
                                     const std::string& method_name) const;
        // Inicializa los atributos de un objeto según su TypeDef (y la del padre)
        void init_object(HulkObject& obj, const TypeDef& def,
                         const std::vector<HulkValue>& ctor_args);

        // ------------------------------------------------------------------
        // CORTE 4 — Expresiones puras, builtins, bloques, let, if, while
        // ------------------------------------------------------------------
        void visit(Number& n)           override;
        void visit(String& n)           override;
        void visit(Boolean& n)          override;

        void visit(ArithmeticBinOp& n)  override;
        void visit(LogicBinOp& n)       override;
        void visit(StringBinOp& n)      override;
        void visit(ArithmeticUnaryOp& n) override;
        void visit(LogicUnaryOp& n)     override;

        void visit(Print& n)            override;
        void visit(BuiltinCall& n)      override;

        void visit(ExprBlock& n)        override;
        void visit(Group& n)            override;

        void visit(LetIn& n)            override;
        void visit(VariableBinding& n)  override;

        void visit(IfStmt& n)           override;
        void visit(WhileStmt& n)        override;
        void visit(For& n)              override;

        // ------------------------------------------------------------------
        // CORTE 5 — Variables, scopes, funciones, recursión, :=
        // ------------------------------------------------------------------
        void visit(VariableReference& n)   override;
        void visit(DestructiveAssign& n)   override;
        void visit(DestructiveAssignMember& n) override;
        void visit(FunctionCall& n)        override;
        void visit(Lambda& n)              override;

        // DeclVisitor — registra funciones y tipos en las tablas globales
        void visit(FunctionDecl& n)        override;
        void visit(TypeDecl& n)            override;
        void visit(TypeMemberAttribute& n) override;
        void visit(TypeMemberMethod& n)    override;
        void visit(ProtocolDecl& n)        override;

        // ------------------------------------------------------------------
        // CORTE 6 — OOP: instancias, miembros, herencia, self, base
        // ------------------------------------------------------------------
        void visit(NewExpr& n)          override;
        void visit(MemberAccess& n)     override;
        void visit(MethodCall& n)       override;
        void visit(SelfRef& n)          override;
        void visit(BaseCall& n)         override;
        void visit(IsExpr& n)           override;
        void visit(AsExpr& n)           override;

        // Vectores
        void visit(VectorLiteral& n)    override;
        void visit(VectorIndex& n)      override;
        void visit(VectorGenerator& n)  override;
    };

} // namespace Hulk

#endif // HULK_EVALUATOR_H
