#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "ast_node.hpp"
#include "nodes/nodes.hpp"

// forward declarations
namespace neon_compiler::ast::nodes
{
	struct Root;
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
	struct Assignment;
	struct StaticFunctionCall;
	struct MethodCall;
	struct StaticFieldCall;
	struct ReferenceCall;
	struct OptFunctionCall;
	struct OptEmpty;
	struct PureFunctionSet;
	struct PureFunction;
	struct GrammarSet;
	struct GrammarRule;
	struct TokenPattern;
	struct ParameterPattern;
	struct CompileFunction;
}

namespace neon_compiler::ast
{

class ASTVisitor
{
public:
	virtual ~ASTVisitor() = default;
	virtual void visit(const nodes::Root& node) = 0;
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
	virtual void visit(const nodes::Assignment& node) = 0;
	virtual void visit(const nodes::StaticFunctionCall& node) = 0;
	virtual void visit(const nodes::MethodCall& node) = 0;
	virtual void visit(const nodes::StaticFieldCall& node) = 0;
	virtual void visit(const nodes::ReferenceCall& node) = 0;
	virtual void visit(const nodes::OptFunctionCall& node) = 0;
	virtual void visit(const nodes::OptEmpty& node) = 0;
	virtual void visit(const nodes::PureFunctionSet& node) = 0;
	virtual void visit(const nodes::PureFunction& node) = 0;
	virtual void visit(const nodes::GrammarSet& node) = 0;
	virtual void visit(const nodes::GrammarRule& node) = 0;
	virtual void visit(const nodes::TokenPattern& node) = 0;
	virtual void visit(const nodes::ParameterPattern& node) = 0;
	virtual void visit(const nodes::CompileFunction& node) = 0;
};

}

#endif // AST_VISITOR_HPP