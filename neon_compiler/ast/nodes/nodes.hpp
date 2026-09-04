#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include "../ast_node.hpp"
#include "../package_member_id.hpp"
#include "../../token.hpp"

// forward declaration
namespace neon_compiler::parser
{
	struct Operator;
}

namespace neon_compiler::ast::nodes
{

struct PackageMember : ASTNode
{
	PackageMemberID id;

	explicit PackageMember
	(
		PackageMemberID init_id
	) :
		id{std::move(init_id)}
	{}
};

struct Statement : ASTNode {};

/** Use statement to enable an operator module. */
struct UseStatement : Statement
{
	neon_compiler::ast::PackageMemberID operator_module_id;

	explicit UseStatement
	(
		neon_compiler::ast::PackageMemberID init_operator_module_id
	) :
		operator_module_id{std::move(init_operator_module_id)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Expression : ASTNode {};

struct FileNode : ASTNode
{
	/** File path */
	std::string file;
	/** File package declaration */
	neon_compiler::ast::PackageMemberID package{};
	/** Imports */
	std::vector<neon_compiler::ast::PackageMemberID> imports;
	/** File-level use statements activating operator modules */
	std::vector<neon_compiler::ast::nodes::UseStatement> use_statements;
	/** Package members declared in this file */
	std::vector<std::unique_ptr<PackageMember>> package_members;

	explicit FileNode
	(
		std::string init_file
	) :
		file{std::move(init_file)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct Root : ASTNode
{
	std::vector<std::unique_ptr<FileNode>> file_nodes;

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
	std::optional<PackageMemberID> package_member_identifier; // Empty if type is INHERITANCE_ONLY.
	std::optional<PackageMemberID> supertype = std::nullopt; // Present if matching by inheritance
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
	/** Whether this reference is `own`, `shared`, or `borrow`, empty means it will be inferred */
	std::optional<MutabilityMode> explicit_mutability_mode;
	/** Whether mutations are allowed through this reference */
	bool mut;
	/** The name of the type */
	PackageMemberID type_id;
	/** Generic arguments */
	std::vector<GenericArgument> generic_arguments;

	explicit ReferenceType
	(
		bool init_opt,
		std::optional<MutabilityMode> init_explicit_mutability_mode,
		bool init_mut,
		PackageMemberID init_type_id,
		std::vector<GenericArgument> init_generic_arguments = std::vector<GenericArgument>{}
	) :
		opt{init_opt},
		explicit_mutability_mode{init_explicit_mutability_mode},
		mut{init_mut},
		type_id{std::move(init_type_id)},
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
	/** Optional explicit reference name, empty means it is automatically inferred */
	std::optional<std::string> explicit_ref_name;
	/** Optional initialisation */
	std::unique_ptr<Expression> initialisation;

	explicit VariableDeclaration
	(
		bool init_var,
		ReferenceType init_reference_type,
		std::optional<std::string> init_explicit_ref_name,
		std::unique_ptr<Expression> init_initialisation = nullptr
	) :
		var{init_var},
		reference_type{std::move(init_reference_type)},
		explicit_ref_name{std::move(init_explicit_ref_name)},
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
		PackageMemberID init_id,
        VariableDeclaration init_constant
    ) :
		PackageMember
		{
			std::move(init_id)
		},
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

struct FieldDeclaration : ASTNode
{
	/** Whether it is reassignable after construction */
	bool var;
	/** The reference type of this field */
	ReferenceType reference_type;
	/** Field name */
	std::string name;

	explicit FieldDeclaration
	(
		bool init_var,
		ReferenceType init_reference_type,
		std::string init_name
	) :
		var{init_var},
		reference_type{std::move(init_reference_type)},
		name{std::move(init_name)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct FunctionDeclaration : ASTNode
{
	/** The access which determines who can use this function */
	Access access;
	/** The reference type this function returns. Empty means it's a `void` method. */
	std::optional<ReferenceType> return_type;
	/** Generic parameters */
	std::vector<GenericParameter> generic_parameters;
	/** Parameters */
	ParameterDeclarationList parameters;
	/** Whether this function may perform I/O */
	bool effect_io;

	explicit FunctionDeclaration
	(
		Access init_access,
		std::optional<ReferenceType> init_return_type,
		std::vector<GenericParameter> init_generic_parameters,
		ParameterDeclarationList init_parameters,
		bool init_effect_io
	) :
		access{std::move(init_access)},
		return_type{std::move(init_return_type)},
		generic_parameters{std::move(init_generic_parameters)},
		parameters{std::move(init_parameters)},
		effect_io{init_effect_io}
	{}
};

struct PackageFunctionDeclaration : FunctionDeclaration, PackageMember
{
	/** Whether this is a compile-time function */
	bool compile_time;
	/** Function body */
	CodeBlock body;

	explicit PackageFunctionDeclaration
	(
		Access init_access,
		bool init_compile_time,
		std::optional<ReferenceType> init_return_type,
		PackageMemberID init_id,
		std::vector<GenericParameter> init_generic_parameters,
		ParameterDeclarationList init_parameters,
		bool init_effect_io,
		CodeBlock init_body
	) :
		FunctionDeclaration
		{
			std::move(init_access),
			std::move(init_return_type),
			std::move(init_generic_parameters),
			std::move(init_parameters),
			init_effect_io
		},
		PackageMember
		{
			std::move(init_id)
		},
		compile_time{std::move(init_compile_time)},
		body{std::move(init_body)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct MethodDeclaration : FunctionDeclaration
{
	/** The name of the method */
	std::string name;
	/** Whether this method may mutate the object */
	bool mut;
	/** Whether this method may mutate the object */
	bool share_mut;
	/** Method body. Empty means it's not implemented (an abstract method). */
	std::optional<CodeBlock> implementation;

	explicit MethodDeclaration
	(
		Access init_access,
		std::optional<ReferenceType> init_return_type,
		std::string init_name,
		std::vector<GenericParameter> init_generic_parameters,
		ParameterDeclarationList init_parameters,
		bool init_mut,
		bool init_share_mut,
		bool init_io,
		std::optional<CodeBlock> init_implementation
	) :
		FunctionDeclaration
		{
			std::move(init_access),
			std::move(init_return_type),
			std::move(init_generic_parameters),
			std::move(init_parameters),
			init_io
		},
		name{std::move(init_name)},
		mut{init_mut},
		share_mut{init_share_mut},
		implementation{std::move(init_implementation)}
	{}

	void accept(ASTVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

struct TypeDeclaration : PackageMember
{
	/** The access which determines who can use this package member */
	Access access;
	/** The type's abstraction level */
	TypeAbstractionLevel abstraction_level;
	/** Constant declarations */
	std::vector<ConstantDeclaration> constants;
	/** Field declarations */
	std::vector<FieldDeclaration> fields;
	/** Method declarations */
	std::vector<MethodDeclaration> methods;

	explicit TypeDeclaration
	(
		Access init_access,
		TypeAbstractionLevel init_abstraction_level,
		PackageMemberID init_id,
		std::vector<ConstantDeclaration> init_constants,
		std::vector<FieldDeclaration> init_fields,
		std::vector<MethodDeclaration> init_methods
	) :
		PackageMember
		{
			std::move(init_id)
		},
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
	GIVE_AND_ASSIGN,
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

struct OperatorFunctionDeclaration : ASTNode
{
	/** The reference type this function returns. */
	ReferenceType return_type;
	/** Generic parameters */
	std::vector<GenericParameter> generic_parameters;
	/** Pattern to match, must use a valid operator */
	std::vector<OperatorFunctionPatternElement> pattern;
	/** Function body */
	CodeBlock body;

	explicit OperatorFunctionDeclaration
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
	std::vector<OperatorFunctionDeclaration> functions;

	explicit OperatorModule
	(
		Access init_access,
		PackageMemberID init_id,
		std::vector<OperatorDeclaration> init_operators,
		std::vector<OperatorFunctionDeclaration> init_functions
	) :
		PackageMember
		{
			std::move(init_id)
		},
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