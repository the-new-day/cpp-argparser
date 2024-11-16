#pragma once

#include "Argument.hpp"

namespace ArgumentParser {

class ArgumentBuilder {
public:
    virtual ~ArgumentBuilder() = default;
    virtual const ArgumentInfo& GetInfo() const = 0;
    virtual ArgumentStatus GetValueStatus() const = 0;

    virtual Argument* Build() = 0;
};

} // namespace ArgumentParser
