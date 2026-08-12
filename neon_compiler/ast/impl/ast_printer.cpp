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

	print_prefix();
	print("parameters:");
	print_line();

	incr_depth();
	for(const VariableDeclaration& var_decl : node.parameters)
	{
		var_decl.accept(*this);
	}
	decr_depth();

	print_prefix();
	print("body:");
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
	print_prefix();

	if(node.var)
	{
		print("var ");
	}

	node.reference_type.accept(*this);

	print(" ");

	print(node.reference_name);

	if(node.initialisation)
	{
		print(" =");
		print_line();

		incr_depth();
		node.initialisation->accept(*this);
		decr_depth();
	}
	else
	{
		print_line();
	}
}

void ASTPrinter::visit(const nodes::ConstantDeclaration& node)
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
	if(node.opt)
	{
		print("opt ");
	}

	switch (node.mutability)
	{
	case MutabilityMode::OWN:
		print("own ");
		break;
	case MutabilityMode::SHARED:
		print("shared ");
		break;
	case MutabilityMode::BORROW:
		print("borrow ");
		break;
	default:
		print("err_mutability_mode ");
		break;
	}

	if(node.mut)
	{
		print("mut:");
	}

	print(node.type);

	if(node.generic_arguments.size() == 0) { return; }

	print("<");

	bool first{true};
	for(const GenericArgument& generic_arg : node.generic_arguments)
	{
		if(first) { first = false; } else { print(", "); }
		print_generic_argument(generic_arg);
	}

	print(">");
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
	print_prefix();
	print("discard expression");
	print_line();

	incr_depth();
	node.expression->accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::LocalDeclaration& node)
{
	print_prefix();
	print("local declaration:");
	print_line();

	incr_depth();
	node.variable_declaration.accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::AutoCall& node)
{

}

void ASTPrinter::visit(const nodes::Return& node)
{
	print_prefix();
	print("return");
	print_line();

	if(node.value)
	{
		incr_depth();
		node.value->accept(*this);
		decr_depth();
	}
}

void ASTPrinter::visit(const nodes::Assignment& node)
{
	print_prefix();
	print("assignment");
	print_line();

	incr_depth();

	node.target->accept(*this);

	print_prefix();
	print("new value:");
	print_line();
	incr_depth();
	node.value->accept(*this);
	decr_depth();

	decr_depth();
}

void ASTPrinter::visit(const nodes::Give& node)
{
	print_prefix();
	print("give");
	print_line();

	incr_depth();
	node.value->accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::ObjectFunctionCall& node)
{
	print_prefix();
	print("object function call - member name: ");
	print(node.member_name);
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
	print("object read - member name: ");
	print(node.member_name);
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

	if(node.generic_arguments.size() > 0)
	{
		print_prefix();
		print("generic arguments: <");
		bool first{true};
		for(const GenericArgument& generic_arg : node.generic_arguments)
		{
			if(first) { first = false; } else { print(", "); }
			print_generic_argument(generic_arg);
		}
		print(">");
		print_line();
	}

	print_prefix();
	print("arguments:");
	print_line();

	incr_depth();
	for(const std::unique_ptr<Expression>& arg : node.arguments)
	{
		arg->accept(*this);
	}
	decr_depth();

	decr_depth();
}

void ASTPrinter::visit(const nodes::SimpleRead& node)
{
	print_prefix();
	print("simple read - reference name: ");
	print(node.reference_name);
	print_line();
}

void ASTPrinter::visit(const nodes::OptionalEmpty& node)
{
	print_prefix();
	print("empty");
	print_line();
}

void ASTPrinter::visit(const nodes::PureFunctionSet& node)
{

}

void ASTPrinter::visit(const nodes::PureFunction& node)
{

}

void ASTPrinter::visit(const nodes::OperatorModule& node)
{
	print_prefix();
	print_access(node.access);
	print(" operator module");
	print_line();

	incr_depth();
	for(const OperatorDeclaration& od : node.operators)
	{
		od.accept(*this);
	}
	for(const OperatorFunction& of : node.functions)
	{
		of.accept(*this);
	}
	decr_depth();
}

