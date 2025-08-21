#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../ast_node.hpp"
#include "statement_nodes.hpp"

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

enum class PackageMemberPatternType
{
	PACKAGE_MEMBER,
	PACKAGE_WITHOUT_SUBPACKAGES,
	PACKAGE_WITH_SUBPACKAGES
};

struct PackageMemberPattern
{
	Identifier package_member_identifier;
	PackageMemberPatternType type;
	std::optional<PackageMemberPattern> extends = std::nullopt;
};

enum class PackageMemberAccessType
{
	PUBLIC,
	PRIVATE,
	EXCLUSIVE
};

struct PackageMemberAccess
{
	/** The privacy of the package member */
	PackageMemberAccessType type;
	/** Empty unless `type == PackageMemberAccessType::EXCLUSIVE`.
	 * Represents the set of package member patterns that determine who can use. */
	std::vector<PackageMemberPattern> patterns;
};

struct PackageMember : ASTNode {};

struct Type : PackageMember
{
	PackageMemberAccess access;

	/** Mapping from reference name to field declaration. */
	std::unordered_map<std::string, Field> fields;
	/** Mapping from method name to methods with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<Method>> methods;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct VariableDeclaration : ASTNode
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
	std::vector<VariableDeclaration> parameters;
	/** Method body. Empty means it's not implemented (an abstract method). */
	std::optional<CodeBlock> implementation;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

enum class MutabilityMode
{
	OWN,
	SHARED,
	BORROW
};

struct ReferenceType : ASTNode
{
	/** Whether this reference is `own`, `shared`, or `borrow` */
	MutabilityMode mutability;
	/** Whether mutations are allowed through this reference */
	bool mut;
	/** The name of the type */
	std::string type;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct CodeBlock : ASTNode
{
	std::vector<std::unique_ptr<Statement>> statements;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct PureFunctionSet : PackageMember
{
	PackageMemberAccess access;

	/** Mapping from function name to functions with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<PureFunction>> methods;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct PureFunction : ASTNode
{
	/** The reference type this pure function returns. */
	ReferenceType reference_type;
	/** Parameters */
	std::vector<ReferenceType> parameters;
	/** Function body */
	CodeBlock body;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GrammarSet : PackageMember
{
	PackageMemberAccess access;

	/** Grammar set rules */
	std::vector<GrammarRule> rules;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GrammarRule : ASTNode
{
	/** Subordination: e.g. `+` has a higher subordination (less precedence) than `*` */
	uint subordination;
	/** The reference type this pure function returns. */
	ReferenceType reference_type;
	/** Pattern to match */
	std::vector<std::unique_ptr<GrammarPatternPart>> pattern;
	/** Function body */
	CodeBlock body;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GrammarPatternPart : ASTNode {};

struct TokenPattern : GrammarPatternPart
{
	/** Token that needs to match */
	Token token;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct ParameterPattern : GrammarPatternPart
{
	/** Parameter */
	VariableDeclaration parameter;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

enum class CompileFunctionScope
{
	PACKAGE,
	TYPE_DEFINITION,
	PURE_FUNCTION_SET,
	CODE_BLOCK
};

struct CompileFunction : PackageMember
{
	PackageMemberAccess access;

	/** Where the compile function can be called from. Defines what a compile function can create and view. */
	CompileFunctionScope scope;
	/** Compile function body */
	CodeBlock body;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

}

#endif // NODES_HPP