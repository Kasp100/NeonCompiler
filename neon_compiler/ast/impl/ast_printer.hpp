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
	void visit(const nodes::ObjectFunctionCall& node) override;
	void visit(const nodes::ObjectRead& node) override;
	void visit(const nodes::FunctionCall& node) override;
	void visit(const nodes::SimpleRead& node) override;
	void visit(const nodes::OptFunctionCall& node) override;
	void visit(const nodes::OptEmpty& node) override;
	void visit(const nodes::PureFunctionSet& node) override;
	void visit(const nodes::PureFunction& node) override;
	void visit(const nodes::OperatorDeclaration& node) override;
	void visit(const nodes::OperatorModule& node) override;
	void visit(const nodes::OperatorFunction& node) override;
	void visit(const nodes::CompileFunction& node) override;
	void visit(const nodes::LiteralNumberExpression& node) override;
	void visit(const nodes::LiteralStringExpression& node) override;
	void visit(const nodes::LiteralBooleanExpression& node) override;
	void visit(const nodes::OperatorCallExpression& node) override;
private:
	uint depth{0};
	void incr_depth();
	void decr_depth();
	void print(const std::string& str) const;
	void print_prefix() const;
	void print_line() const;
	void print_access(const nodes::Access& a) const;
	void print_package_member_pattern(const nodes::PackageMemberPattern& pmp) const;
};

}

#endif // AST_PRINTER_HPP