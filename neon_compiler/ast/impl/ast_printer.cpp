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
		print_prefix();
		print("package member ");
		print(pm.first);
		print_line();

		incr_depth();
		pm.second->accept(*this);
		decr_depth();
	}
}

void ASTPrinter::visit(const nodes::Entrypoint& node)
{
	print_prefix();
	print_access(node.access);
	print(" entrypoint");
	print_line();

	incr_depth();
	node.body.accept(*this);
	decr_depth();
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
	print_prefix();
	print("code block");
	print_line();

	incr_depth();
	for(const std::unique_ptr<Statement>& stmt : node.statements)
	{
		stmt->accept(*this);
	}
	decr_depth();
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
	print_prefix();
	print("return");
	print_line();

	incr_depth();
	node.value->accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::Assignment& node)
{

}

void ASTPrinter::visit(const nodes::ObjectFunctionCall& node)
{
	print_prefix();
	print("object function call - function name: ");
	print(node.function_name);
	print_line();

	incr_depth();
	node.object->accept(*this);
	for(const std::unique_ptr<Expression>& arg : node.arguments)
	{
		arg->accept(*this);
	}
	decr_depth();
}

void ASTPrinter::visit(const nodes::ObjectRead& node)
{
	print_prefix();
	print("object read - reference name: ");
	print(node.reference_name);
	print_line();
	
	incr_depth();
	node.object->accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::FunctionCall& node)
{
	print_prefix();
	print("function call - function name: ");
	print(node.function_name);
	print_line();

	incr_depth();
	for(const std::unique_ptr<Expression>& arg : node.arguments)
	{
		arg->accept(*this);
	}
	decr_depth();
}

void ASTPrinter::visit(const nodes::SimpleRead& node)
{
	print_prefix();
	print("simple read - reference name: ");
	print(node.reference_name);
	print_line();
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
	print_prefix();
	print("literal number expression - value: ");
	print(node.value);
	print_line();
}

void ASTPrinter::visit(const nodes::LiteralStringExpression& node)
{
	print_prefix();
	print("literal string expression - length: ");
	print(std::to_string(node.value.size()));
	print_line();
}

void ASTPrinter::visit(const nodes::LiteralBooleanExpression& node)
{
	print_prefix();
	print("literal boolean expression ");
	print(node.value ? "true" : "false");
	print_line();
}

void ASTPrinter::visit(const nodes::OperatorCallExpression& node)
{
	print_prefix();
	print("operator call expression - ");
	print(std::to_string(node.arguments.size()));
	print(" argument(s)");
	print_line();

	const std::vector<std::unique_ptr<Expression>>& args = node.arguments;
	const std::vector<OperatorSyntaxPatternElement>& pattern = node.op->get_declaration()->pattern;

	uint arg_i{0};

	incr_depth();
	for(const OperatorSyntaxPatternElement& elem : pattern)
	{
		if(std::holds_alternative<TokenPattern>(elem))
		{
			const TokenPattern& tp = std::get<TokenPattern>(elem);
			print_prefix();
			print("token - type: ");
			print(std::to_string(static_cast<int>(tp.token_type)));

			if(tp.lexeme.has_value())
			{
				const std::string& v = tp.lexeme.value();
				print(", lexeme: ");
				print(v);
			}
			
			print_line();
		}
		else
		{
			const OperatorSyntaxParameter& param = std::get<OperatorSyntaxParameter>(elem);
			print_prefix();
			print("param ");
			print(param.name);
			print_line();

			incr_depth();
			args.at(arg_i)->accept(*this);
			decr_depth();
			++arg_i;
		}
	}
	decr_depth();
}

void ASTPrinter::incr_depth()
{
	++depth;
}

void ASTPrinter::decr_depth()
{
	--depth;
}

void ASTPrinter::print(const std::string& str) const
{
	std::cout << str;
}

void ASTPrinter::print_prefix() const
{
	print("[AST] ");
	for(uint i = 0; i < depth; ++i)
	{
		print("|  ");
	}
	print("|--");
}

void ASTPrinter::print_line() const
{
	print("\n");
}

void ASTPrinter::print_access(const nodes::Access& a) const
{
	switch (a.type)
	{
	case nodes::AccessType::PUBLIC:
		print("public");
		break;
	case nodes::AccessType::PRIVATE:
		print("private");
		break;
	case nodes::AccessType::PROTECTED:
		print("protected");
		break;
	case nodes::AccessType::EXCLUSIVE:
		print("exclusive { ");

		bool first{true};
		for(const nodes::PackageMemberPattern& pmp : a.patterns)
		{
			if(first) { first = false; }
			else { print(", "); }

			print_package_member_pattern(pmp);
		}
		
		print(" }");
		break;
	}
}

void ASTPrinter::print_package_member_pattern(const nodes::PackageMemberPattern& pmp) const
{
	switch (pmp.type)
	{
	case nodes::PackageMemberPatternType::PACKAGE_WITHOUT_SUBPACKAGES:
		print("shallow pkg ");
		break;
	case nodes::PackageMemberPatternType::PACKAGE_WITH_SUBPACKAGES:
		print("deep pkg ");
		break;
	default:
		break;
	}
	
	if(pmp.package_member_identifier.has_value())
	{
		print(pmp.package_member_identifier.value().to_string());
	}

	if(pmp.supertype.has_value())
	{
		if(pmp.package_member_identifier.has_value()) { print(" "); }
		print("extends ");
		print(pmp.supertype.value().to_string());
	}
}
