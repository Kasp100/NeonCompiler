#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <memory>
#include "../logging/logger.hpp"
#include "../reading/char_reader.hpp"

namespace compiler
{

class Tokeniser
{
public:
    explicit Tokeniser(std::shared_ptr<logging::Logger> logger, std::unique_ptr<reading::CharReader> reader);
    void run();
private:
    std::shared_ptr<logging::Logger> logger;
    std::unique_ptr<reading::CharReader> reader;
};

}

#endif // TOKENISER_HPP