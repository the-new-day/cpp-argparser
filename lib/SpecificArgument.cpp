#include "SpecificArgument.hpp"
#include "utils/utils.hpp"

#include <cstdint>
#include <string_view>

namespace ArgumentParser {

template<>
ArgumentParsingErrorType SpecificArgument<int32_t>::ParseValue(std::string_view value_string) {
    auto parsing_result = ParseNumber<int32_t>(value_string);
    if (!parsing_result.has_value()) {
        value_status_ = ArgumentStatus::kInvalidArgument;
        return ArgumentParsingErrorType::kInvalidArgument;
    }

    value_ = parsing_result.value();
    return ArgumentParsingErrorType::kSuccess;
}

template<>
ArgumentParsingErrorType SpecificArgument<std::string>::ParseValue(std::string_view value_string) {
    value_ = value_string;
    return ArgumentParsingErrorType::kSuccess;
}

template<>
ArgumentParsingErrorType SpecificArgument<bool>::ParseValue(std::string_view value_string) {
    if (!value_string.empty()) {
        value_status_ = ArgumentStatus::kInvalidArgument;
        return ArgumentParsingErrorType::kInvalidArgument;
    }

    value_ = true;
    return ArgumentParsingErrorType::kSuccess;
}

template<>
ArgumentParsingErrorType SpecificArgument<double>::ParseValue(std::string_view value_string) {
    auto parsing_result = ParseNumber<double>(value_string);
    if (!parsing_result.has_value()) {
        value_status_ = ArgumentStatus::kInvalidArgument;
        return ArgumentParsingErrorType::kInvalidArgument;
    }

    value_ = parsing_result.value();
    return ArgumentParsingErrorType::kSuccess;
}
    
} // namespace ArgumentParser
