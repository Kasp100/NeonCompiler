#ifndef TOKENISATION_ERROR_HPP
#define TOKENISATION_ERROR_HPP

#include <cstdint>
#include <string_view>
#include "../../reading/source_position.hpp"

namespace neon_compiler::lexer
{

struct TokenisationError
{
    const reading::SourcePosition source_position;
    const std::string_view message;
};

}

#endif // TOKENISATION_ERROR_HPP