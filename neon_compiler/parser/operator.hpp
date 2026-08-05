#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include <functional>
#include "../token.hpp"
#include "../token_reader.hpp"
#include "../ast/nodes/nodes.hpp"

namespace neon_compiler::parser
{

constexpr std::size_t PATTERN_MIN_SIZE = 2;
	
namespace operator_error_messages
{
	constexpr std::string_view TOO_FEW_PATTERN_ELEMENTS =
		"Too few pattern elements. An operator must have at least one parameter and one token, e.g. `__!`.";
	constexpr std::string_view OUTSIDE_PARAMETER_MISSING =
		"Outside parameter missing. An operator must have at least one parameter at the outside, e.g. `-__`.";
}

enum class Fixity
{
	PREFIX,
	INFIX,
	POSTFIX,
	INVALID
};

using FuncParseExpressionWCursor = std::function<uint(uint peek_offset, uint expression_max_subordination)>;

using PeekCursor = uint*;

class Operator
{
public:
	explicit Operator(const neon_compiler::ast::nodes::OperatorDeclaration* init_declaration);

	const neon_compiler::ast::nodes::OperatorDeclaration* get_declaration() const;
	Fixity get_fixity() const;
	bool operator<(const Operator& other) const;
	bool matches
	(
		const neon_compiler::TokenReader& reader,
		PeekCursor peek_cursor,
		const FuncParseExpressionWCursor& func_parse_expression_w_cursor,
		bool skip_first
	) const;
private:
	/** Declaration of this operator, non-owning */
	const neon_compiler::ast::nodes::OperatorDeclaration* declaration;
	/** Fixity of the operator: prefix, infix, or postfix */
	Fixity fixity;

	void validate();
	bool is_less_specific_from(const Operator& other, std::size_t check_index = 0) const;
	static std::size_t count_consecutive_tokens
	(
		const std::vector<neon_compiler::ast::nodes::OperatorSyntaxPatternElement>& pattern,
		std::size_t from
	);
};

}

#endif // OPERATOR_HPP