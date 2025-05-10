#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdint>
#include <optional>
#include <string_view>

namespace compiler
{

enum class TokenType
{
	IDENTIFIER,
	LITERAL_NUMBER,
	LITERAL_STRING,
	BRACKET_CURLY_OPEN,
	BRACKET_CURLY_CLOSE,
	BRACKET_ROUND_OPEN,
	BRACKET_ROUND_CLOSE,
	SMALLER_THAN,
	GREATER_THAN,
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
	STMT_RETURN,
	STMT_MOVE,
	STMT_PASS,
	STMT_COPY,
	DECL_TYPE_COPYABLE,
	DECL_TYPE_SERIALISABLE,
	BOOL_TRUE,
	BOOL_FALSE,
	CALL_COMPILE_FUNCTION,
	DEFINE_COMPILE_FUNCTION,
	DEFINE_GRAMMAR_SET,
	CUSTOM_TOKEN
};

class Token {

public:
	explicit Token(TokenType type, std::uint32_t row, std::uint32_t col, std::optional<std::string_view> lexeme = std::nullopt);

	TokenType get_type() const;
	std::uint32_t get_row() const;
	std::uint32_t get_col() const;
	std::optional<std::string_view> get_lexeme() const;

	static std::optional<TokenType> keyword_to_token_type(std::string_view word);

private:
	TokenType type;
	std::uint32_t row, col;
	std::optional<std::string_view> lexeme;
};
}

#endif // TOKEN_HPP