#include "ast_printer.hpp"

#include <iostream>

using namespace neon_compiler::ast::impl;

ASTPrinter::ASTPrinter() {}

void ASTPrinter::visit(const nodes::Root& node)
{
	for(const std::pair<const std::string, std::unique_ptr<nodes::PackageMember>>& pm : node.package_members)
	{
		std::cout << "[AST] PM " << pm.first << " = ";
		pm.second->accept(*this);
		std::cout << "\n";
	}
}

void ASTPrinter::visit(const nodes::Entrypoint& node)
{
	print_access(node.access);
	std::cout << " entrypoint ";
	node.body.accept(*this);
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

}

void ASTPrinter::visit(const nodes::Assignment& node)
{

}

void ASTPrinter::visit(const nodes::StaticFunctionCall& node)
{

}

void ASTPrinter::visit(const nodes::MethodCall& node)
{

}

void ASTPrinter::visit(const nodes::StaticFieldCall& node)
{

}

void ASTPrinter::visit(const nodes::ReferenceCall& node)
{

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

void ASTPrinter::visit(const nodes::GrammarSet& node)
{

}

void ASTPrinter::visit(const nodes::GrammarRule& node)
{

}

void ASTPrinter::visit(const nodes::TokenPattern& node)
{

}

void ASTPrinter::visit(const nodes::ParameterPattern& node)
{

}

void ASTPrinter::visit(const nodes::CompileFunction& node)
{

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
		std::cout << "exclusive {";

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
	// TODO: Align with newest spec
	
	if(pmp.package_member_identifier.has_value())
	{
		std::cout << " " << pmp.package_member_identifier.value().to_string();
	}

	switch (pmp.type)
	{
	case nodes::PackageMemberPatternType::PACKAGE_WITHOUT_SUBPACKAGES:
		std::cout << "*";
		break;
	case nodes::PackageMemberPatternType::PACKAGE_WITH_SUBPACKAGES:
		std::cout << "...";
		break;
	default:
		break;
	}

	if(pmp.supertype.has_value())
	{
		std::cout << " extends " << pmp.supertype.value().to_string();
	}
}
