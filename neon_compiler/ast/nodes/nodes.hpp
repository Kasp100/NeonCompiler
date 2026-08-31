#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include "../ast_node.hpp"
#include "../identifiers.hpp"
#include "../../token.hpp"
#include <iostream>


// forward declaration
namespace neon_compiler::parser
{
	struct Operator;
}

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

	Root() = default;

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
	std::vector<PackageMemberPattern> patterns{};
};

enum class MutabilityMode
{
	OWN,
	SHARED,
	BORROW
};

struct GenericParameter
{
	std::string type;
	std::string reference_name;
	std::vector<std::string> supertypes;
};

struct GenericArgument
{
	/** A reference to a type/constant or a value from a literal */
	std::string value;
	/** Whether `value` is a reference to a type/constant */
	bool is_reference{false};
	/** If `value` is a reference to a type, these are the generic arguments associated with it */
	std::vector<GenericArgument> nested_generic_args{};
};

struct ReferenceType : ASTNode
{
	/** Whether the reference is optional */
	bool opt;
	/** Whether this reference is `own`, `shared`, or `borrow` */
	MutabilityMode mutability;
	/** Whether mutations are allowed through this reference */
	bool mut;
	/** The name of the type */
	std::string type;
	/** Inferred name */
	std::string inferred_name;
	/** Generic arguments */
	std::vector<GenericArgument> generic_arguments;

	explicit ReferenceType
	(
		bool init_opt,
		MutabilityMode init_mutability,
		bool init_mut,
		std::string init_type,
		std::string init_inferred_name = std::string{},
		std::vector<GenericArgument> init_generic_arguments = std::vector<GenericArgument>{}
	) :
		opt{init_opt},
		mutability{init_mutability},
		mut{init_mut},
		type{std::move(init_type)},
		inferred_name{std::move(init_inferred_name)},
		generic_arguments{std::move(init_generic_arguments)}
	{}

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
	std::unique_ptr<Expression> initialisation;

