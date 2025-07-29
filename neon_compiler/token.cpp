#include "token.hpp"

#include <string_view>

using namespace neon_compiler;

Token::Token(TokenType type, uint32_t line, uint32_t column, uint32_t length, std::optional<std::string> lexeme)
	: type{type}, line{line}, column{column}, length{length}, lexeme{lexeme} {}

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

std::string Token::get_location() const
{
	std::string location{};
	if(type == TokenType::END_OF_FILE)
	{
		location += "at the end of file";
	}
	else
	{
		location += "at line ";
		location += std::to_string(get_line());
		location += ", column ";
		location += std::to_string(get_column());
		location += " (length: ";
		location += std::to_string(get_length());
		location += "), in file";
	}
	return location;
}

std::optional<std::string_view> Token::get_lexeme() const
{
	if (lexeme.has_value()) {
		return std::string_view(*lexeme);
	}
	return std::nullopt;
}

std::optional<TokenType> Token::keyword_to_token_type(std::string_view word)
{
	     if (word == "pkg")               return TokenType::PACKAGE;
	else if (word == "public")            return TokenType::VISIBILITY_PUBLIC;
	else if (word == "private")           return TokenType::VISIBILITY_PRIVATE;
	else if (word == "protected")         return TokenType::VISIBILITY_PROTECTED;
	else if (word == "exclusive")         return TokenType::VISIBILITY_EXCLUSIVE;
	else if (word == "pure")              return TokenType::MEMBER_PURE;
	else if (word == "const")             return TokenType::MEMBER_CONST;
	else if (word == "mut:")              return TokenType::MUTABLE_REFERENCE;
	else if (word == "mut")               return TokenType::MUTABLE_DECLARATION;
	else if (word == "opt")               return TokenType::REF_TYPE_OPTIONAL;
	else if (word == "own")               return TokenType::REF_TYPE_OWN;
	else if (word == "shared")            return TokenType::REF_TYPE_SHARED;
	else if (word == "borrow")            return TokenType::REF_TYPE_BORROW;
	else if (word == "entrypoint")        return TokenType::PACKAGE_MEMBER_ENTRYPOINT;
	else if (word == "pure_function_set") return TokenType::PACKAGE_MEMBER_PURE_FUNCTION_SET;
	else if (word == "compile_function")  return TokenType::PACKAGE_MEMBER_COMPILE_FUNCTION;
	else if (word == "grammar_set")       return TokenType::PACKAGE_MEMBER_GRAMMAR_SET;
	else if (word == "class")             return TokenType::PACKAGE_MEMBER_CLASS;
	else if (word == "interface")         return TokenType::PACKAGE_MEMBER_INTERFACE;
	else if (word == "abstract")          return TokenType::MEMBER_ABSTRACT;
	else if (word == "constructor")       return TokenType::CONSTRUCTOR;
	else if (word == "super")             return TokenType::REFERENCE_SUPER;
	else if (word == "this")              return TokenType::REFERENCE_THIS;
	else if (word == "impl")              return TokenType::INHERITANCE_IMPLEMENTS;
	else if (word == "extends")           return TokenType::INHERITANCE_EXTENDS;
	else if (word == "extendable")        return TokenType::DECL_CLASS_EXTENDABLE;
	else if (word == "override")          return TokenType::DECL_METHOD_OVERRIDE;
	else if (word == "overridable")       return TokenType::DECL_METHOD_OVERRIDABLE;
	else if (word == "copyable")          return TokenType::DECL_TYPE_COPYABLE;
	else if (word == "serialisable")      return TokenType::DECL_TYPE_SERIALISABLE;
	else if (word == "true")              return TokenType::BOOL_TRUE;
	else if (word == "false")             return TokenType::BOOL_FALSE;
	else if (word == "opt:")              return TokenType::CALL_OPTIONAL;
	else if (word == "auto:")             return TokenType::CALL_COMPILE_FUNCTION;
	else if (word == "void")              return TokenType::RETURN_TYPE_VOID;
	else if (word == "if")                return TokenType::STMT_IF;
	else if (word == "else")              return TokenType::STMT_ELSE;
	else if (word == "for")               return TokenType::STMT_FOR;
	else if (word == "for_each_in")       return TokenType::STMT_FOR_EACH_IN;
	else if (word == "while")             return TokenType::STMT_WHILE;
	else if (word == "serialising")       return TokenType::STMT_SERIALISING;
	else if (word == "ret")               return TokenType::STMT_RETURN;
	else if (word == "pass")              return TokenType::STMT_PASS;
	else if (word == "copy")              return TokenType::STMT_COPY;

	return std::nullopt;
}