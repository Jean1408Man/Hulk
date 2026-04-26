#ifndef HULK_VISITOR_H
#define HULK_VISITOR_H

// Forward declarations — todos los nodos concretos del AST
namespace Hulk {

    // Literales
    class Number;
    class String;
    class Boolean;

    // Operaciones binarias
    class ArithmeticBinOp;
    class LogicBinOp;
    class StringBinOp;

    // Operaciones unarias
    class ArithmeticUnaryOp;
    class LogicUnaryOp;

    // Variables
    class VariableReference;
    class VariableBinding;
    class LetIn;

    // Asignaciones
    class DestructiveAssign;
    class DestructiveAssignMember;

    // Control de flujo
    class IfStmt;
    class WhileStmt;
    class For;

    // Funciones
    class FunctionCall;
    class Lambda;

    // Builtins
    class Print;
    class BuiltinCall;

    // Otros
    class ExprBlock;
    class Group;
    class SelfRef;
    class BaseCall;

    // OOP (corte 6)
    class NewExpr;
    class MemberAccess;
    class MethodCall;
    class IsExpr;
    class AsExpr;

    // Vectores
    class VectorLiteral;
    class VectorIndex;
    class VectorGenerator;

    // Declaraciones
    class FunctionDecl;
    class TypeDecl;
    class TypeMemberAttribute;
    class TypeMemberMethod;
    class ProtocolDecl;

    // Programa raíz
    class Program;

} // namespace Hulk

namespace Hulk {

    // -----------------------------------------------------------------------
    // ExprVisitor — visita todos los nodos que heredan de Expr
    // -----------------------------------------------------------------------
    class ExprVisitor {
    public:
        virtual ~ExprVisitor() = default;

        // Literales
        virtual void visit(Number&) = 0;
        virtual void visit(String&) = 0;
        virtual void visit(Boolean&) = 0;

        // Operaciones
        virtual void visit(ArithmeticBinOp&) = 0;
        virtual void visit(LogicBinOp&) = 0;
        virtual void visit(StringBinOp&) = 0;
        virtual void visit(ArithmeticUnaryOp&) = 0;
        virtual void visit(LogicUnaryOp&) = 0;

        // Variables y binding
        virtual void visit(VariableReference&) = 0;
        virtual void visit(VariableBinding&) = 0;
        virtual void visit(LetIn&) = 0;

        // Asignaciones
        virtual void visit(DestructiveAssign&) = 0;
        virtual void visit(DestructiveAssignMember&) = 0;

        // Control de flujo
        virtual void visit(IfStmt&) = 0;
        virtual void visit(WhileStmt&) = 0;
        virtual void visit(For&) = 0;

        // Funciones
        virtual void visit(FunctionCall&) = 0;
        virtual void visit(Lambda&) = 0;

        // Builtins
        virtual void visit(Print&) = 0;
        virtual void visit(BuiltinCall&) = 0;

        // Otros
        virtual void visit(ExprBlock&) = 0;
        virtual void visit(Group&) = 0;
        virtual void visit(SelfRef&) = 0;
        virtual void visit(BaseCall&) = 0;

        // OOP (corte 6)
        virtual void visit(NewExpr&) = 0;
        virtual void visit(MemberAccess&) = 0;
        virtual void visit(MethodCall&) = 0;
        virtual void visit(IsExpr&) = 0;
        virtual void visit(AsExpr&) = 0;

        // Vectores
        virtual void visit(VectorLiteral&) = 0;
        virtual void visit(VectorIndex&) = 0;
        virtual void visit(VectorGenerator&) = 0;
    };

    // -----------------------------------------------------------------------
    // DeclVisitor — visita todos los nodos que heredan de Decl
    // -----------------------------------------------------------------------
    class DeclVisitor {
    public:
        virtual ~DeclVisitor() = default;

        virtual void visit(FunctionDecl&) = 0;
        virtual void visit(TypeDecl&) = 0;
        virtual void visit(TypeMemberAttribute&) = 0;
        virtual void visit(TypeMemberMethod&) = 0;
        virtual void visit(ProtocolDecl&) = 0;
    };

    // -----------------------------------------------------------------------
    // ProgramVisitor — visita el nodo raíz Program
    // -----------------------------------------------------------------------
    class ProgramVisitor {
    public:
        virtual ~ProgramVisitor() = default;
        virtual void visit(Program&) = 0;
    };

} // namespace Hulk

#endif // HULK_VISITOR_H
