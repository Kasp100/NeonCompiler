#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../ast_node.hpp"
#include "../identifiers.hpp"
#include "../../token.hpp"

namespace neon_compiler::ast::nodes
{

struct PackageMember : ASTNode {};

struct Statement : ASTNode {};

struct Expression : ASTNode {};

struct Root : ASTNode
{
	/** Mapping from package member identifier to package member */
	std::unordered_map<std::string, std::unique_ptr<PackageMember>> package_members;
	/** Mapping from file path to package member identifiers */
	std::unordered_map<std::string, std::vector<std::string>> file_package_members;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

enum class PackageMemberPatternType
{
	INHERITANCE_ONLY,
	PACKAGE_MEMBER,
	PACKAGE_WITHOUT_SUBPACKAGES,
	PACKAGE_WITH_SUBPACKAGES
};

struct PackageMemberPattern
{
	PackageMemberPatternType type;
	std::optional<Identifier> package_member_identifier; // Empty if type is INHERITANCE_ONLY.
	std::optional<Identifier> supertype = std::nullopt; // Present if matching by inheritance
};

enum class AccessType
{
	PUBLIC,
	PRIVATE,
	PROTECTED,
	EXCLUSIVE
};

struct Access
{
	/** The access type: `public`, `private`, `protected` (not for package members and non-constant fields), or `exclusive` */
	AccessType type;
	/** Empty unless `type == AccessType::EXCLUSIVE`.
	 * Represents the set of package member patterns that determine who can use. */
	std::vector<PackageMemberPattern> patterns;
};

struct Type : PackageMember
{
	/** The access which determines who can use this package member */
	Access access;
	/** Mapping from reference name to field declaration. */
	std::unordered_map<std::string, Field> fields;
	/** Mapping from method name to methods with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<Method>> methods;
	/** Mapping from reference name to constant declaration. */
	std::unordered_map<std::string, Constant> constants;
	/** Mapping from pure function name to pure functions with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<PureFunction>> pure_functions;

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
	/** The access which determines who can use this method */
	Access access;
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

struct Constant : ASTNode
{
	/** The access which determines who can use this constant */
	Access access;
	/** The type of this constant */
	std::string type;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct PureFunctionSet : PackageMember
{
	/** The access which determines who can use this pure function set */
	Access access;
	/** Mapping from function name to functions with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<PureFunction>> methods;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct PureFunction : ASTNode
{
	/** The access which determines who can use this pure function */
	Access access;
	/** The immutable tyoe this pure function returns. */
	std::string return_type;
	/** Parameters (should be immutable values) */
	std::vector<VariableDeclaration> parameters;
	/** Pure function body. Empty means it's not implemented (an abstract pure function). */
	std::optional<CodeBlock> implementation;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GrammarSet : PackageMember
{
	/** The access which determines who can use this grammar set */
	Access access;
	/** Grammar set rules */
	std::vector<GrammarRule> rules;

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct GrammarPatternPart : ASTNode {};

struct TokenPattern : GrammarPatternPart
{
	/** Token that needs to match */
	neon_compiler::Token token;

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

enum class CompileFunctionScope
{
	PACKAGE,
	TYPE_DEFINITION,
	PURE_FUNCTION_SET,
	CODE_BLOCK
};

struct CompileFunction : PackageMember
{
	/** The access which determines who can use this compile function */
	Access access;
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