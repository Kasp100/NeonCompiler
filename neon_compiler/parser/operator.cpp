#include "operator.hpp"

#include <stdexcept>
#include "../token_reader.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::ast::nodes;

void Operator::validate()
{
	const std::vector<OperatorSyntaxPatternElement>& pattern = declaration->pattern;

	if(pattern.size() < PATTERN_MIN_SIZE)
	{
		throw std::invalid_argument{std::string{operator_error_messages::TOO_FEW_PATTERN_ELEMENTS}};
	}

	bool ends_with_param = std::holds_alternative<OperatorSyntaxParameter>(pattern[pattern.size() - 1]);
	bool starts_with_param = std::holds_alternative<OperatorSyntaxParameter>(pattern[0]);

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
			throw std::invalid_argument{std::string{operator_error_messages::OUTSIDE_PARAMETER_MISSING}};
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

bool Operator::operator<(const Operator& other) const
{
	return is_less_specific_from(other);
}

bool Operator::is_less_specific_from(const Operator& other, std::size_t check_index) const
{
	if(check_index >= declaration->pattern.size())
	{
		return other.declaration->pattern.size() > declaration->pattern.size();
	}

	std::size_t consecutive_tokens_count_this = count_consecutive_tokens(declaration->pattern, check_index);
	std::size_t consecutive_tokens_count_other = count_consecutive_tokens(other.declaration->pattern, check_index);

	if(consecutive_tokens_count_other == consecutive_tokens_count_this)
	{
		return is_less_specific_from(other, check_index + consecutive_tokens_count_this + 1);
	}
	else
	{
		return consecutive_tokens_count_other < consecutive_tokens_count_this;
	}
}

std::size_t Operator::count_consecutive_tokens(const std::vector<OperatorSyntaxPatternElement>& pattern, std::size_t from)
{
	std::size_t count = 0;

	for(std::size_t i = from; i < pattern.size(); ++i)
	{
		if(std::holds_alternative<OperatorSyntaxParameter>(pattern[i]))
		{
			break;
		}
		++count;
	}

	return count;
}

bool Operator::matches
(
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor,
	bool skip_first
) const
{
	uint peek_offset{peek_cursor ? *peek_cursor : 0}; 

	const std::vector<OperatorSyntaxPatternElement>& pattern = get_declaration()->pattern;

	for(std::size_t i = (skip_first ? 1 : 0); i < pattern.size(); ++i)
	{
		const OperatorSyntaxPatternElement& elem = pattern[i];

		if(std::holds_alternative<OperatorSyntaxParameter>(elem))
		{
			peek_offset = func_parse_expression_w_cursor(peek_offset);
			continue;
		}

		if(reader.peek(peek_offset).get_type() == std::get<TokenPattern>(elem).token_type &&
			reader.peek(peek_offset).get_lexeme() == std::get<TokenPattern>(elem).lexeme)
		{
			++peek_offset;
			continue;
		}

		return false;
	}

	return true;
}
