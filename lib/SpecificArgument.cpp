#include "SpecificArgument.hpp"
#include "utils/utils.hpp"

#include <cstdint>
#include <string_view>

namespace ArgumentParser {

template<>
std::expected<int32_t, ArgumentParsingErrorType> ParseValue<int32_t>(std::string_view value_string) {
    auto parsing_result = ParseNumber<int32_t>(value_string);
    if (!parsing_result.has_value()) {
        return std::unexpected(ArgumentParsingErrorType::kInvalidArgument);
    }

    return parsing_result.value();
}

template<>
std::expected<std::string, ArgumentParsingErrorType> ParseValue<std::string>(std::string_view value_string) {
    return std::string(value_string);
}

template<>
std::expected<bool, ArgumentParsingErrorType> ParseValue<bool>(std::string_view value_string) {
    if (!value_string.empty()) {
        return std::unexpected(ArgumentParsingErrorType::kInvalidArgument);
    }

    return true;
}

template<>
std::expected<double, ArgumentParsingErrorType> ParseValue<double>(std::string_view value_string) {
    auto parsing_result = ParseNumber<double>(value_string);
    if (!parsing_result.has_value()) {
        return std::unexpected(ArgumentParsingErrorType::kInvalidArgument);
    }

    return parsing_result.value();
}
    
} // namespace ArgumentParser
