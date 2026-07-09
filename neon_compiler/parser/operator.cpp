#include "operator.hpp"

#include <stdexcept>

using namespace neon_compiler::parser;
using namespace neon_compiler::ast::nodes;

void Operator::validate()
{
	const std::vector<const OperatorSyntaxPatternElement>& pattern = declaration->pattern;

	if(pattern.size() < PATTERN_MIN_SIZE)
	{
		throw std::invalid_argument(std::string{operator_error_messages::TOO_FEW_PATTERN_ELEMENTS});
	}

	bool ends_with_param = std::holds_alternative<OperatorFunctionParameter>(pattern[pattern.size() - 1]);
	bool starts_with_param = std::holds_alternative<OperatorFunctionParameter>(pattern[0]);

	if(starts_with_param)
	{
		if(ends_with_param)
		{
			fixity = Fixity::INFIX;
		}
		else
		{
			fixity = Fixity::POSTFIX;
		}
	}
	else
	{
		if(ends_with_param)
		{
			fixity = Fixity::PREFIX;
		}
		else
		{
			throw std::invalid_argument(std::string{operator_error_messages::OUTSIDE_PARAMETER_MISSING});
		}
	}
}

std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> Operator::get_declaration() const
{
	return declaration;
}

Fixity Operator::get_fixity() const
{
	return fixity;
}
