#include "ConcreteArgument.hpp"
#include "utils/utils.hpp"

#include <cstdint>

namespace ArgumentParser {

template<>
void ConcreteArgument<int32_t>::ParseValue(std::string_view value_string) {
    auto parsing_result = ParseNumber<int32_t>(value_string);
    if (!parsing_result.has_value()) {
        value_status_ = ArgumentParsingStatus::kInvalidArgument;
        return;
    }

    value_ = parsing_result.value();
}

template<>
void ConcreteArgument<std::string>::ParseValue(std::string_view value_string) {
    value_ = value_string;
}

template<>
void ConcreteArgument<bool>::ParseValue(std::string_view value_string) {
    if (!value_string.empty()) {
        value_status_ = ArgumentParsingStatus::kInvalidArgument;
        return;
    }

    value_ = true;
}
    
} // namespace ArgumentParser

