#ifndef OPERATOR_TABLE_HPP
#define OPERATOR_TABLE_HPP

#include <memory>
#include <vector>
#include <functional>
#include "../parser/operator.hpp"
#include "../ast/nodes/nodes.hpp"
#include "../token_reader.hpp"

namespace neon_compiler::parser
{

namespace builtin_operators
{
	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> DOT_PATTERN
	{
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"lhs"},
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::DOT},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"rhs"}
	};

	inline const std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> DOT_DECLARATION =
		std::make_shared<const neon_compiler::ast::nodes::OperatorDeclaration>
		(
			DOT_PATTERN,
			0,
			neon_compiler::ast::nodes::OperatorAssociativity::LEFT,
			neon_compiler::ast::nodes::BuiltinOperatorKind::MEMBER_ACCESS
		);

    inline const std::shared_ptr<const parser::Operator> DOT =
		std::make_shared<const parser::Operator>(DOT_DECLARATION);
}

namespace operator_table_error_messages
{
	constexpr std::string_view ADD_AFTER_FINALISE =
		"Add after finalise";
	constexpr std::string_view INVALID_FIXITY =
		"Invalid operator fixity";
}

class OperatorTable
{
public:
	explicit OperatorTable();
	void add(std::shared_ptr<const neon_compiler::parser::Operator> op);

	std::shared_ptr<const neon_compiler::parser::Operator> match_prefix
	(
		const neon_compiler::TokenReader& reader,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor
	);
	std::shared_ptr<const neon_compiler::parser::Operator> match_infix
	(
		const neon_compiler::TokenReader& reader,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor
	);
	std::shared_ptr<const neon_compiler::parser::Operator> match_postfix
	(
		const neon_compiler::TokenReader& reader,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor
	);
private:
	std::vector<std::shared_ptr<const neon_compiler::parser::Operator>> prefix_operators;
	std::vector<std::shared_ptr<const neon_compiler::parser::Operator>> infix_operators;
	std::vector<std::shared_ptr<const neon_compiler::parser::Operator>> postfix_operators;
	bool finalised{false};

	void finalise();
	void sort_operator_list(std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>& list);

	std::shared_ptr<const neon_compiler::parser::Operator> match
	(
		std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>& operators,
		const neon_compiler::TokenReader& reader,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor
	);
};

}

#endif // OPERATOR_TABLE_HPP