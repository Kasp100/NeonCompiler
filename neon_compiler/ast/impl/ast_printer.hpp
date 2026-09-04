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
	void visit(const nodes::FileNode& node) override;
	void visit(const nodes::TypeDeclaration& node) override;
	void visit(const nodes::VariableDeclaration& node) override;
    void visit(const nodes::ConstantDeclaration& node) override;
	void visit(const nodes::FieldDeclaration& node) override;
	void visit(const nodes::MethodDeclaration& node) override;
	void visit(const nodes::ReferenceType& node) override;
	void visit(const nodes::CodeBlock& node) override;
	void visit(const nodes::DiscardExpression& node) override;
	void visit(const nodes::LocalDeclarationOrAssignment& node) override;
	void visit(const nodes::UseStatement& node) override;
	void visit(const nodes::AutoCall& node) override;
	void visit(const nodes::Return& node) override;
	void visit(const nodes::Assignment& node) override;
	void visit(const nodes::Give& node) override;
	void visit(const nodes::GiveAndAssign& node) override;
	void visit(const nodes::ObjectFunctionCall& node) override;
	void visit(const nodes::ObjectRead& node) override;
	void visit(const nodes::FunctionCall& node) override;
	void visit(const nodes::SimpleRead& node) override;
	void visit(const nodes::OptionalEmpty& node) override;
	void visit(const nodes::PackageFunctionDeclaration& node) override;
	void visit(const nodes::OperatorModule& node) override;
	void visit(const nodes::OperatorDeclaration& node) override;
	void visit(const nodes::OperatorFunctionDeclaration& node) override;
	void visit(const nodes::LiteralNumberExpression& node) override;
	void visit(const nodes::LiteralStringExpression& node) override;
	void visit(const nodes::LiteralBooleanExpression& node) override;
	void visit(const nodes::OperatorCallExpression& node) override;
	void visit(const nodes::CheckPresence& node) override;
	void visit(const nodes::CheckAbsence& node) override;
	void visit(const nodes::Fallback& node) override;
private:
	uint depth{0};
	void incr_depth();
	void decr_depth();
	void print(const std::string& str) const;
	void print_prefix() const;
	void print_line() const;
	void print_access(const nodes::Access& a) const;
	void print_package_member_pattern(const nodes::PackageMemberPattern& pmp) const;
	void print_generic_arguments(const std::vector<nodes::GenericArgument>& generic_args) const;
	void print_generic_parameters(const std::vector<nodes::GenericParameter>& generic_params) const;
};

}

#endif // AST_PRINTER_HPP