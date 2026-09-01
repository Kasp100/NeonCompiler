#ifndef STATEMENT_NODES_HPP
#define STATEMENT_NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include "nodes.hpp"
#include "../ast_node.hpp"
#include "../../token.hpp"
#include "../../parser/operator.hpp"

namespace neon_compiler::ast::nodes
{

struct DiscardExpression : Statement
{
	/** The expression which will be evaluated, discarding the result. Typically done with `void` calls. */
	std::unique_ptr<Expression> expression;

	explicit DiscardExpression
	(
		std::unique_ptr<Expression> init_expression
	) :
		expression{std::move(init_expression)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct LocalDeclarationOrAssignment : Statement
{
	/** The variable declaration within this statement */
	VariableDeclaration declaration_or_assignment;

	explicit LocalDeclarationOrAssignment
	(
		VariableDeclaration init_declaration_or_assignment
	) :
		declaration_or_assignment{std::move(init_declaration_or_assignment)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Call to a compile function */
struct AutoCall : Statement
{
	/** The name matching the one of a compile function name */
	std::string function_name;
	/** Tokens to pass. `,` separates arguments. */
	std::vector<std::vector<neon_compiler::Token>> arguments;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Return : Statement
{
	/** Optional return value. `nullptr` means void is returned. */
	std::unique_ptr<Expression> value;

	explicit Return
	(
		std::unique_ptr<Expression> init_value
	) :
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Assignment : Expression
{
	std::unique_ptr<Expression> target;
	std::unique_ptr<Expression> value;

	explicit Assignment
	(
		std::unique_ptr<Expression> init_target,
		std::unique_ptr<Expression> init_value
	) :
		target{std::move(init_target)},
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Give : Expression
{
	std::unique_ptr<Expression> value;

	explicit Give
	(
		std::unique_ptr<Expression> init_value
	) :
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GiveAndAssign : Expression
{
	std::unique_ptr<Expression> value;
	std::unique_ptr<Expression> replacement;

	explicit GiveAndAssign
	(
		std::unique_ptr<Expression> init_value,
		std::unique_ptr<Expression> init_replacement
	) :
		value{std::move(init_value)},
		replacement{std::move(init_replacement)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Represents an function call on a value from an expression.
 * Example: `background_colour.darker()`, here `background_colour` is a simple read expression */
struct ObjectFunctionCall : Expression
{
	/** The object */
	std::unique_ptr<Expression> object;
	/** Member function name */
	std::string member_name;
	/** Generic arguments */
	std::vector<GenericArgument> generic_arguments;
	/** Arguments */
	std::vector<std::unique_ptr<Expression>> arguments;

	explicit ObjectFunctionCall
	(
		std::unique_ptr<Expression> init_object,
		std::string init_member_name,
		std::vector<GenericArgument> init_generic_arguments,
		std::vector<std::unique_ptr<Expression>> init_arguments
	) :
		object{std::move(init_object)},
		member_name{std::move(init_member_name)},
		generic_arguments{std::move(init_generic_arguments)},
		arguments{std::move(init_arguments)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Represents a read from a value from an expression.
 * Example: `background_colour.red`, here `background_colour` is a simple read expression */
struct ObjectRead : Expression
{
	/** The object */
	std::unique_ptr<Expression> object;
	/** Member name */
	std::string member_name;

	explicit ObjectRead
	(
		std::unique_ptr<Expression> init_object,
		std::string init_member_name
	) :
		object{std::move(init_object)},
		member_name{std::move(init_member_name)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Represents a simple call to a constructor, function, or method if this is inside a type.
 * Example: `fs_path("/home/user")`, here an instance of `ps_path` (a type) is made by calling the constructor,
 * which takes a single string. */
struct FunctionCall : Expression
{
	/** Function name */
	std::string function_name;
	/** Generic arguments */
	std::vector<GenericArgument> generic_arguments;
	/** Arguments */
	std::vector<std::unique_ptr<Expression>> arguments;

	explicit FunctionCall
	(
		std::string init_function_name,
		std::vector<GenericArgument> init_generic_arguments,
		std::vector<std::unique_ptr<Expression>> init_arguments
	) :
		function_name{std::move(init_function_name)},
		generic_arguments{std::move(init_generic_arguments)},
		arguments{std::move(init_arguments)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Represents a read from local variable, a constant, or a field (if this is inside a class), e.g. `speed` */
struct SimpleRead : Expression
{
	/** Reference name */
	std::string reference_name;

	explicit SimpleRead
	(
		std::string init_reference_name
	) :
		reference_name{std::move(init_reference_name)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** The node representing `empty` */
struct OptionalEmpty : Expression
{
	explicit OptionalEmpty() {}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct LiteralNumberExpression : Expression
{
	std::string value;

	explicit LiteralNumberExpression
	(
		std::string init_value
	) :
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct LiteralStringExpression : Expression
{
	std::string value;

	explicit LiteralStringExpression
	(
		std::string init_value
	) :
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct LiteralBooleanExpression : Expression
{
	bool value;

	explicit LiteralBooleanExpression
	(
		bool init_value
	) :
		value{init_value}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct OperatorCallExpression : Expression
{
	std::vector<std::unique_ptr<Expression>> arguments;
	std::shared_ptr<const neon_compiler::parser::Operator> op;

	explicit OperatorCallExpression
	(
		std::vector<std::unique_ptr<Expression>> init_arguments,
		std::shared_ptr<const neon_compiler::parser::Operator> init_op
	)
		: arguments{std::move(init_arguments)}, op{init_op}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct CheckPresence : Expression
{
	/** Value to check for presence. */
	std::unique_ptr<Expression> value;

	explicit CheckPresence
	(
		std::unique_ptr<Expression> init_value
	) :
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct CheckAbsence : Expression
{
	/** Value to check for absence. */
	std::unique_ptr<Expression> value;

	explicit CheckAbsence
	(
		std::unique_ptr<Expression> init_value
	) :
		value{std::move(init_value)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Fallback : Expression
{
	std::unique_ptr<Expression> optional;
	std::unique_ptr<Expression> fallback;

	explicit Fallback
	(
		std::unique_ptr<Expression> init_optional,
		std::unique_ptr<Expression> init_fallback
	) :
		optional{std::move(init_optional)},
		fallback{std::move(init_fallback)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

}

#endif // STATEMENT_NODES_HPP