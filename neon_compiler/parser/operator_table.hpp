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

	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> MULTIPLY_PATTERN
	{
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"a"},
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::CUSTOM_TOKEN, "*"},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"b"}
	};

	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> ADD_PATTERN
	{
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"a"},
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::CUSTOM_TOKEN, "+"},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"b"}
	};

	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> POWER_PATTERN
	{
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"base"},
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::CUSTOM_TOKEN, "^"},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{"exp"}
	};

	inline const std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> DOT_DECLARATION =
		std::make_shared<const neon_compiler::ast::nodes::OperatorDeclaration>
		(
			DOT_PATTERN,
			0,
			neon_compiler::ast::nodes::OperatorAssociativity::LEFT,
			neon_compiler::ast::nodes::BuiltinOperatorKind::MEMBER_ACCESS
		);

	inline const std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> MULTIPLY_DECLARATION =
		std::make_shared<const neon_compiler::ast::nodes::OperatorDeclaration>
		(
			MULTIPLY_PATTERN,
			2,
			neon_compiler::ast::nodes::OperatorAssociativity::LEFT,
			neon_compiler::ast::nodes::BuiltinOperatorKind::NOT_BUILT_IN
		);

	inline const std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> ADD_DECLARATION =
		std::make_shared<const neon_compiler::ast::nodes::OperatorDeclaration>
		(
			ADD_PATTERN,
			3,
			neon_compiler::ast::nodes::OperatorAssociativity::LEFT,
			neon_compiler::ast::nodes::BuiltinOperatorKind::NOT_BUILT_IN
		);

	inline const std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> POWER_DECLARATION =
		std::make_shared<const neon_compiler::ast::nodes::OperatorDeclaration>
		(
			POWER_PATTERN,
			1,
			neon_compiler::ast::nodes::OperatorAssociativity::RIGHT,
			neon_compiler::ast::nodes::BuiltinOperatorKind::NOT_BUILT_IN
		);

    inline const std::shared_ptr<const parser::Operator> DOT =
		std::make_shared<const parser::Operator>(DOT_DECLARATION);

    inline const std::shared_ptr<const parser::Operator> MULTIPLY =
		std::make_shared<const parser::Operator>(MULTIPLY_DECLARATION);

    inline const std::shared_ptr<const parser::Operator> ADD =
		std::make_shared<const parser::Operator>(ADD_DECLARATION);

    inline const std::shared_ptr<const parser::Operator> POWER =
		std::make_shared<const parser::Operator>(POWER_DECLARATION);
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
		neon_compiler::parser::PeekCursor peek_cursor,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor
	);
	std::shared_ptr<const neon_compiler::parser::Operator> match_infix
	(
		const neon_compiler::TokenReader& reader,
		neon_compiler::parser::PeekCursor peek_cursor,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor
	);
	std::shared_ptr<const neon_compiler::parser::Operator> match_postfix
	(
		const neon_compiler::TokenReader& reader,
		neon_compiler::parser::PeekCursor peek_cursor,
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
		PeekCursor peek_cursor,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor,
		bool skip_fist
	);
};

}

#endif // OPERATOR_TABLE_HPP