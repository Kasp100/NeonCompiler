#ifndef PACKAGE_MEMBER_ID_HPP
#define PACKAGE_MEMBER_ID_HPP

#include <vector>
#include <string>

namespace neon_compiler::ast
{

constexpr std::string_view UNDEFINED_PACKAGE_MEMBER_NAME = "undefined";

class PackageMemberID
{
public:
	explicit PackageMemberID(std::vector<std::string> init_parts = std::vector<std::string>{std::string{UNDEFINED_PACKAGE_MEMBER_NAME}});
	std::string to_string() const;
	std::size_t get_length() const;
	std::string get_last_part() const;
	PackageMemberID append(std::string part) const;
private:
	/** Parts that together make an identifier.
	 * Identifiers for static objects (incl. packages and package members like types) may contain multiple parts.
	 * The symbol `::` separates the parts in source code.
	 * Example 1: `main::subpkg` in source code becomes "main" followed by "subpkg" in this vector.
	 * This is unambiguously static.
	 * Example 2: `my_variable` in source code becomes "my_variable" (single element) in this vector.
	 * It is later determined whether this is a static or non-static reference */
	std::vector<std::string> parts;
};

}

#endif // PACKAGE_MEMBER_ID_HPP