#include "operator_table.hpp"

#include <stdexcept>
#include <algorithm>

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::ast::nodes;

OperatorTable::OperatorTable()
{
	add(builtin_operators::DOT);
	add(builtin_operators::MULTIPLY);
	add(builtin_operators::ADD);
	add(builtin_operators::POWER);
}

void OperatorTable::add(std::shared_ptr<const neon_compiler::parser::Operator> op)
{
	if(finalised)
	{
		throw std::logic_error{std::string{operator_table_error_messages::ADD_AFTER_FINALISE}};
	}

	switch(op->get_fixity())
	{
		case Fixity::PREFIX:  { prefix_operators.push_back(op);   break; }
		case Fixity::INFIX:   { infix_operators.push_back(op);    break; }
		case Fixity::POSTFIX: { postfix_operators.push_back(op);  break; }
		default: { throw std::invalid_argument{std::string{operator_table_error_messages::INVALID_FIXITY}}; }
	}
}

std::shared_ptr<const Operator> OperatorTable::match_prefix
(
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor
)
{
	return match(prefix_operators, reader, peek_cursor, func_parse_expression_w_cursor, false);
}

std::shared_ptr<const Operator> OperatorTable::match_infix
(
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor
)
{
	return match(infix_operators, reader, peek_cursor, func_parse_expression_w_cursor, true);
}

std::shared_ptr<const Operator> OperatorTable::match_postfix
(
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor
)
{
	return match(postfix_operators, reader, peek_cursor, func_parse_expression_w_cursor, true);
}

void OperatorTable::finalise()
{
	sort_operator_list(prefix_operators);
	sort_operator_list(infix_operators);
	sort_operator_list(postfix_operators);

	finalised = true;
}

void OperatorTable::sort_operator_list(std::vector<std::shared_ptr<const Operator>>& list)
{
	std::sort
	(
		list.begin(),
		list.end(),
		[](const std::shared_ptr<const Operator>& a, const std::shared_ptr<const Operator>& b)
		{
			return *b < *a;
		}
	);
}

std::shared_ptr<const Operator> OperatorTable::match
(
	std::vector<std::shared_ptr<const Operator>>& operators,
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor,
	bool skip_first
)
{
	if(!finalised)
	{
		finalise();
	}

	for(std::size_t i = 0; i < operators.size(); ++i)
	{
		if(operators[i]->matches(reader, peek_cursor, func_parse_expression_w_cursor, skip_first))
		{
			return operators[i];
		}
	}

	return nullptr;
}
