#ifndef DECLARATION_ANALYSER_HPP
#define DECLARATION_ANALYSER_HPP

#include "../ast/ast_visitor.hpp"

namespace neon_compiler::semantic_analyser
{

class DeclarationAnalyser : neon_compiler::ast::ASTVisitor
{
public:
    explicit DeclarationAnalyser();
	void visit(const neon_compiler::ast::nodes::Root& node) override;
	void visit(const neon_compiler::ast::nodes::FileNode& node) override;
	void visit(const neon_compiler::ast::nodes::TypeDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::VariableDeclaration& node) override;
    void visit(const neon_compiler::ast::nodes::ConstantDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::FieldDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::MethodDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::ReferenceType& node) override;
	void visit(const neon_compiler::ast::nodes::CodeBlock& node) override;
	void visit(const neon_compiler::ast::nodes::DiscardExpression& node) override;
	void visit(const neon_compiler::ast::nodes::LocalDeclarationOrAssignment& node) override;
	void visit(const neon_compiler::ast::nodes::UseStatement& node) override;
	void visit(const neon_compiler::ast::nodes::AutoCall& node) override;
	void visit(const neon_compiler::ast::nodes::Return& node) override;
	void visit(const neon_compiler::ast::nodes::Assignment& node) override;
	void visit(const neon_compiler::ast::nodes::Give& node) override;
	void visit(const neon_compiler::ast::nodes::GiveAndAssign& node) override;
	void visit(const neon_compiler::ast::nodes::ObjectFunctionCall& node) override;
	void visit(const neon_compiler::ast::nodes::ObjectRead& node) override;
	void visit(const neon_compiler::ast::nodes::FunctionCall& node) override;
	void visit(const neon_compiler::ast::nodes::SimpleRead& node) override;
	void visit(const neon_compiler::ast::nodes::OptionalEmpty& node) override;
	void visit(const neon_compiler::ast::nodes::PackageFunctionDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::OperatorModule& node) override;
	void visit(const neon_compiler::ast::nodes::OperatorDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::OperatorFunctionDeclaration& node) override;
	void visit(const neon_compiler::ast::nodes::LiteralNumberExpression& node) override;
	void visit(const neon_compiler::ast::nodes::LiteralStringExpression& node) override;
	void visit(const neon_compiler::ast::nodes::LiteralBooleanExpression& node) override;
	void visit(const neon_compiler::ast::nodes::OperatorCallExpression& node) override;
	void visit(const neon_compiler::ast::nodes::CheckPresence& node) override;
	void visit(const neon_compiler::ast::nodes::CheckAbsence& node) override;
	void visit(const neon_compiler::ast::nodes::Fallback& node) override;

};

}

#endif // DECLARATION_ANALYSER_HPP