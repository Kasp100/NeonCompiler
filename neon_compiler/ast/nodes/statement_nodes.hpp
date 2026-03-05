#ifndef STATEMENT_NODES_HPP
#define STATEMENT_NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include "nodes.hpp"
#include "../ast_node.hpp"
#include "../../token.hpp"

namespace neon_compiler::ast::nodes
{

struct DiscardExpression : Statement
{
    /** The expression which will be evaluated, discarding the result. Typically done with `void` calls. */
	std::unique_ptr<Expression> expression;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct LocalDeclaration : Statement
{
	/** The variable declaration within this statement */
    VariableDeclaration variable_declaration;

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
	/** Tokens to pass. `,` separates parameters. */
	std::vector<std::vector<neon_compiler::Token>> parameters;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Return : Statement
{
    /** Optional return value. `nullptr` means void is returned. */
	std::unique_ptr<Expression> value;

	Return(std::unique_ptr<Expression> value)
		: value(std::move(value)) {}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Assignment : Expression
{
    /** Reference being assigned */
    std::string variable_name;
    /** New value */
	std::unique_ptr<Expression> expression;

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
    /** Function name */
    std::string function_name;
	/** Parameter values */
	std::vector<std::unique_ptr<Expression>> parameters;
};

/** Represents a read from a value from an expression.
 * Example: `background_colour.red`, here `background_colour` is a simple read expression */
struct ObjectReadExpression : Expression
{
    /** The object */
    std::unique_ptr<Expression> object;
    /** Reference name */
    std::string reference_name;
};

/** Represents a simple call to a constructor, pure function, method (if this is inside a type), or entrypoint.
 * Example: `fs_path("/home/user")`, here an instance of `ps_path` (a type) is made by calling the constructor,
 * which takes a single string. */
struct FunctionCall : Expression
{
    /** Function name */
    std::string function_name;
	/** Parameter values */
	std::vector<std::unique_ptr<Expression>> parameters;
};

/** Represents a read from local variable, a constant, or a field (if this is inside a class), e.g. `speed` */
struct ReadExpression : Expression
{
    /** Reference name */
    std::string reference_name;
};

struct OptFunctionCall : Expression
{
	/** The name of the optional function. */
	std::string function_name;
	/** Parameter values */
	std::vector<std::unique_ptr<Expression>> parameters;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** The node representing `opt:empty` */
struct OptEmpty : Expression
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

}

#endif // STATEMENT_NODES_HPP