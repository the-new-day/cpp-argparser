#pragma once

#include "Argument.hpp"

namespace ArgumentParser {

class ArgumentBuilder {
public:
    virtual ~ArgumentBuilder() = default;
    virtual const ArgumentInfo& GetInfo() = 0;
    virtual ArgumentParsingStatus GetValueStatus() = 0;

    virtual Argument* Build() = 0;
};

} // namespace ArgumentParser
