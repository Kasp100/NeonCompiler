#include "operator_table.hpp"

#include <stdexcept>
#include <algorithm>

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::ast::nodes;

void OperatorTable::add(std::shared_ptr<const neon_compiler::parser::Operator> op)
{
	check_not_finalised();

	switch(op->get_fixity())
	{
		case Fixity::PREFIX:  { prefix_operators.push_back(op);   break; }
		case Fixity::INFIX:   { infix_operators.push_back(op);    break; }
		case Fixity::POSTFIX: { postfix_operators.push_back(op);  break; }
		default: { throw std::invalid_argument{std::string{operator_table_error_messages::INVALID_FIXITY}}; }
	}
}

void OperatorTable::add_all(std::shared_ptr<const OperatorTable> other)
{
	check_not_finalised();

	for(std::shared_ptr<const Operator> op : other->prefix_operators)
	{
		prefix_operators.push_back(op);
	}

	for(std::shared_ptr<const Operator> op : other->infix_operators)
	{
		infix_operators.push_back(op);
	}

	for(std::shared_ptr<const Operator> op : other->postfix_operators)
	{
		postfix_operators.push_back(op);
	}
}

void OperatorTable::check_not_finalised() const
{
	if(finalised)
	{
		throw std::logic_error{std::string{operator_table_error_messages::ADD_AFTER_FINALISE}};
	}
}

std::shared_ptr<const Operator> OperatorTable::match_prefix
(
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor
)
{
	std::shared_ptr<const Operator> builtin_operator_match =
		match(builtin_operators::PREFIX, reader, peek_cursor, func_parse_expression_w_cursor, false);

	if(builtin_operator_match)
	{
		return builtin_operator_match;
	}

	return match(prefix_operators, reader, peek_cursor, func_parse_expression_w_cursor, false);
}

std::shared_ptr<const Operator> OperatorTable::match_infix
(
	const TokenReader& reader,
	PeekCursor peek_cursor,
	const FuncParseExpressionWCursor& func_parse_expression_w_cursor
)
{
	std::shared_ptr<const Operator> builtin_operator_match =
		match(builtin_operators::INFIX, reader, peek_cursor, func_parse_expression_w_cursor, true);

	if(builtin_operator_match)
	{
		return builtin_operator_match;
	}

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
	const std::vector<std::shared_ptr<const Operator>>& operators,
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
