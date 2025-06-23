#include "token.hpp"

#include <string_view>

using namespace compiler;
using namespace std;

Token::Token(TokenType type, uint32_t line, uint32_t column, uint32_t length, optional<string> lexeme)
		: type(type), line(line), column(column), length(length), lexeme(lexeme) {}

TokenType Token::get_type() const
{
	return type;
}

uint32_t Token::get_line() const
{
	return line;
}

uint32_t Token::get_column() const
{
	return column;
}

uint32_t Token::get_length() const
{
	return length;
}

optional<string_view> Token::get_lexeme() const
{
	if (lexeme.has_value()) {
		return string_view(*lexeme);
	}
	return std::nullopt;
}

optional<TokenType> Token::keyword_to_token_type(string_view word)
{
	     if (word == "pkg")              return TokenType::PACKAGE;
	else if (word == "public")           return TokenType::VISIBILITY_PUBLIC;
	else if (word == "private")          return TokenType::VISIBILITY_PRIVATE;
	else if (word == "protected")        return TokenType::VISIBILITY_PROTECTED;
	else if (word == "inner")            return TokenType::VISIBILITY_INNER;
	else if (word == "static")           return TokenType::MEMBER_STATIC;
	else if (word == "const")            return TokenType::MEMBER_CONST;
	else if (word == "external")         return TokenType::MEMBER_EXTERNAL;
	else if (word == "mut:")             return TokenType::MUTABLE_REFERENCE;
	else if (word == "mut")              return TokenType::MUTABLE_DECLARATION;
	else if (word == "own")              return TokenType::REF_TYPE_OWN;
	else if (word == "shared")           return TokenType::REF_TYPE_SHARED;
	else if (word == "class")            return TokenType::TYPE_CLASS;
	else if (word == "interface")        return TokenType::TYPE_INTERFACE;
	else if (word == "constructor")      return TokenType::CONSTRUCTOR;
	else if (word == "super")            return TokenType::REFERENCE_SUPER;
	else if (word == "this")             return TokenType::REFERENCE_THIS;
	else if (word == "impl")             return TokenType::INHERITANCE_IMPLEMENTS;
	else if (word == "extends")          return TokenType::INHERITANCE_EXTENDS;
	else if (word == "extendable")       return TokenType::DECL_CLASS_EXTENDABLE;
	else if (word == "final")            return TokenType::DECL_METHOD_FINAL;
	else if (word == "override")         return TokenType::DECL_METHOD_OVERRIDE;
	else if (word == "copyable")         return TokenType::DECL_TYPE_COPYABLE;
	else if (word == "serialisable")     return TokenType::DECL_TYPE_SERIALISABLE;
	else if (word == "true")             return TokenType::BOOL_TRUE;
	else if (word == "false")            return TokenType::BOOL_FALSE;
	else if (word == "auto:")            return TokenType::CALL_COMPILE_FUNCTION;
	else if (word == "compile_function") return TokenType::DEFINE_COMPILE_FUNCTION;
	else if (word == "grammar")          return TokenType::DEFINE_GRAMMAR_SET;
	else if (word == "void")             return TokenType::RETURN_TYPE_VOID;
	else if (word == "if")               return TokenType::STMT_IF;
	else if (word == "else")             return TokenType::STMT_ELSE;
	else if (word == "for")              return TokenType::STMT_FOR;
	else if (word == "for_each_in")      return TokenType::STMT_FOR_EACH_IN;
	else if (word == "while")            return TokenType::STMT_WHILE;
	else if (word == "serialising")      return TokenType::STMT_SERIALISING;
	else if (word == "ret")              return TokenType::STMT_RETURN;
	else if (word == "move")             return TokenType::STMT_MOVE;
	else if (word == "pass")             return TokenType::STMT_PASS;
	else if (word == "copy")             return TokenType::STMT_COPY;

	return nullopt;
}