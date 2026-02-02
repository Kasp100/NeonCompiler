#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdint>
#include <optional>
#include <string>
#include "../reading/source_position.hpp"

namespace neon_compiler
{

enum class TokenType
{
	END_OF_FILE,
	IDENTIFIER,
	LITERAL_NUMBER,
	LITERAL_STRING,
	LITERAL_CHARACTER,
	CUSTOM_TOKEN,
	BRACKET_CURLY_OPEN,
	BRACKET_CURLY_CLOSE,
	BRACKET_ROUND_OPEN,
	BRACKET_ROUND_CLOSE,
	SMALLER_THAN,
	GREATER_THAN,
	END_STATEMENT,
	COMMA,
	PACKAGE,
	IMPORT,
	STATIC_ACCESSOR,
	ACCESS_PUBLIC,
	ACCESS_PRIVATE,
	ACCESS_PROTECTED,
	ACCESS_EXCLUSIVE,
	SHALLOW,
	DEEP,
	MEMBER_PURE,
	MEMBER_CONST,
	MUTABLE_REFERENCE,
	MUTABLE_DECLARATION,
	VAR,
	REF_TYPE_OPTIONAL,
	REF_TYPE_OWN,
	REF_TYPE_SHARED,
	REF_TYPE_BORROW,
	PACKAGE_MEMBER_ENTRYPOINT,
	PACKAGE_MEMBER_PURE_FUNCTION_SET,
	PACKAGE_MEMBER_COMPILE_FUNCTION,
	PACKAGE_MEMBER_GRAMMAR_SET,
	PACKAGE_MEMBER_CLASS,
	PACKAGE_MEMBER_INTERFACE,
	MEMBER_ABSTRACT,
	CONSTRUCTOR,
	REFERENCE_SUPER,
	REFERENCE_THIS,
	INHERITANCE_IMPLEMENTS,
	INHERITANCE_EXTENDS,
	DECL_CLASS_EXTENDABLE,
	DECL_METHOD_OVERRIDE,
	DECL_METHOD_OVERRIDABLE,
	DECL_TYPE_COPYABLE,
	DECL_TYPE_SERIALISABLE,
	BOOL_TRUE,
	BOOL_FALSE,
	CALL_OPTIONAL,
	CALL_COMPILE_FUNCTION,
	RETURN_TYPE_VOID,
	STMT_IF,
	STMT_ELSE,
	STMT_FOR,
	STMT_FOR_EACH_IN,
	STMT_WHILE,
	STMT_SERIALISING,
	STMT_RETURN,
	STMT_MOVE,
	STMT_PASS,
	STMT_COPY,
};

class Token
{
public:
	explicit Token
	(
		TokenType type,
		reading::SourcePosition source_position,
		uint length,
		std::optional<std::string> lexeme = std::nullopt
	);

	TokenType get_type() const;
	reading::SourcePosition get_source_position() const;
	uint get_length() const;
	std::optional<std::string_view> get_lexeme() const;
	std::string get_location() const;

	static std::optional<TokenType> keyword_to_token_type(std::string_view word);

private:
	TokenType type;
	reading::SourcePosition source_position;
	uint length;
	std::optional<std::string> lexeme;
};
}

#endif // TOKEN_HPP