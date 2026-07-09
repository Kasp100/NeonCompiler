#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "../token.hpp"
#include "../ast/nodes/nodes.hpp"

namespace neon_compiler::parser
{

constexpr std::size_t PATTERN_MIN_SIZE = 2;
	
namespace operator_error_messages
{
	constexpr std::string_view TOO_FEW_PATTERN_ELEMENTS =
		"Too few pattern elements; an operator must have at least one parameter and one token, e.g. `(v)!`.";
	constexpr std::string_view OUTSIDE_PARAMETER_MISSING =
		"Outside parameter missing; an operator must have at least one parameter at the outside, e.g. `-(v)`";
}

enum class Fixity
{
	PREFIX,
	INFIX,
	POSTFIX,
	INVALID
};

class Operator
{
public:
	Operator(std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> declaration)
	: declaration{declaration}, fixity{Fixity::INVALID}
	{
		validate();
	}

	std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> get_declaration() const;
	Fixity get_fixity() const;
	bool operator<(const Operator& other) const;
private:
	/** Declaration of this operator */
	std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> declaration;
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