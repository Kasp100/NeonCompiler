#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <vector>
#include <string>

namespace neon_compiler::ast
{

struct Identifier
{
	/** Parts that together make an identifier.
	 * Identifiers for static objects (incl. packages and package members like types) may contain multiple parts.
	 * The symbol `::` separates the parts in source code.
	 * Example 1: `main::subpkg` in source code becomes "main" followed by "subpkg" in this vector.
	 * This is unambiguously static.
	 * Example 2: `my_variable` in source code becomes "my_variable" (single element) in this vector.
	 * It is later determined whether this is a static or non-static reference */
	std::vector<std::string> parts = std::vector<std::string>{};

	std::string to_string()
	{
		std::string s{};
		bool first = true;
		for(const std::string& part : parts)
		{
			if(first)
			{
				first = false;
			}
			else
			{
				s += "::";
			}
			s += part;
		}
		return s;
	}
};

}

#endif // IDENTIFIERS_HPP