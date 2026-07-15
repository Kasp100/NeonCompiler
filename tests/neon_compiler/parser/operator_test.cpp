#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libs/doctest/doctest.hpp"

#include "../../../neon_compiler/token.hpp"
#include "../../../neon_compiler/ast/nodes/nodes.hpp"
#include "../../../neon_compiler/parser/operator.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::ast::nodes;

TEST_CASE("Operators compare specificity correctly")
{
	// Arrange
	std::shared_ptr<OperatorDeclaration> op_decl_0 = std::make_shared<OperatorDeclaration>
	(
		std::vector<OperatorSyntaxPatternElement>
		{
			OperatorSyntaxParameter{"a"},
			TokenPattern{TokenType::CUSTOM_TOKEN, "+"},
			OperatorSyntaxParameter{"b"}
		},
		0,
		OperatorAssociativity::LEFT,
		BuiltinOperatorKind::NOT_BUILT_IN
	);

	std::shared_ptr<const Operator> op_0 = std::make_shared<const Operator>(op_decl_0);

	std::shared_ptr<OperatorDeclaration> op_decl_1 = std::make_shared<OperatorDeclaration>
	(
		std::vector<OperatorSyntaxPatternElement>
		{
			OperatorSyntaxParameter{"a"},
			TokenPattern{TokenType::CUSTOM_TOKEN, "-"},
			OperatorSyntaxParameter{"b"}
		},
		0,
		OperatorAssociativity::LEFT,
		BuiltinOperatorKind::NOT_BUILT_IN
	);

	std::shared_ptr<const Operator> op_1 = std::make_shared<const Operator>(op_decl_1);

	std::shared_ptr<OperatorDeclaration> op_decl_2 = std::make_shared<OperatorDeclaration>
	(
		std::vector<OperatorSyntaxPatternElement>
		{
			OperatorSyntaxParameter{"a"},
			TokenPattern{TokenType::CUSTOM_TOKEN, "["},
			OperatorSyntaxParameter{"b"},
			TokenPattern{TokenType::CUSTOM_TOKEN, "]"}
		},
		0,
		OperatorAssociativity::LEFT,
		BuiltinOperatorKind::NOT_BUILT_IN
	);

	std::shared_ptr<const Operator> op_2 = std::make_shared<const Operator>(op_decl_2);

	// Act & Assert
	CHECK(!(*op_0 < *op_1));
	CHECK(!(*op_1 < *op_0));
	CHECK(*op_0 < *op_2);
	CHECK(*op_1 < *op_2);

	std::vector<std::shared_ptr<const Operator>> list;

	list.push_back(op_0);
	list.push_back(op_1);
	list.push_back(op_2);

	std::sort
	(
		list.begin(),
		list.end(),
		[](const std::shared_ptr<const Operator>& a, const std::shared_ptr<const Operator>& b)
		{
			return *b < *a;
		}
	);

	CHECK(list[0] == op_2);
}