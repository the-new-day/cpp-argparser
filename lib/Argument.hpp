#pragma once

#include <string>
#include <string_view>
#include <cstddef>
#include <vector>
#include <expected>

namespace ArgumentParser {

const char kNoShortName = 0;

enum class ArgumentStatus {
    kSuccess,
    kNoArgument,
    kInvalidArgument,
    kInsufficient
};

enum class ArgumentParsingErrorType {
    kInsufficent,
    kInvalidArgument,
    kUnknownArgument,
    kSuccess
};

struct ArgumentParsingError {
    std::string argument_string{};
    ArgumentParsingErrorType status = ArgumentParsingErrorType::kSuccess;
    std::string argument_name{};
};

struct ArgumentInfo {
    std::string long_name;
    char short_name = kNoShortName;
    std::string description;
    std::string type;

    size_t minimum_values = 0;
    bool is_multi_value = false;
    bool is_positional = false;
    bool has_default = false;
    bool has_store_values = false;
    bool has_store_value = false;
};

class Argument {
public:
    virtual ~Argument() = default;

    virtual const ArgumentInfo& GetInfo() const = 0;
    virtual const std::string& GetType() const = 0;
    virtual ArgumentStatus GetValueStatus() const = 0;
    virtual size_t GetValuesSet() const = 0;

    virtual std::expected<size_t, ArgumentParsingError> ParseArgument(const std::vector<std::string>& argv,
                                                                      size_t position) = 0;

    virtual void ClearValuesStorage() = 0;
};

} // namespace ArgumentParser
