#pragma once

#include "Argument.hpp"
#include "ConcreteArgumentBuilder.hpp"

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace ArgumentParser {

class ArgParser {
public:
    ArgParser(const std::string& program_name) 
        : program_name_(program_name) {}
    ~ArgParser();

    template<typename T>
    ConcreteArgumentBuilder<T>& AddArgument(const std::string& long_name,
                                            char short_name,
                                            const std::string& description = "");

    template<typename T>
    ConcreteArgumentBuilder<T>& AddArgument(const std::string& long_name,
                                            const std::string& description = "");

    

private:
    std::string program_name_;

    std::map<std::string, ArgumentBuilder*> argument_builders_;

    std::map<std::string, Argument*> arguments_;
    std::map<char, std::string> short_names_to_long_;

    std::vector<std::string> allowed_typenames_;
};

template<typename T>
ConcreteArgumentBuilder<T>& ArgParser::AddArgument(const std::string& long_name,
                                                   char short_name,
                                                   const std::string& description) {
    auto* argument_builder = new ConcreteArgumentBuilder<T>(long_name, short_name, description);

    argument_builders_[long_name] = argument_builder;
    short_names_to_long_[short_name] = long_name;

    return *argument_builder;
}

template<typename T>
ConcreteArgumentBuilder<T>& ArgParser::AddArgument(const std::string& long_name,
                                                   const std::string& description) {
    return AddArgument<T>(long_name, kNoShortName, description);
}

} // namespace ArgumentParser
