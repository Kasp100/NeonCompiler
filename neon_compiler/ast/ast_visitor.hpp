#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "ast_node.hpp"
#include "nodes/nodes.hpp"

// forward declarations
namespace neon_compiler::ast::nodes
{
	struct Root;
	struct TypeDeclaration;
	struct VariableDeclaration;
    struct ConstantDeclaration;
	struct FieldDeclaration;
	struct MethodDeclaration;
	struct ReferenceType;
	struct CodeBlock;
	struct DiscardExpression;
	struct LocalDeclarationOrAssignment;
	struct AutoCall;
	struct Return;
	struct Assignment;
	struct Give;
	struct GiveAndAssign;
	struct ObjectFunctionCall;
	struct ObjectRead;
	struct FunctionCall;
	struct SimpleRead;
	struct OptionalEmpty;
	struct PackageFunctionDeclaration;
	struct OperatorModule;
	struct OperatorDeclaration;
	struct OperatorFunctionDeclaration;
	struct LiteralNumberExpression;
	struct LiteralStringExpression;
	struct LiteralBooleanExpression;
	struct OperatorCallExpression;
	struct CheckPresence;
	struct CheckAbsence;
	struct Fallback;
}

namespace neon_compiler::ast
{

class ASTVisitor
{
public:
	virtual ~ASTVisitor() = default;
	virtual void visit(const nodes::Root& node) = 0;
	virtual void visit(const nodes::TypeDeclaration& node) = 0;
	virtual void visit(const nodes::VariableDeclaration& node) = 0;
	virtual void visit(const nodes::ConstantDeclaration& node) = 0;
	virtual void visit(const nodes::FieldDeclaration& node) = 0;
	virtual void visit(const nodes::MethodDeclaration& node) = 0;
	virtual void visit(const nodes::ReferenceType& node) = 0;
	virtual void visit(const nodes::CodeBlock& node) = 0;
	virtual void visit(const nodes::DiscardExpression& node) = 0;
	virtual void visit(const nodes::LocalDeclarationOrAssignment& node) = 0;
	virtual void visit(const nodes::AutoCall& node) = 0;
	virtual void visit(const nodes::Return& node) = 0;
	virtual void visit(const nodes::Assignment& node) = 0;
	virtual void visit(const nodes::Give& node) = 0;
	virtual void visit(const nodes::GiveAndAssign& node) = 0;
	virtual void visit(const nodes::ObjectFunctionCall& node) = 0;
	virtual void visit(const nodes::ObjectRead& node) = 0;
	virtual void visit(const nodes::FunctionCall& node) = 0;
	virtual void visit(const nodes::SimpleRead& node) = 0;
	virtual void visit(const nodes::OptionalEmpty& node) = 0;
	virtual void visit(const nodes::PackageFunctionDeclaration& node) = 0;
	virtual void visit(const nodes::OperatorModule& node) = 0;
	virtual void visit(const nodes::OperatorDeclaration& node) = 0;
	virtual void visit(const nodes::OperatorFunctionDeclaration& node) = 0;
	virtual void visit(const nodes::LiteralNumberExpression& node) = 0;
	virtual void visit(const nodes::LiteralStringExpression& node) = 0;
	virtual void visit(const nodes::LiteralBooleanExpression& node) = 0;
	virtual void visit(const nodes::OperatorCallExpression& node) = 0;
	virtual void visit(const nodes::CheckPresence& node) = 0;
	virtual void visit(const nodes::CheckAbsence& node) = 0;
	virtual void visit(const nodes::Fallback& node) = 0;
};

}

#endif // AST_VISITOR_HPP