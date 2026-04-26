// Implementaciones de accept() para todos los nodos del AST.
// Centralizado aquí para no dispersar el boilerplate en cada .cpp de nodo.
#include "eval/visitor.h"

// Literales
#include "ast/literales/number.h"
#include "ast/literales/string.h"
#include "ast/literales/boolean.h"

// Operaciones binarias
#include "ast/binOps/arithmeticBinOp.h"
#include "ast/binOps/logicBinOp.h"
#include "ast/binOps/stringBinOp.h"

// Operaciones unarias
#include "ast/unaryOps/arithmeticUnaryOp.h"
#include "ast/unaryOps/logicUnaryOp.h"

// Variables
#include "ast/variables/variableReference.h"
#include "ast/variables/variableBinding.h"
#include "ast/variables/letIn.h"

// Asignaciones
#include "ast/assignments/desctructiveAssign.h"
#include "ast/assignments/destructiveAssignMember.h"

// Control de flujo
#include "ast/conditionals/ifStmt.h"
#include "ast/loops/while.h"
#include "ast/loops/for.h"

// Funciones
#include "ast/functions/functionCall.h"
#include "ast/functions/functionDecl.h"
#include "ast/functions/lambda.h"

// Builtins
#include "ast/domainFunctions/print.h"
#include "ast/domainFunctions/builtinCall.h"

// Otros
#include "ast/others/exprBlock.h"
#include "ast/others/group.h"
#include "ast/others/selfRef.h"
#include "ast/others/baseCall.h"

// OOP
#include "ast/types/newExpr.h"
#include "ast/types/memberAccess.h"
#include "ast/types/methodCall.h"
#include "ast/types/isExpr.h"
#include "ast/types/asExpr.h"
#include "ast/types/typeDecl.h"
#include "ast/types/typeMemberAttribute.h"
#include "ast/types/typeMemberMethod.h"

// Vectores
#include "ast/vectors/vectorLiteral.h"
#include "ast/vectors/vectorIndex.h"
#include "ast/vectors/vectorGenerator.h"

// Protocolos
#include "ast/protocols/protocolDecl.h"

namespace Hulk {

// ---------------------------------------------------------------------------
// Expr nodes
// ---------------------------------------------------------------------------
void Number::accept(ExprVisitor& v)               { v.visit(*this); }
void String::accept(ExprVisitor& v)               { v.visit(*this); }
void Boolean::accept(ExprVisitor& v)              { v.visit(*this); }

void ArithmeticBinOp::accept(ExprVisitor& v)      { v.visit(*this); }
void LogicBinOp::accept(ExprVisitor& v)           { v.visit(*this); }
void StringBinOp::accept(ExprVisitor& v)          { v.visit(*this); }

void ArithmeticUnaryOp::accept(ExprVisitor& v)    { v.visit(*this); }
void LogicUnaryOp::accept(ExprVisitor& v)         { v.visit(*this); }

void VariableReference::accept(ExprVisitor& v)    { v.visit(*this); }
void VariableBinding::accept(ExprVisitor& v)      { v.visit(*this); }
void LetIn::accept(ExprVisitor& v)                { v.visit(*this); }

void DestructiveAssign::accept(ExprVisitor& v)    { v.visit(*this); }
void DestructiveAssignMember::accept(ExprVisitor& v) { v.visit(*this); }

void IfStmt::accept(ExprVisitor& v)               { v.visit(*this); }
void WhileStmt::accept(ExprVisitor& v)            { v.visit(*this); }
void For::accept(ExprVisitor& v)                  { v.visit(*this); }

void FunctionCall::accept(ExprVisitor& v)         { v.visit(*this); }
void Lambda::accept(ExprVisitor& v)               { v.visit(*this); }

void Print::accept(ExprVisitor& v)                { v.visit(*this); }
void BuiltinCall::accept(ExprVisitor& v)          { v.visit(*this); }

void ExprBlock::accept(ExprVisitor& v)            { v.visit(*this); }
void Group::accept(ExprVisitor& v)                { v.visit(*this); }
void SelfRef::accept(ExprVisitor& v)              { v.visit(*this); }
void BaseCall::accept(ExprVisitor& v)             { v.visit(*this); }

void NewExpr::accept(ExprVisitor& v)              { v.visit(*this); }
void MemberAccess::accept(ExprVisitor& v)         { v.visit(*this); }
void MethodCall::accept(ExprVisitor& v)           { v.visit(*this); }
void IsExpr::accept(ExprVisitor& v)               { v.visit(*this); }
void AsExpr::accept(ExprVisitor& v)               { v.visit(*this); }

void VectorLiteral::accept(ExprVisitor& v)        { v.visit(*this); }
void VectorIndex::accept(ExprVisitor& v)          { v.visit(*this); }
void VectorGenerator::accept(ExprVisitor& v)      { v.visit(*this); }

// ---------------------------------------------------------------------------
// Decl nodes
// ---------------------------------------------------------------------------
void FunctionDecl::accept(DeclVisitor& v)         { v.visit(*this); }
void TypeDecl::accept(DeclVisitor& v)             { v.visit(*this); }
void TypeMemberAttribute::accept(DeclVisitor& v)  { v.visit(*this); }
void TypeMemberMethod::accept(DeclVisitor& v)     { v.visit(*this); }
void ProtocolDecl::accept(DeclVisitor& v)         { v.visit(*this); }

} // namespace Hulk
