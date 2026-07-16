#include "ast_printer.hpp"

#include <iostream>
#include "../nodes/statement_nodes.hpp"

using namespace neon_compiler::ast::impl;
using namespace neon_compiler::ast::nodes;

ASTPrinter::ASTPrinter() {}

void ASTPrinter::visit(const nodes::Root& node)
{
	for(const std::pair<const std::string, std::unique_ptr<nodes::PackageMember>>& pm : node.package_members)
	{
		std::cout << "[AST] PM ";
		std::cout << pm.first;
		std::cout << "\n";
		pm.second->accept(*this);
		std::cout << "[--- end PM ---]\n";
	}
}

void ASTPrinter::visit(const nodes::Entrypoint& node)
{
	std::cout << "[AST] ";
	print_access(node.access);
	std::cout << " entrypoint";
	std::cout << "\n";
	node.body.accept(*this);
	std::cout << "[--- end entrypoint ---]\n";
}

void ASTPrinter::visit(const nodes::Type& node)
{

}

void ASTPrinter::visit(const nodes::VariableDeclaration& node)
{

}

void ASTPrinter::visit(const nodes::Field& node)
{

}

void ASTPrinter::visit(const nodes::Method& node)
{

}

void ASTPrinter::visit(const nodes::Constant& node)
{

}

void ASTPrinter::visit(const nodes::ReferenceType& node)
{

}

void ASTPrinter::visit(const nodes::CodeBlock& node)
{
	std::cout << "[AST] code block\n";
	for(const std::unique_ptr<Statement>& stmt : node.statements)
	{
		stmt->accept(*this);
	}
	std::cout << "[--- end code block ---]\n";
}

void ASTPrinter::visit(const nodes::DiscardExpression& node)
{

}

void ASTPrinter::visit(const nodes::LocalDeclaration& node)
{

}

void ASTPrinter::visit(const nodes::AutoCall& node)
{

}

void ASTPrinter::visit(const nodes::Return& node)
{
	std::cout << "[AST] return\n";
	node.value->accept(*this);
	std::cout << "[--- end return ---]\n";
}

void ASTPrinter::visit(const nodes::Assignment& node)
{

}

void ASTPrinter::visit(const nodes::ObjectFunctionCall& node)
{

}

void ASTPrinter::visit(const nodes::ObjectRead& node)
{

}

void ASTPrinter::visit(const nodes::FunctionCall& node)
{
	std::cout << "[AST] function call - function name: ";
	std::cout << node.function_name;
	std::cout << "\n";
	for(const std::unique_ptr<Expression>& arg : node.arguments)
	{
		arg->accept(*this);
	}
	std::cout << "[--- end function call expression ---]\n";
}

void ASTPrinter::visit(const nodes::SimpleRead& node)
{
	std::cout << "[AST] simple read - reference name: ";
	std::cout << node.reference_name;
	std::cout << " [end]\n";
}

void ASTPrinter::visit(const nodes::OptFunctionCall& node)
{

}

void ASTPrinter::visit(const nodes::OptEmpty& node)
{

}

void ASTPrinter::visit(const nodes::PureFunctionSet& node)
{

}

void ASTPrinter::visit(const nodes::PureFunction& node)
{

}

void ASTPrinter::visit(const nodes::OperatorDeclaration& node)
{

}

void ASTPrinter::visit(const nodes::OperatorModule& node)
{

}

void ASTPrinter::visit(const nodes::OperatorFunction& node)
{

}

void ASTPrinter::visit(const nodes::CompileFunction& node)
{

}

void ASTPrinter::visit(const nodes::LiteralNumberExpression& node)
{
	std::cout << "[AST] literal number expression - amount of digits: ";
	std::cout << std::to_string(node.value.size());
	std::cout << " [end]\n";
}

void ASTPrinter::visit(const nodes::LiteralStringExpression& node)
{
	std::cout << "[AST] literal string expression - length: ";
	std::cout << std::to_string(node.value.size());
	std::cout << " [end]\n";
}

void ASTPrinter::visit(const nodes::LiteralBooleanExpression& node)
{
	std::cout << "[AST] literal boolean expression ";
	std::cout << (node.value ? "true" : "false");
	std::cout << " [end]\n";
}

void ASTPrinter::visit(const nodes::OperatorCallExpression& node)
{
	std::cout << "[AST] operator call expression - ";
	std::cout << std::to_string(node.arguments.size());
	std::cout << " argument(s)\n";
	for(const std::unique_ptr<Expression>& arg : node.arguments)
	{
		arg->accept(*this);
	}
	std::cout << "[--- end operator call expression ---]\n";
}

void ASTPrinter::print_access(const nodes::Access& a)
{
	switch (a.type)
	{
	case nodes::AccessType::PUBLIC:
		std::cout << "public";
		break;
	case nodes::AccessType::PRIVATE:
		std::cout << "private";
		break;
	case nodes::AccessType::PROTECTED:
		std::cout << "protected";
		break;
	case nodes::AccessType::EXCLUSIVE:
		std::cout << "exclusive { ";

		bool first{true};
		for(const nodes::PackageMemberPattern& pmp : a.patterns)
		{
			if(first) { first = false; }
			else { std::cout << ", "; }

			print_package_member_pattern(pmp);
		}
		
		std::cout << " }";
		break;
	}
}

void ASTPrinter::print_package_member_pattern(const nodes::PackageMemberPattern& pmp)
{
	switch (pmp.type)
	{
	case nodes::PackageMemberPatternType::PACKAGE_WITHOUT_SUBPACKAGES:
		std::cout << "shallow pkg ";
		break;
	case nodes::PackageMemberPatternType::PACKAGE_WITH_SUBPACKAGES:
		std::cout << "deep pkg ";
		break;
	default:
		break;
	}
	
	if(pmp.package_member_identifier.has_value())
	{
		std::cout << pmp.package_member_identifier.value().to_string();
	}

	if(pmp.supertype.has_value())
	{
		if(pmp.package_member_identifier.has_value()) { std::cout << " "; }
		std::cout << "extends " << pmp.supertype.value().to_string();
	}
}
