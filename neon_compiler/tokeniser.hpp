#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <memory>
#include "../reading/char_reader.hpp"

namespace compiler
{

class Tokeniser
{
public:
    explicit Tokeniser(std::unique_ptr<reading::CharReader> reader);
    void run();
};

}

#endif // TOKENISER_HPP