#ifndef TOKEN_READER_HPP
#define TOKEN_READER_HPP

#include <memory>
#include <span>
#include "token.hpp"

namespace neon_compiler
{
	class TokenReader
	{
	public:
		explicit TokenReader(std::span<const neon_compiler::Token> tokens);

		const neon_compiler::Token& consume(uint offset = 0);
		const neon_compiler::Token& peek(uint offset = 0) const;
		bool end_of_file_reached() const;

	private:
		std::span<const neon_compiler::Token> tokens;
		uint reading_index = 0;
	};

}

#endif // TOKEN_READER_HPP