#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include "../nodes/nodes.hpp"

namespace neon_compiler::ast::impl
{

class ASTPrinter : ASTVisitor
{
public:
	ASTPrinter();
	void visit(const nodes::Root& node) override;
	void visit(const nodes::Entrypoint& node) override;
	void visit(const nodes::Type& node) override;
	void visit(const nodes::VariableDeclaration& node) override;
	void visit(const nodes::Field& node) override;
	void visit(const nodes::Method& node) override;
	void visit(const nodes::Constant& node) override;
	void visit(const nodes::ReferenceType& node) override;
	void visit(const nodes::CodeBlock& node) override;
	void visit(const nodes::DiscardExpression& node) override;
	void visit(const nodes::LocalDeclaration& node) override;
	void visit(const nodes::AutoCall& node) override;
	void visit(const nodes::Return& node) override;
	void visit(const nodes::Assignment& node) override;
	void visit(const nodes::StaticFunctionCall& node) override;
	void visit(const nodes::MethodCall& node) override;
	void visit(const nodes::StaticFieldCall& node) override;
	void visit(const nodes::ReferenceCall& node) override;
	void visit(const nodes::OptFunctionCall& node) override;
	void visit(const nodes::OptEmpty& node) override;
	void visit(const nodes::PureFunctionSet& node) override;
	void visit(const nodes::PureFunction& node) override;
	void visit(const nodes::GrammarSet& node) override;
	void visit(const nodes::GrammarRule& node) override;
	void visit(const nodes::TokenPattern& node) override;
	void visit(const nodes::ParameterPattern& node) override;
	void visit(const nodes::CompileFunction& node) override;
private:
	static void print_access(const nodes::Access& access);
};

}

#endif // AST_PRINTER_HPP