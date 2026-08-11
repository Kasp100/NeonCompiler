#ifndef OPERATOR_TABLE_HPP
#define OPERATOR_TABLE_HPP

#include <memory>
#include <vector>
#include <functional>
#include "operator.hpp"
#include "../ast/nodes/nodes.hpp"
#include "../token_reader.hpp"

namespace neon_compiler::parser
{

namespace builtin_operators
{
	// `.` operator

	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> MEMBER_ACCESS_DOT_PATTERN
	{
		neon_compiler::ast::nodes::OperatorSyntaxParameter{},
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::MEMBER_ACCESS_DOT},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{}
	};

	inline const neon_compiler::ast::nodes::OperatorDeclaration MEMBER_ACCESS_DOT_DECLARATION
	{
		MEMBER_ACCESS_DOT_PATTERN,
		0,
		neon_compiler::ast::nodes::OperatorAssociativity::LEFT,
		neon_compiler::ast::nodes::BuiltinOperatorKind::MEMBER_ACCESS
	};

    inline const std::shared_ptr<const parser::Operator> MEMBER_ACCESS_DOT =
		std::make_shared<const parser::Operator>(&MEMBER_ACCESS_DOT_DECLARATION);

	// `=` operator

	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> ASSIGNMENT_PATTERN
	{
		neon_compiler::ast::nodes::OperatorSyntaxParameter{},
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::ASSIGN},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{}
	};

	inline const neon_compiler::ast::nodes::OperatorDeclaration ASSIGNMENT_DECLARATION
	{
		ASSIGNMENT_PATTERN,
		0,
		neon_compiler::ast::nodes::OperatorAssociativity::RIGHT,
		neon_compiler::ast::nodes::BuiltinOperatorKind::ASSIGNMENT
	};

    inline const std::shared_ptr<const parser::Operator> ASSIGNMENT =
		std::make_shared<const parser::Operator>(&ASSIGNMENT_DECLARATION);

	// `give` operator

	inline const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement> GIVE_PATTERN
	{
		neon_compiler::ast::nodes::TokenPattern{neon_compiler::TokenType::GIVE},
		neon_compiler::ast::nodes::OperatorSyntaxParameter{}
	};

	inline const neon_compiler::ast::nodes::OperatorDeclaration GIVE_DECLARATION
	{
		GIVE_PATTERN,
		0,
		neon_compiler::ast::nodes::OperatorAssociativity::RIGHT,
		neon_compiler::ast::nodes::BuiltinOperatorKind::GIVE
	};

    inline const std::shared_ptr<const parser::Operator> GIVE =
		std::make_shared<const parser::Operator>(&GIVE_DECLARATION);

	inline const std::vector<std::shared_ptr<const neon_compiler::parser::Operator>> PREFIX{GIVE};
	inline const std::vector<std::shared_ptr<const neon_compiler::parser::Operator>> INFIX{MEMBER_ACCESS_DOT, ASSIGNMENT};
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
	void add(std::shared_ptr<const neon_compiler::parser::Operator> op);
	void add_all(std::shared_ptr<const OperatorTable> other);

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
	void check_not_finalised() const;
	void sort_operator_list(std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>& list);

	std::shared_ptr<const neon_compiler::parser::Operator> match
	(
		const std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>& operators,
		const neon_compiler::TokenReader& reader,
		PeekCursor peek_cursor,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor,
		bool skip_fist
	);
};

}

#endif // OPERATOR_TABLE_HPP