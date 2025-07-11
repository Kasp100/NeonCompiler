#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../ast_node.hpp"

namespace neon_compiler::ast::nodes
{

struct Root : ASTNode
{
	std::unordered_map<std::string, std::unique_ptr<PackageMember>> members;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct PackageMember : ASTNode {};

struct Type : PackageMember
{
	/** Mapping from reference name to field declaration. */
	std::unordered_map<std::string, Field> fields;
	/** Mapping from method name to methods with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<Method>> methods;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Field : ASTNode
{
	/** Whether it is reassignable after construction */
	bool var;
	/** The reference type of this field. */
	ReferenceType reference_type;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Method : ASTNode
{
	/** The reference type this method returns. Empty means it's a `void` method. */
	std::optional<ReferenceType> reference_type;
	/** Whether this method may mutate the object. */
	bool mutating;
	/** Parameters */
	std::vector<ReferenceType> parameters;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct ReferenceType : PackageMember
{
	/** `true` for `shared`, `false` for `own` */
	bool shared;
	/** Whether the reference is mutable */
	bool mut;
	/** The name of the type */
	std::string type;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct PureFunctionSet : PackageMember
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GrammarSet : PackageMember
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct CompileFunction : PackageMember
{
	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

}

#endif // NODES_HPP