#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "ast_node.hpp"
#include "nodes/nodes.hpp"

namespace neon_compiler::ast
{

class ASTVisitor
{
public:
	virtual ~ASTVisitor() = default;
	virtual void visit(const nodes::Root& node) = 0;
	virtual void visit(const nodes::Type& node) = 0;
	virtual void visit(const nodes::Field& node) = 0;
	virtual void visit(const nodes::Method& node) = 0;
	virtual void visit(const nodes::ReferenceType& node) = 0;
	virtual void visit(const nodes::CodeBlock& node) = 0;
	virtual void visit(const nodes::DiscardExpression& node) = 0;
	virtual void visit(const nodes::VariableDeclaration& node) = 0;
	virtual void visit(const nodes::AutoCall& node) = 0;
	virtual void visit(const nodes::Assignment& node) = 0;
	virtual void visit(const nodes::StaticFunctionCall& node) = 0;
	virtual void visit(const nodes::MethodCall& node) = 0;
	virtual void visit(const nodes::StaticFieldCall& node) = 0;
	virtual void visit(const nodes::FieldCall& node) = 0;
	virtual void visit(const nodes::OptFunctionCall& node) = 0;
	virtual void visit(const nodes::OptFieldCall& node) = 0;
	virtual void visit(const nodes::PureFunctionSet& node) = 0;
	virtual void visit(const nodes::GrammarSet& node) = 0;
	virtual void visit(const nodes::CompileFunction& node) = 0;
};

}

#endif // AST_VISITOR_HPP