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

struct StaticFunctionCall : Expression
{
	/** The path and name of a specific static function, relative to the current package member. */
	std::string function_path;
	/** Parameter values */
	std::vector<std::unique_ptr<Expression>> parameters;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct MethodCall : Expression
{
	/** The object on which the method is called. `nullptr` means the object is "this". */
	std::unique_ptr<Expression> receiver;
	/** The name of a specific method. */
	std::string method_name;
	/** Parameter values */
	std::vector<std::unique_ptr<Expression>> arguments;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct StaticFieldCall : Expression
{
	/** The path and name of a specific static field, relative to the current package member. */
	std::string static_field_path;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct ReferenceCall : Expression
{
	/** The object on which the field is called. Empty if the object is "this". */
	std::optional<std::unique_ptr<Expression>> callee;
	/** The name of a specific field, local variable, or constant. */
	std::string reference_name;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
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