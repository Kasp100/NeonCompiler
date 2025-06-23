#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdint>
#include <optional>
#include <string>

namespace compiler
{

enum class TokenType
{
	IDENTIFIER,
	LITERAL_NUMBER,
	LITERAL_STRING,
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
	STATIC_ACCESSOR,
	VISIBILITY_PUBLIC,
	VISIBILITY_PRIVATE,
	VISIBILITY_PROTECTED,
	VISIBILITY_INNER,
	MEMBER_STATIC,
	MEMBER_CONST,
	MEMBER_EXTERNAL,
	MUTABLE_REFERENCE,
	MUTABLE_DECLARATION,
	REF_TYPE_OWN,
	REF_TYPE_SHARED,
	TYPE_CLASS,
	TYPE_INTERFACE,
	CONSTRUCTOR,
	REFERENCE_SUPER,
	REFERENCE_THIS,
	INHERITANCE_IMPLEMENTS,
	INHERITANCE_EXTENDS,
	DECL_CLASS_EXTENDABLE,
	DECL_METHOD_FINAL,
	DECL_METHOD_OVERRIDE,
	DECL_TYPE_COPYABLE,
	DECL_TYPE_SERIALISABLE,
	BOOL_TRUE,
	BOOL_FALSE,
	CALL_COMPILE_FUNCTION,
	DEFINE_COMPILE_FUNCTION,
	DEFINE_GRAMMAR_SET,
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

class Token {

public:
	explicit Token
	(
		TokenType type,
		std::uint32_t line,
		std::uint32_t column,
		std::uint32_t length,
		std::optional<std::string> lexeme = std::nullopt
	);

	TokenType get_type() const;
	std::uint32_t get_line() const;
	std::uint32_t get_column() const;
	std::uint32_t get_length() const;
	std::optional<std::string_view> get_lexeme() const;

	static std::optional<TokenType> keyword_to_token_type(std::string_view word);

private:
	TokenType type;
	std::uint32_t line, column, length;
	std::optional<std::string> lexeme;
};
}

#endif // TOKEN_HPP