	explicit VariableDeclaration
	(
		bool init_var,
		ReferenceType init_reference_type,
		std::string init_reference_name,
		std::unique_ptr<Expression> init_initialisation = nullptr
	) :
		var{init_var},
		reference_type{std::move(init_reference_type)},
		reference_name{std::move(init_reference_name)},
		initialisation{std::move(init_initialisation)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

using ParameterDeclarationList = std::vector<VariableDeclaration>;

struct ConstantDeclaration : PackageMember
{
	/** The access which determines who can use this constant */
	Access access;
	/** Constant */
	VariableDeclaration constant;

    explicit ConstantDeclaration
    (
		Access init_access,
        VariableDeclaration init_constant
    ) :
		access{std::move(init_access)},
        constant{std::move(init_constant)}
    {}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct CodeBlock : ASTNode
{
	std::vector<std::unique_ptr<Statement>> statements;

	explicit CodeBlock
	(
		std::vector<std::unique_ptr<Statement>> init_statements
	) :
		statements{std::move(init_statements)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

enum class TypeAbstractionLevel
{
	INTERFACE,
	ABSTRACT_CLASS,
	CLASS
};

struct Field : ASTNode
{
	/** Whether it is reassignable after construction */
	bool var;
	/** The reference type of this field. */
	ReferenceType reference_type;

	explicit Field
	(
		bool init_var,
		ReferenceType init_reference_type
	) :
		var{init_var},
		reference_type{init_reference_type}
	{}

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
	std::optional<ReferenceType> return_type;
	/** Whether this method may mutate the object. */
	bool mutating;
	/** Whether this method may perform IO. */
	bool io;
	/** Parameters */
	ParameterDeclarationList parameters;
	/** Method body. Empty means it's not implemented (an abstract method). */
	std::optional<CodeBlock> implementation;

	explicit Method
	(
		Access init_access,
		std::optional<ReferenceType> init_return_type,
		bool init_mutating,
		bool init_io,
		ParameterDeclarationList init_parameters,
		std::optional<CodeBlock> init_implementation
	) :
		access{std::move(init_access)},
		return_type{std::move(init_return_type)},
		mutating{init_mutating},
		io{init_io},
		parameters{std::move(init_parameters)},
		implementation{std::move(init_implementation)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Type : PackageMember
{
	/** The access which determines who can use this package member */
	Access access;
	/** The type's abstraction level */
	TypeAbstractionLevel abstraction_level;
	/** Mapping from reference name to constant declaration. */
	std::unordered_map<std::string, ConstantDeclaration> constants;
	/** Mapping from reference name to field declaration. */
	std::unordered_map<std::string, Field> fields;
	/** Mapping from method name to methods with the same name, but different parameters (overloads). */
	std::unordered_map<std::string, std::vector<Method>> methods;

	explicit Type
	(
		Access init_access,
		TypeAbstractionLevel init_abstraction_level,
		std::unordered_map<std::string, ConstantDeclaration> init_constants,
		std::unordered_map<std::string, Field> init_fields,
		std::unordered_map<std::string, std::vector<Method>> init_methods
	) :
		access{std::move(init_access)},
		abstraction_level{init_abstraction_level},
		constants{std::move(init_constants)},
		fields{std::move(init_fields)},
		methods{std::move(init_methods)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Function : ASTNode
{
	/** The access which determines who can use this function */
	Access access;
	/** Whether this is a compile-time function */
	bool compile_time;
	/** The type this function returns. */
	ReferenceType return_type;
	/** Generic parameters */
	std::vector<GenericParameter> generic_parameters;
	/** Parameters */
	ParameterDeclarationList parameters;
	/** Whether this function may perform I/O */
	bool effect_io;
	/** Function body */
	CodeBlock body;

	explicit Function
	(
		Access init_access,
		bool init_compile_time,
		ReferenceType init_return_type,
		std::vector<GenericParameter> init_generic_parameters,
		ParameterDeclarationList init_parameters,
		bool init_effect_io,
		CodeBlock init_body
	) :
		access{std::move(init_access)},
		compile_time{init_compile_time},
		return_type{std::move(init_return_type)},
		generic_parameters{std::move(init_generic_parameters)},
		parameters{std::move(init_parameters)},
		effect_io{init_effect_io},
		body{std::move(init_body)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

/** Functions with the same name, but different parameter types */
struct FunctionOverloadList : PackageMember
{
	std::vector<Function> functions;

	explicit FunctionOverloadList
	(
		std::vector<Function> init_functions
	) :
		functions{std::move(init_functions)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

enum class OperatorAssociativity
{
	NONE,
	LEFT,
	RIGHT
};

enum class BuiltinOperatorKind
{
	NOT_BUILT_IN,
	MEMBER_ACCESS,
	ASSIGNMENT,
	GIVE,
	CHECK_PRESENCE,
	CHECK_ABSENCE,
	FALLBACK
};

struct TokenPattern
{
	/** Token type that needs to match */
	neon_compiler::TokenType token_type;
	/** The optional lexeme that needs to match */
	std::optional<std::string> lexeme;
	
	explicit TokenPattern
	(
		neon_compiler::TokenType init_token_type,
		std::optional<std::string> init_lexeme = std::nullopt
	) :
		token_type{init_token_type},
		lexeme{std::move(init_lexeme)}
	{}
};

struct OperatorSyntaxParameter{};

struct OperatorFunctionParameter
{
	/** Parameter */
	neon_compiler::ast::nodes::VariableDeclaration parameter;
	
	explicit OperatorFunctionParameter(
		neon_compiler::ast::nodes::VariableDeclaration& init_parameter
	) :
		parameter{std::move(init_parameter)}
	{}
};

using OperatorSyntaxPatternElement = std::variant<TokenPattern, OperatorSyntaxParameter>;

using OperatorFunctionPatternElement = std::variant<TokenPattern, OperatorFunctionParameter>;

struct OperatorDeclaration : ASTNode
{
	/** The sequence of tokens or parameters which makes this operator */
	std::vector<OperatorSyntaxPatternElement> pattern;
	/** Subordination: e.g. `+` has a higher subordination (less precedence) than `*` */
	uint subordination;
	/** Associativity with this operator */
	OperatorAssociativity associativity;
	/** The kind of built-in operator. NOT_BUILT_IN means it's not built-in. */
	BuiltinOperatorKind builtin_operator_kind;

	explicit OperatorDeclaration
	(
		std::vector<OperatorSyntaxPatternElement> init_pattern,
		uint init_subordination,
		OperatorAssociativity init_associativity,
		BuiltinOperatorKind init_builtin_operator_kind
	) :
		pattern{std::move(init_pattern)},
		subordination{init_subordination},
		associativity{init_associativity},
		builtin_operator_kind{init_builtin_operator_kind}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct OperatorFunction : ASTNode
{
	/** The reference type this function returns. */
	ReferenceType return_type;
	/** Generic parameters */
	std::vector<GenericParameter> generic_parameters;
	/** Pattern to match, must use a valid operator */
	std::vector<OperatorFunctionPatternElement> pattern;
	/** Function body */
	CodeBlock body;

	explicit OperatorFunction
	(
		ReferenceType init_return_type,
		std::vector<GenericParameter> init_generic_parameters,
		std::vector<OperatorFunctionPatternElement> init_pattern,
		CodeBlock init_body
	) :
		return_type{std::move(init_return_type)},
		generic_parameters{std::move(init_generic_parameters)},
		pattern{std::move(init_pattern)},
		body{std::move(init_body)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct OperatorModule : PackageMember
{
	/** The access which determines who can use this expression grammar */
	Access access;
	/** Operator declarations */
	std::vector<OperatorDeclaration> operators;
	/** Operator functions */
	std::vector<OperatorFunction> functions;

	explicit OperatorModule
	(
		Access init_access,
		std::vector<OperatorDeclaration> init_operators,
		std::vector<OperatorFunction> init_functions
	) :
		access{std::move(init_access)},
		operators{std::move(init_operators)},
		functions{std::move(init_functions)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

}

#endif // NODES_HPP