#pragma once

#include "Argument.hpp"
#include "SpecificArgument.hpp"

#include <string>
#include <vector>
#include <map>
#include <cstdint>

#define ARGPARSER_ADD_ARGUMENT(NewName, Type) \
inline SpecificArgument<Type>& NewName(char short_name, \
                                       const std::string& long_name, \
                                       const std::string& description = "") { \
    return AddArgument<Type>(short_name, long_name, description); \
} \
\
inline SpecificArgument<Type>& NewName(const std::string& long_name, \
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
    explicit ArgParser(const std::string& program_name, const std::string& program_description = "");
    ~ArgParser();

    template<typename T>
    SpecificArgument<T>& AddArgument(char short_name,
                                            const std::string& long_name,
                                            const std::string& description = "");

    template<typename T>
    SpecificArgument<T>& AddArgument(const std::string& long_name,
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

    template<typename T>
    void SetTypeAlias(const std::string& alias);

private:
    std::string program_name_;
    std::string program_description_;

    std::vector<Argument*> arguments_;

    std::map<char, std::string> short_names_to_long_;
    std::map<std::string_view, size_t> arguments_indeces_;

    std::map<std::string, std::string> help_description_types_;

    ArgumentParsingError error_;

    bool need_help_ = false;
    std::string help_argument_name_;

    void RefreshParser();

    std::vector<std::string_view> GetLongNames(std::string_view argument) const;

    void ParsePositionalArguments(const std::vector<std::string>& argv,
                                  const std::vector<size_t>& positions);

    bool HandleErrors();

    std::string GetArgumentDescription(const Argument* argument,
                                       size_t max_argument_names_length) const;

    std::string GetArgumentNamesDescription(const Argument* argument) const;
};

template<typename T>
SpecificArgument<T>& ArgParser::AddArgument(char short_name,
                                                   const std::string& long_name,
                                                   const std::string& description) {
    auto* argument = new SpecificArgument<T>(short_name, long_name, description);

    arguments_indeces_[long_name] = arguments_.size();

    arguments_.push_back(argument);
    short_names_to_long_[short_name] = long_name;

    return *argument;
}

template<typename T>
SpecificArgument<T>& ArgParser::AddArgument(const std::string& long_name,
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

template <typename T>
void ArgParser::SetTypeAlias(const std::string& alias) {
    help_description_types_[typeid(T).name()] = alias;
}

} // namespace ArgumentParser

#undef ARGPARSER_ADD_ARGUMENT
#undef ARGPARSER_GET_VALUE
#undef ARGPARSER_GET_VALUE_STATUS
