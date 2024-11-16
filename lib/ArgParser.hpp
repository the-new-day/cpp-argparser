#pragma once

#include "Argument.hpp"
#include "SpecificArgumentBuilder.hpp"

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <iostream>

#define ARGPARSER_ADD_ARGUMENT(NewName, Type) \
inline SpecificArgumentBuilder<Type>& NewName(char short_name, \
                                       const std::string& long_name, \
                                       const std::string& description = "") { \
    return AddArgument<Type>(short_name, long_name, description); \
} \
\
inline SpecificArgumentBuilder<Type>& NewName(const std::string& long_name, \
                                       const std::string& description = "") { \
    return AddArgument<Type>(kNoShortName, long_name, description); \
} \

#define ARGPARSER_GET_VALUE(NewName, Type) \
inline Type NewName(const std::string& long_name, size_t index = 0) const { \
    return GetValue<Type>(long_name, index); \
} \

#define ARGPARSER_GET_VALUE_STATUS(NewName, Type) \
inline ArgumentStatus NewName(const std::string& long_name) const { \
    return GetValueStatus<Type>(long_name); \
} \

namespace ArgumentParser {

struct HelpArgument {
    char short_name = kNoShortName;
    std::string long_name;
    std::string description;
};

class ArgParser {
public:
    ArgParser(const std::string& program_name) : program_name_(program_name) {}
    ~ArgParser();

    template<typename T>
    SpecificArgumentBuilder<T>& AddArgument(char short_name,
                                            const std::string& long_name,
                                            const std::string& description = "");

    template<typename T>
    SpecificArgumentBuilder<T>& AddArgument(const std::string& long_name,
                                            const std::string& description = "");

    ARGPARSER_ADD_ARGUMENT(AddIntArgument, int32_t);
    ARGPARSER_ADD_ARGUMENT(AddStringArgument, std::string);
    ARGPARSER_ADD_ARGUMENT(AddFlag, bool);
    ARGPARSER_ADD_ARGUMENT(AddDoubleArgument, double);

    template<typename T>
    ArgumentStatus GetValueStatus(const std::string& long_name) const;

    ARGPARSER_GET_VALUE_STATUS(GetIntValueStatus, int32_t);
    ARGPARSER_GET_VALUE_STATUS(GetStringValueStatus, std::string);
    ARGPARSER_GET_VALUE_STATUS(GetFlagStatus, bool);
    ARGPARSER_GET_VALUE_STATUS(GetDoubleValueStatus, double);

    template<typename T>
    T GetValue(const std::string& long_name, size_t index = 0) const;

    ARGPARSER_GET_VALUE(GetIntValue, int32_t);
    ARGPARSER_GET_VALUE(GetStringValue, std::string);
    ARGPARSER_GET_VALUE(GetFlag, bool);
    ARGPARSER_GET_VALUE(GetDoubleValue, double);

    bool Parse(const std::vector<std::string>& argv);
    bool Parse(int argc, char** argv);

    void AddHelp(char short_name,
                 const std::string& long_name,
                 const std::string& description = "");

    void AddHelp(const std::string& long_name,
                 const std::string& description = "");

    bool Help() const;
    std::string HelpDescription() const;

    ArgumentParsingError GetError() const;
    bool HasError() const;

private:
    std::string program_name_;

    std::vector<Argument*> arguments_;
    std::vector<ArgumentBuilder*> argument_builders_;

    std::map<char, std::string> short_names_to_long_;
    std::map<std::string_view, size_t> arguments_indeces_;

    std::map<std::string, std::string> help_description_types_;

    ArgumentParsingError error_;

    void SetArguments();

    std::vector<std::string_view> GetLongNames(std::string_view argument) const;

    void ParsePositionalArguments(const std::vector<std::string>& argv,
                                  const std::vector<size_t>& positions);

    bool HandleErrors();
};

template<typename T>
SpecificArgumentBuilder<T>& ArgParser::AddArgument(char short_name,
                                                   const std::string& long_name,
                                                   const std::string& description) {
    auto* argument_builder = new SpecificArgumentBuilder<T>(short_name, long_name, description);

    arguments_indeces_[long_name] = argument_builders_.size();

    argument_builders_.push_back(argument_builder);
    short_names_to_long_[short_name] = long_name;

    return *argument_builder;
}

template<typename T>
SpecificArgumentBuilder<T>& ArgParser::AddArgument(const std::string& long_name,
                                                   const std::string& description) {
    return AddArgument<T>(kNoShortName, long_name, description);
}

template <typename T>
ArgumentStatus ArgParser::GetValueStatus(const std::string& long_name) const {
    size_t argument_index = arguments_indeces_.at(long_name);
    return arguments_.at(argument_index)->GetValueStatus();
}

template <typename T>
T ArgParser::GetValue(const std::string& long_name, size_t index) const {
    size_t argument_index = arguments_indeces_.at(long_name);
    auto* argument = static_cast<SpecificArgument<T>*>(arguments_.at(argument_index));

    return argument->GetValue(index);
}

} // namespace ArgumentParser

#undef ARGPARSER_ADD_ARGUMENT
#undef ARGPARSER_GET_VALUE
#undef ARGPARSER_GET_VALUE_STATUS
