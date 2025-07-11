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
	virtual void visit(const nodes::PureFunctionSet& node) = 0;
	virtual void visit(const nodes::GrammarSet& node) = 0;
	virtual void visit(const nodes::CompileFunction& node) = 0;
};

}

#endif // AST_VISITOR_HPP