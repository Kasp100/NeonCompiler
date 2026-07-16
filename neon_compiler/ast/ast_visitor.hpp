#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "ast_node.hpp"
#include "nodes/nodes.hpp"

// forward declarations
namespace neon_compiler::ast::nodes
{
	struct Root;
	struct Entrypoint;
	struct Type;
	struct VariableDeclaration;
	struct Field;
	struct Method;
	struct Constant;
	struct ReferenceType;
	struct CodeBlock;
	struct DiscardExpression;
	struct LocalDeclaration;
	struct AutoCall;
	struct Return;
	struct Assignment;
	struct ObjectFunctionCall;
	struct ObjectRead;
	struct FunctionCall;
	struct SimpleRead;
	struct OptFunctionCall;
	struct OptEmpty;
	struct PureFunctionSet;
	struct PureFunction;
	struct OperatorDeclaration;
	struct OperatorModule;
	struct OperatorFunction;
	struct CompileFunction;
	struct LiteralNumberExpression;
	struct LiteralStringExpression;
	struct LiteralBooleanExpression;
	struct OperatorCallExpression;
}

namespace neon_compiler::ast
{

class ASTVisitor
{
public:
	virtual ~ASTVisitor() = default;
	virtual void visit(const nodes::Root& node) = 0;
	virtual void visit(const nodes::Entrypoint& node) = 0;
	virtual void visit(const nodes::Type& node) = 0;
	virtual void visit(const nodes::VariableDeclaration& node) = 0;
	virtual void visit(const nodes::Field& node) = 0;
	virtual void visit(const nodes::Method& node) = 0;
	virtual void visit(const nodes::Constant& node) = 0;
	virtual void visit(const nodes::ReferenceType& node) = 0;
	virtual void visit(const nodes::CodeBlock& node) = 0;
	virtual void visit(const nodes::DiscardExpression& node) = 0;
	virtual void visit(const nodes::LocalDeclaration& node) = 0;
	virtual void visit(const nodes::AutoCall& node) = 0;
	virtual void visit(const nodes::Return& node) = 0;
	virtual void visit(const nodes::Assignment& node) = 0;
	virtual void visit(const nodes::ObjectFunctionCall& node) = 0;
	virtual void visit(const nodes::ObjectRead& node) = 0;
	virtual void visit(const nodes::FunctionCall& node) = 0;
	virtual void visit(const nodes::SimpleRead& node) = 0;
	virtual void visit(const nodes::OptFunctionCall& node) = 0;
	virtual void visit(const nodes::OptEmpty& node) = 0;
	virtual void visit(const nodes::PureFunctionSet& node) = 0;
	virtual void visit(const nodes::PureFunction& node) = 0;
	virtual void visit(const nodes::OperatorDeclaration& node) = 0;
	virtual void visit(const nodes::OperatorModule& node) = 0;
	virtual void visit(const nodes::OperatorFunction& node) = 0;
	virtual void visit(const nodes::CompileFunction& node) = 0;
	virtual void visit(const nodes::LiteralNumberExpression& node) = 0;
	virtual void visit(const nodes::LiteralStringExpression& node) = 0;
	virtual void visit(const nodes::LiteralBooleanExpression& node) = 0;
	virtual void visit(const nodes::OperatorCallExpression& node) = 0;
};

}

#endif // AST_VISITOR_HPP