#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include <vector>
#include <string>
#include "ast/nodes/nodes.hpp"
#include "../logging/logger.hpp"
#include "token.hpp"

namespace neon_compiler
{

class Compiler
{
public:
	explicit Compiler(std::shared_ptr<logging::Logger> logger);

	void read_file(std::unique_ptr<std::istream> stream, std::string_view file_name);
	void build() const;
	void generate_analysis() const;

private:
	std::shared_ptr<logging::Logger> logger;
	std::unordered_map<std::string, std::vector<neon_compiler::Token>> file_tokens;
	std::shared_ptr<neon_compiler::ast::nodes::Root> root_node;
};

}

#endif // COMPILER_HPP