void ASTPrinter::visit(const nodes::OperatorDeclaration& node)
{
	print_prefix();

	print("operator declaration - subordination: ");
	print(std::to_string(node.subordination));
	print(", associativity: ");

	switch (node.associativity)
	{
		case OperatorAssociativity::NONE:
			print("none");
			break;
		case OperatorAssociativity::LEFT:
			print("left");
			break;
		case OperatorAssociativity::RIGHT:
			print("right");
			break;
		default:
			print("err");
			break;
	}

	print(", pattern: ");
	print_line();

	incr_depth();
	for(const OperatorSyntaxPatternElement& elem : node.pattern)
	{
		print_prefix();
		if(std::holds_alternative<OperatorSyntaxParameter>(elem))
		{
			print("parameter");
		}
		else
		{
			const TokenPattern& tp = std::get<TokenPattern>(elem);

			print("token - type: ");
			print(std::to_string(static_cast<int>(tp.token_type)));

			if(tp.lexeme.has_value())
			{
				print(", lexeme: ");
				print(tp.lexeme.value());
			}
		}
		print_line();
	}
	decr_depth();
}

void ASTPrinter::visit(const nodes::OperatorFunction& node)
{
	print_prefix();
	print("operator function:");
	print_line();

	incr_depth();

	if(node.generic_parameters.size() > 0)
	{
		print_prefix();
		print("generic parameters: ");
		print_generic_parameters(node.generic_parameters);
		print_line();
	}

	print_prefix();
	print("pattern:");
	print_line();

	incr_depth();
	for(const OperatorFunctionPatternElement& elem : node.pattern)
	{
		print_prefix();
		if(std::holds_alternative<OperatorFunctionParameter>(elem))
		{
			const OperatorFunctionParameter& param = std::get<OperatorFunctionParameter>(elem);
			
			print("parameter:");
			print_line();

			incr_depth();
			param.parameter.accept(*this);
			decr_depth();
		}
		else
		{
			const TokenPattern& tp = std::get<TokenPattern>(elem);

			print("token - type: ");
			print(std::to_string(static_cast<int>(tp.token_type)));

			if(tp.lexeme.has_value())
			{
				print(", lexeme: ");
				print(tp.lexeme.value());
			}

			print_line();
		}
	}
	decr_depth();

	print_prefix();
	print("return type: ");
	node.return_type.accept(*this);
	print_line();

	print_prefix();
	print("body:");
	print_line();

	incr_depth();
	node.body.accept(*this);
	decr_depth();

	decr_depth();
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
			print_prefix();
			print("parameter");
			print_line();

			incr_depth();
			args.at(arg_i)->accept(*this);
			decr_depth();
			++arg_i;
		}
	}
	decr_depth();
}

void ASTPrinter::visit(const nodes::CheckPresence& node)
{
	print_prefix();
	print("check presence");
	print_line();
	
	incr_depth();
	node.value->accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::CheckAbsence& node)
{
	print_prefix();
	print("check absence");
	print_line();
	
	incr_depth();
	node.value->accept(*this);
	decr_depth();
}

void ASTPrinter::visit(const nodes::Fallback& node)
{
	print_prefix();
	print("fallback");
	print_line();

	incr_depth();

	print_prefix();
	print("optional:");
	print_line();
	incr_depth();
	node.optional->accept(*this);
	decr_depth();

	print_prefix();
	print("fallback:");
	print_line();
	incr_depth();
	node.fallback->accept(*this);
	decr_depth();

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

void ASTPrinter::print_generic_argument(const nodes::GenericArgument& generic_arg) const
{
	print(generic_arg.value);

	if(generic_arg.nested_generic_args.size() == 0)
	{
		return;
	}

	print("<");

	bool first{true};
	for(const nodes::GenericArgument& element_generic_arg : generic_arg.nested_generic_args)
	{
		if(first) { first = false; } else { print(", "); }

		print_generic_argument(element_generic_arg);
	}

	print(">");
}

void ASTPrinter::print_generic_parameters(const std::vector<nodes::GenericParameter>& generic_params) const
{
	print("<");

	bool first_p{true};
	for(const nodes::GenericParameter& p : generic_params)
	{
		if(first_p) { first_p = false; } else { print(", "); }

		print(p.type);
		print(" ");
		print(p.reference_name);

		const std::vector<std::string>& supertypes = p.supertypes;

		if(supertypes.size() == 0) { continue; }

		print(" {");

		bool first_st{true};
		for(const std::string& st : supertypes)
		{
			if(first_st) { first_st = false; } else { print(", "); }

			print(st);
		}

		print("}");
	}

	print(">");
}
