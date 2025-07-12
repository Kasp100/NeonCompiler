#ifndef STATEMENT_NODES_HPP
#define STATEMENT_NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include "../ast_node.hpp"
#include "../../token.hpp"

namespace neon_compiler::ast::nodes
{

struct Statement : ASTNode {};

struct DiscardExpression : Statement
{
    /** The expression which will be evaluated, discarding the result. Typically done with `void` calls. */
	std::unique_ptr<Expression> expression;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct VariableDeclaration : Statement
{
    /** Whether this variable can be reassigned after initialisation */
    bool var;
    /** The reference type */
	ReferenceType reference_type;
    /** The reference name */
    std::string reference_name;
    /** Optional initialisation */
    std::optional<std::unique_ptr<Expression>> initialisation;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Call to a compile function */
struct AutoCall : Statement
{
	std::string function_name;
	std::vector<std::vector<neon_compiler::Token>> parameters;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Expression : ASTNode {};

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
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct MethodCall : Expression
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct StaticFieldCall : Expression
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct FieldCall : Expression
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct OptFunctionCall : Expression
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct OptFieldCall : Expression
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

}

#endif // STATEMENT_NODES_HPP