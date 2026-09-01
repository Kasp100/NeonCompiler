#include "package_member_id.hpp"

using namespace neon_compiler::ast;

PackageMemberID::PackageMemberID
(
	std::vector<std::string> init_parts
) :
	parts{std::move(init_parts)}
{}

std::string PackageMemberID::to_string() const
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

std::size_t PackageMemberID::get_length() const
{
	return parts.size();
}

std::string PackageMemberID::get_last_part() const
{
	return parts[parts.size() - 1];
}

PackageMemberID PackageMemberID::append(std::string part) const
{
	std::vector<std::string> new_parts{parts};
	new_parts.push_back(std::move(part));
	return PackageMemberID{new_parts};
}