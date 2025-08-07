#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include <vector>
#include <string>
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
	std::vector<Token> tokens;
	std::string latest_file;
};

}

#endif // COMPILER_HPP