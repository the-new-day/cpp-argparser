#include "SpecificArgument.hpp"
#include "utils/utils.hpp"

#include <cstdint>
#include <string_view>

namespace ArgumentParser {

template<>
std::optional<int32_t> ParseValue<int32_t>(std::string_view value_string) {
    auto parsing_result = ParseNumber<int32_t>(value_string);
    if (!parsing_result.has_value()) {
        return std::nullopt;
    }

    return parsing_result.value();
}

template<>
std::optional<std::string> ParseValue<std::string>(std::string_view value_string) {
    return std::string(value_string);
}

template<>
std::optional<bool> ParseValue<bool>(std::string_view value_string) {
    if (!value_string.empty()) {
        return std::nullopt;
    }

    return true;
}

template<>
std::optional<double> ParseValue<double>(std::string_view value_string) {
    auto parsing_result = ParseNumber<double>(value_string);
    if (!parsing_result.has_value()) {
        return std::nullopt;
    }

    return parsing_result.value();
}
    
} // namespace ArgumentParser
